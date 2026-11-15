#pragma once
#include "plugin.hpp"
#include <atomic>

// ---------------------------------------------------------------------------
// Tehom <-> DIR. IN / DIR. OUT link
//
// The expanders don't need to touch Tehom: each one links to a Tehom anywhere in
// the patch (the nearest one by default, or one picked from its context menu)
// and exchanges data through a TehomBus owned by that Tehom.
//
// Thread safety: Rack processes all modules for a frame, then synchronizes
// before the next frame. Each buffer is double-buffered by frame parity: a
// writer fills slot [frame & 1] while readers read slot [(frame + 1) & 1], which
// was completed in the previous frame. Readers and writers never touch the same
// slot within a frame. Each slot is stamped with the frame it was written in so
// stale data (writer removed, unlinked or not processing) is ignored. This
// gives one sample of latency, the same as Rack's adjacent-expander messages.
// ---------------------------------------------------------------------------

// Tehom -> DIR. OUT
//   pre  = pre-mixer, pre-effects: each deck after its Source crossfade, read at
//          the un-warbled playhead, with no Tone/saturation and no noise/return
//   post = the deck through everything: warble, XY mixer level (and pan when
//          "XY pad pans audio" is on), Tone filter/saturation (run per deck), and
//          the deck's XY share of the noise/return
struct TehomOutMessage {
	float preL[4] = {};
	float preR[4] = {};
	float postL[4] = {};
	float postR[4] = {};
};

// DIR. IN -> Tehom
// Left and right are independent: a patched DIR. IN jack replaces only that
// side of that deck's input; the other side keeps Tehom's main input.
struct TehomInMessage {
	float inL[4] = {};
	float inR[4] = {};
	bool connectedL[4] = {};
	bool connectedR[4] = {};
};

struct TehomBus {
	TehomOutMessage out[2];
	int64_t outFrame[2] = {-1, -1};
	// Last frame a DIR. OUT read this bus; Tehom skips the extra per-deck work when nobody listens
	std::atomic<int64_t> outReaderFrame{-1};

	TehomInMessage in[2];
	int64_t inFrame[2] = {-1, -1};
	// Only one DIR. IN may drive a Tehom at a time
	std::atomic<int64_t> inWriter{-1};
	std::atomic<int64_t> inWriterFrame{-1};

	// Previous frame's slot, if it was written in the previous frame
	const TehomOutMessage* readOut(int64_t frame) {
		outReaderFrame.store(frame, std::memory_order_relaxed);
		int s = (int)((frame + 1) & 1);
		return (outFrame[s] == frame - 1) ? &out[s] : nullptr;
	}
	const TehomInMessage* readIn(int64_t frame) const {
		int s = (int)((frame + 1) & 1);
		return (inFrame[s] == frame - 1) ? &in[s] : nullptr;
	}
	TehomOutMessage& writeOut(int64_t frame) {
		int s = (int)(frame & 1);
		outFrame[s] = frame;
		return out[s];
	}
	// Returns nullptr if another DIR. IN is currently driving this Tehom
	TehomInMessage* writeIn(int64_t frame, int64_t writerId) {
		int64_t cur = inWriter.load();
		if (cur != writerId) {
			bool otherActive = (cur != -1) && (inWriterFrame.load() >= frame - 1);
			if (otherActive || !inWriter.compare_exchange_strong(cur, writerId))
				return nullptr;
		}
		inWriterFrame.store(frame);
		int s = (int)(frame & 1);
		inFrame[s] = frame;
		return &in[s];
	}
};

// Tehom inherits this so expanders can reach its bus via dynamic_cast
struct TehomBusHost {
	TehomBus tehomBus;
	virtual ~TehomBusHost() {}
};

// Base for DIR. IN / DIR. OUT: link management + persistence
struct TehomLinkedModule : MoffModule {
	// User choice: -1 = nearest Tehom (automatic), otherwise a Tehom module ID
	std::atomic<int64_t> linkedId{-1};
	// Tehom actually in use, resolved on the UI thread (see TehomLinkedWidget)
	std::atomic<int64_t> resolvedId{-1};
	// True if this DIR. IN was refused because another DIR. IN drives the same Tehom
	std::atomic<bool> blocked{false};

	Module* cachedModule = nullptr;
	TehomBus* cachedBus = nullptr;

	// Audio thread: the linked Tehom's bus, or nullptr
	TehomBus* getBus() {
		int64_t id = resolvedId.load();
		if (id < 0)
			id = linkedId.load();  // headless / before first UI resolve
		if (id < 0)
			return nullptr;
		Module* m = APP->engine->getModule_NoLock(id);
		if (!m || m->model != modelTehom)
			return nullptr;
		if (m != cachedModule) {
			cachedModule = m;
			TehomBusHost* host = dynamic_cast<TehomBusHost*>(m);
			cachedBus = host ? &host->tehomBus : nullptr;
		}
		return cachedBus;
	}

	json_t* toJson() override {
		json_t* rootJ = MoffModule::toJson();
		json_object_set_new(rootJ, "tehomLinkId", json_integer(linkedId.load()));
		return rootJ;
	}
	void fromJson(json_t* rootJ) override {
		MoffModule::fromJson(rootJ);
		json_t* j = json_object_get(rootJ, "tehomLinkId");
		if (j)
			linkedId.store(json_integer_value(j));
	}
};

struct TehomLinkedWidget : MoffModuleWidget {
	int resolveCounter = 0;

	// All Tehom widgets currently in the rack
	static std::vector<ModuleWidget*> tehomWidgets() {
		std::vector<ModuleWidget*> out;
		if (!APP->scene || !APP->scene->rack)
			return out;
		for (ModuleWidget* mw : APP->scene->rack->getModules())
			if (mw->module && mw->module->model == modelTehom)
				out.push_back(mw);
		return out;
	}

	static std::string describe(ModuleWidget* mw) {
		int row = (int)std::round(mw->box.pos.y / RACK_GRID_HEIGHT) + 1;
		int hp = (int)std::round(mw->box.pos.x / RACK_GRID_WIDTH);
		return string::f("Tehom (row %d, %d HP)", row, hp);
	}

	// UI thread: choose the Tehom to use (explicit link if valid, else nearest)
	void resolve() {
		TehomLinkedModule* m = dynamic_cast<TehomLinkedModule*>(module);
		if (!m)
			return;
		std::vector<ModuleWidget*> tehoms = tehomWidgets();
		int64_t want = m->linkedId.load();
		for (ModuleWidget* mw : tehoms) {
			if (mw->module->id == want) {
				m->resolvedId.store(want);
				return;
			}
		}
		// Automatic (or the chosen Tehom was deleted): nearest Tehom by panel distance
		Vec me = box.getCenter();
		ModuleWidget* best = nullptr;
		float bestDist = INFINITY;
		for (ModuleWidget* mw : tehoms) {
			float d = mw->box.getCenter().minus(me).norm();
			if (d < bestDist) {
				bestDist = d;
				best = mw;
			}
		}
		m->resolvedId.store(best ? best->module->id : -1);
	}

	void step() override {
		// Re-resolve a few times per second (handles moves, adds, deletes)
		if (module && (resolveCounter++ % 15) == 0)
			resolve();
		MoffModuleWidget::step();
	}

	void appendContextMenu(Menu* menu) override {
		MoffModuleWidget::appendContextMenu(menu);
		TehomLinkedModule* m = dynamic_cast<TehomLinkedModule*>(module);
		if (!m)
			return;
		resolve();
		menu->addChild(new MenuSeparator);

		std::vector<ModuleWidget*> tehoms = tehomWidgets();
		int64_t resolved = m->resolvedId.load();
		std::string status = "Not linked (no Tehom in patch)";
		for (ModuleWidget* mw : tehoms)
			if (mw->module->id == resolved)
				status = "Linked to " + describe(mw);
		if (m->blocked.load())
			status += " - another DIR. IN is driving it";
		menu->addChild(createMenuLabel(status));

		menu->addChild(createSubmenuItem("Link to Tehom", "", [=](Menu* sub) {
			sub->addChild(createCheckMenuItem("Nearest (automatic)", "",
				[=]() { return m->linkedId.load() < 0; },
				[=]() { m->linkedId.store(-1); }));
			for (ModuleWidget* mw : tehomWidgets()) {
				int64_t id = mw->module->id;
				sub->addChild(createCheckMenuItem(describe(mw), "",
					[=]() { return m->linkedId.load() == id; },
					[=]() { m->linkedId.store(id); }));
			}
		}));
	}
};
