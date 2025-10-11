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

struct CKSSHorizontal : app::SvgSwitch {
	CKSSHorizontal() {
		shadow->opacity = 0.0;
		addFrame(Svg::load(asset::system("res/ComponentLibrary/CKSSThreeHorizontal_0.svg")));
		addFrame(Svg::load(asset::system("res/ComponentLibrary/CKSSThreeHorizontal_2.svg")));
	}
};

struct _2Pos : SvgSwitch {
    _2Pos() {
        momentary = false; // true for momentary behavior
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SubMiniToggle_0.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SubMiniToggle_2.svg")));
    }
};

struct _2PosHorizontal : SvgSwitch {
    _2PosHorizontal() {
        momentary = false; // true for momentary behavior
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SubMiniToggleH_0.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SubMiniToggleH_2.svg")));
    }
};

struct _3Pos : SvgSwitch {
    _3Pos() {
        momentary = false; // true for momentary behavior
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SubMiniToggle_0.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SubMiniToggle_1.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SubMiniToggle_2.svg")));
    }
};

struct _3PosHorizontal : SvgSwitch {
    _3PosHorizontal() {
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

struct _9mmKnob : SvgKnob {
	widget::SvgWidget* bg;
	_9mmKnob() {
		minAngle = -0.85 * M_PI;
		maxAngle = 0.85 * M_PI;
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/9mm_Knob.svg")));
	}
};

struct StargazerDavies : SvgKnob {
	widget::SvgWidget* bg;
	StargazerDavies() {
		minAngle = -0.85 * M_PI;
		maxAngle = 0.85 * M_PI;
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/StargazerDavies.svg")));
	}
};

struct StargazerTinyKnob : SvgKnob {
	widget::SvgWidget* bg;
	StargazerTinyKnob() {
		minAngle = -0.85 * M_PI;
		maxAngle = 0.85 * M_PI;
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/StargazerTinyKnob.svg")));
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