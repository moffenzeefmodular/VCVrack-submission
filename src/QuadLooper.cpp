#include "plugin.hpp"


struct QuadLooper : Module {
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

	QuadLooper() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SLEW_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LOOPSIZE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LOOPPOSITION_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LOOPXFADE_PARAM, 0.f, 1.f, 0.f, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct QuadLooperWidget : ModuleWidget {
	QuadLooperWidget(QuadLooper* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/panels/QuadLooper.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(76.2, 24.892)), module, QuadLooper::SLEW_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(57.7, 87.737)), module, QuadLooper::LOOPSIZE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(76.2, 87.737)), module, QuadLooper::LOOPPOSITION_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(94.7, 87.737)), module, QuadLooper::LOOPXFADE_PARAM));
	}
};


Model* modelQuadLooper = createModel<QuadLooper, QuadLooperWidget>("QuadLooper");