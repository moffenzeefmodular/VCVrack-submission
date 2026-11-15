#pragma once
#include <rack.hpp>

using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
extern Model* modelBadIdea9;
extern Model* modelBadIdea1800;
extern Model* modelBobcat;
extern Model* modelBusMult;
extern Model* modelCount;
extern Model* modelDeviant;
extern Model* modelDialUp;
extern Model* modelGMO;
extern Model* modelKriket;
extern Model* modelMito;
extern Model* modelMoffenmix;
extern Model* modelMongrel;
extern Model* modelMuskrat;
extern Model* modelSimplify;
extern Model* model_2hpBlank;
extern Model* model_14hpBlank;
extern Model* modelSwarm;
extern Model* modelINTENSIFIES;
extern Model* modelTheRunner;
extern Model* modelStargazer;
extern Model* modelKleztizer;
extern Model* modelTantz;
extern Model* modelQuadDeviant;
extern Model* modelTehom;
extern Model* modelDitDit;
extern Model* modelParticles;
extern Model* modelTehomOut;
extern Model* modelTehomIn;

// ---------------------------------------------------------------------------
// Per-module panel theme override
//
// Rack's themed widgets (ThemedSvgPanel, ThemedScrew, ThemedPJ301MPort, and our
// own LEDBezelSilver / Tehom display) pick light or dark by reading the global
// settings::preferDarkPanels. MoffModuleWidget temporarily sets that global to
// the module's chosen theme while its own subtree steps and draws, then restores
// it, so every themed child follows the per-module choice with no new panels.
// ---------------------------------------------------------------------------
enum PanelTheme {
	PANEL_THEME_AUTO = 0,   // follow Rack's "Use dark panels if available"
	PANEL_THEME_LIGHT = 1,
	PANEL_THEME_DARK = 2,
};

struct MoffModule : Module {
	int panelTheme = PANEL_THEME_AUTO;

	// Stored at the top level of the module JSON (outside "data") so it doesn't
	// interfere with any module's own dataToJson()/dataFromJson().
	json_t* toJson() override {
		json_t* rootJ = Module::toJson();
		json_object_set_new(rootJ, "moffPanelTheme", json_integer(panelTheme));
		return rootJ;
	}
	void fromJson(json_t* rootJ) override {
		Module::fromJson(rootJ);
		json_t* themeJ = json_object_get(rootJ, "moffPanelTheme");
		if (themeJ)
			panelTheme = clamp((int)json_integer_value(themeJ), 0, 2);
	}
};

struct MoffModuleWidget : ModuleWidget {
	// RAII guard: applies the module's theme to the global, restores on exit
	struct ThemeScope {
		bool active = false;
		bool saved = false;
		explicit ThemeScope(Module* module) {
			MoffModule* m = dynamic_cast<MoffModule*>(module);
			if (!m || m->panelTheme == PANEL_THEME_AUTO)
				return;
			saved = settings::preferDarkPanels;
			settings::preferDarkPanels = (m->panelTheme == PANEL_THEME_DARK);
			active = true;
		}
		~ThemeScope() {
			if (active)
				settings::preferDarkPanels = saved;
		}
	};

	void step() override {
		ThemeScope scope(module);
		ModuleWidget::step();
	}
	void draw(const DrawArgs& args) override {
		ThemeScope scope(module);
		ModuleWidget::draw(args);
	}
	void drawLayer(const DrawArgs& args, int layer) override {
		ThemeScope scope(module);
		ModuleWidget::drawLayer(args, layer);
	}

	void appendContextMenu(Menu* menu) override {
		MoffModule* m = dynamic_cast<MoffModule*>(module);
		if (!m)
			return;
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Panel theme",
			{"Rack default", "Light", "Dark"}, &m->panelTheme));
	}
};

struct CKSSHorizontal : app::SvgSwitch {
	CKSSHorizontal() {
		shadow->opacity = 0.0;
		addFrame(Svg::load(asset::system("res/ComponentLibrary/CKSSThreeHorizontal_0.svg")));
		addFrame(Svg::load(asset::system("res/ComponentLibrary/CKSSThreeHorizontal_2.svg")));
	}
};

struct TwoPos : SvgSwitch {
    TwoPos() {
        momentary = false; // true for momentary behavior
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SubMiniToggle_0.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SubMiniToggle_2.svg")));
    }
};

struct TwoPosHorizontal : SvgSwitch {
    TwoPosHorizontal() {
        momentary = false; // true for momentary behavior
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SubMiniToggleH_0.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SubMiniToggleH_2.svg")));
    }
};

struct ThreePos : SvgSwitch {
    ThreePos() {
        momentary = false; // true for momentary behavior
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SubMiniToggle_0.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SubMiniToggle_1.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SubMiniToggle_2.svg")));
    }
};

struct ThreePosHorizontal : SvgSwitch {
    ThreePosHorizontal() {
        momentary = false; // true for momentary behavior
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SubMiniToggleH_0.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SubMiniToggleH_1.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SubMiniToggleH_2.svg")));
    }
};

struct BefacoBigKnobBlack : SvgKnob {
	widget::SvgWidget* bg;
	BefacoBigKnobBlack() {
		minAngle = -0.85 * M_PI;
		maxAngle = 0.85 * M_PI;
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/BefacoBigKnobBlack.svg")));

		bg = new widget::SvgWidget;
		fb->addChildBelow(bg, tw);
		bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/BefacoBigKnobBlack_bg.svg")));
	}
};

struct MediumKnob : SvgKnob {
	widget::SvgWidget* bg;
	MediumKnob() {
		minAngle = -0.85 * M_PI;
		maxAngle = 0.85 * M_PI;
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/MediumKnob.svg")));

		bg = new widget::SvgWidget;
		fb->addChildBelow(bg, tw);
		bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/MediumKnob_bg.svg")));
	}
};

struct NineMMKnob : SvgKnob {
	widget::SvgWidget* bg;
	NineMMKnob() {
		minAngle = -0.85 * M_PI;
		maxAngle = 0.85 * M_PI;
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/9mm_knob.svg")));
	}
};

struct TehomBigKnob : RoundBlackKnob {
	TehomBigKnob() {
		box.size = mm2px(Vec(11.f, 11.f));
		shadow->box.size = box.size;
	}
};

struct StargazerDavies : SvgKnob {
	widget::SvgWidget* bg;
	StargazerDavies() {
		minAngle = -0.85 * M_PI;
		maxAngle = 0.85 * M_PI;
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/Davies1900hBlack.svg")));

		bg = new widget::SvgWidget;
		fb->addChildBelow(bg, tw);
		bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/Davies1900hBlack_bg.svg")));
	}
};

struct StargazerTinyKnob : SvgKnob {
	widget::SvgWidget* bg;
	StargazerTinyKnob() {
		minAngle = -0.85 * M_PI;
		maxAngle = 0.85 * M_PI;
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/StargazerTinyKnob.svg")));

		bg = new widget::SvgWidget;
		fb->addChildBelow(bg, tw);
		bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/StargazerTinyKnob_bg.svg")));
	}
};

struct StargazerLFOKnob : SvgKnob {
	widget::SvgWidget* bg;
	StargazerLFOKnob() {
		minAngle = -0.85 * M_PI;
		maxAngle = 0.85 * M_PI;
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/StargazerLFOKnob.svg")));
	}
};


struct LEDBezelSilver : SvgKnob {
	widget::SvgWidget* bg;
	bool lastDark = false;
	LEDBezelSilver() {
		lastDark = settings::preferDarkPanels;
		setSvg(Svg::load(asset::plugin(pluginInstance, lastDark
			? "res/components/LEDBezel-Dark.svg"
			: "res/components/LEDBezelSilver.svg")));
	}
	void step() override {
		bool dark = settings::preferDarkPanels;
		if (dark != lastDark) {
			setSvg(Svg::load(asset::plugin(pluginInstance, dark
				? "res/components/LEDBezel-Dark.svg"
				: "res/components/LEDBezelSilver.svg")));
			lastDark = dark;
		}
		SvgKnob::step();
	}
};

struct LEDBezelDark : SvgKnob {
	widget::SvgWidget* bg;
	LEDBezelDark() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/LEDBezel-Dark.svg")));
	}
};

struct TehomKnob : SvgKnob {
	widget::SvgWidget* bg;
	TehomKnob() {
		minAngle = -0.85 * M_PI;
		maxAngle = 0.85 * M_PI;
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/TehomKnob.svg")));

		bg = new widget::SvgWidget;
		fb->addChildBelow(bg, tw);
		bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/TehomKnobBG.svg")));
	}
};

struct TehomSmallKnob : SvgKnob {
	widget::SvgWidget* bg;
	TehomSmallKnob() {
		minAngle = -0.85 * M_PI;
		maxAngle = 0.85 * M_PI;
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/TehomSmallKnob.svg")));

		bg = new widget::SvgWidget;
		fb->addChildBelow(bg, tw);
		bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/TehomSmallKnobBG.svg")));
	}
};