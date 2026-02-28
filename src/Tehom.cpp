#include "plugin.hpp"


struct Tehom : Module {
	enum ParamId {
		SLEW_PARAM,
		LOOPSIZE_PARAM,
		LOOPPOSITION_PARAM,
		LOOPXFADE_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Tehom() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SLEW_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LOOPSIZE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LOOPPOSITION_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LOOPXFADE_PARAM, 0.f, 1.f, 0.f, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct TehomWidget : ModuleWidget {
	TehomWidget(Tehom* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/panels/Tehom.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	}
};


Model* modelTehom = createModel<Tehom, TehomWidget>("Tehom");