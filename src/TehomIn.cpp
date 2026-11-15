#include "plugin.hpp"
#include "TehomExpander.hpp"

// DIR. IN (slug "TehomIn"): 4hp direct-inputs expander for Tehom, stereo per deck.
// Links to a Tehom anywhere in the patch (nearest by default, or chosen from the
// context menu); it doesn't need to touch Tehom. Left and right are independent:
// a patched jack replaces only that side of that deck's input (for monitoring and
// recording); an unpatched side keeps Tehom's main input for that side.
struct TehomIn : TehomLinkedModule {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		DECK1L_INPUT,
		DECK1R_INPUT,
		DECK2L_INPUT,
		DECK2R_INPUT,
		DECK3L_INPUT,
		DECK3R_INPUT,
		DECK4L_INPUT,
		DECK4R_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	TehomIn() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		for (int i = 0; i < 4; i++) {
			std::string deck = "Deck " + std::to_string(i + 1);
			configInput(DECK1L_INPUT + 2 * i, deck + " left (replaces Tehom's main left input for this deck when patched)");
			configInput(DECK1R_INPUT + 2 * i, deck + " right (replaces Tehom's main right input for this deck when patched)");
		}
	}

	void send(const ProcessArgs& args, bool active) {
		TehomBus* bus = getBus();
		if (!bus) {
			blocked.store(false);
			return;
		}
		TehomInMessage* msg = bus->writeIn(args.frame, id);
		blocked.store(msg == nullptr);
		if (!msg)
			return;
		for (int i = 0; i < 4; i++) {
			Input& inL = inputs[DECK1L_INPUT + 2 * i];
			Input& inR = inputs[DECK1R_INPUT + 2 * i];
			msg->connectedL[i] = active && inL.isConnected();
			msg->connectedR[i] = active && inR.isConnected();
			msg->inL[i] = msg->connectedL[i] ? inL.getVoltage() : 0.f;
			msg->inR[i] = msg->connectedR[i] ? inR.getVoltage() : 0.f;
		}
	}

	void process(const ProcessArgs& args) override {
		send(args, true);
	}

	// Bypassed: release all decks back to Tehom's main inputs
	void processBypass(const ProcessArgs& args) override {
		send(args, false);
	}
};

struct TehomInWidget : TehomLinkedWidget {
	TehomInWidget(TehomIn* module) {
		setModule(module);
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/panels/TehomIn.svg"),
			asset::plugin(pluginInstance, "res/panels/TehomIn-dark.svg")
		));

		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// Positions match the "components" layer of res/panels/TehomIn.svg
		const float deckY[4] = {0.f, 27.6f, 55.2f, 82.8f};
		for (int i = 0; i < 4; i++) {
			float dy = deckY[i];
			addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(5.56f, 31.4f + dy)), module, TehomIn::DECK1L_INPUT + 2 * i));
			addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(14.76f, 31.4f + dy)), module, TehomIn::DECK1R_INPUT + 2 * i));
		}
	}
};

Model* modelTehomIn = createModel<TehomIn, TehomInWidget>("TehomIn");
