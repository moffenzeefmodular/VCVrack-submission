#include "plugin.hpp"


struct INTENSIFIES : Module {
	enum ParamId {
		PATH1_PARAM,
		PATH1_5_PARAM,
		PATH1_5_7_5_PARAM,
		PATH1_5_7_PARAM,
		PATH1_5_29_PARAM,
		PATH1_4_PARAM,
		PATH1_5_4_PARAM,
		PATH1_5_25_PARAM,
		PATH1_5_5_PARAM,
		PATH1_5_83_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		PATH1_8_0_9_INPUT,
		PATH1_5_8_INPUT,
		PATH1_8_0_9_7_INPUT,
		PATH1_8_0_INPUT,
		PATH1_8_INPUT,
		PATH1_8_5_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		PATH1_8_9_OUTPUT,
		PATH1_8_9_6_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		PATH1_5_2_LIGHT,
		PATH1_5_84_LIGHT,
		PATH1_5_87_LIGHT,
		LIGHTS_LEN
	};

	INTENSIFIES() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PATH1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PATH1_5_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PATH1_5_7_5_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PATH1_5_7_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PATH1_5_29_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PATH1_4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PATH1_5_4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PATH1_5_25_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PATH1_5_5_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PATH1_5_83_PARAM, 0.f, 1.f, 0.f, "");
		configInput(PATH1_8_0_9_INPUT, "");
		configInput(PATH1_5_8_INPUT, "");
		configInput(PATH1_8_0_9_7_INPUT, "");
		configInput(PATH1_8_0_INPUT, "");
		configInput(PATH1_8_INPUT, "");
		configInput(PATH1_8_5_INPUT, "");
		configOutput(PATH1_8_9_OUTPUT, "");
		configOutput(PATH1_8_9_6_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct INTENSIFIESWidget : ModuleWidget {
	INTENSIFIESWidget(INTENSIFIES* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/INTENSIFIES.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(18.44, 34.222)), module, INTENSIFIES::PATH1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(48.122, 35.871)), module, INTENSIFIES::PATH1_5_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(79.661, 35.678)), module, INTENSIFIES::PATH1_5_7_5_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(107.163, 50.988)), module, INTENSIFIES::PATH1_5_7_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(67.346, 59.976)), module, INTENSIFIES::PATH1_5_29_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(38.99, 62.038)), module, INTENSIFIES::PATH1_4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(87.182, 77.145)), module, INTENSIFIES::PATH1_5_4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.278, 93.051)), module, INTENSIFIES::PATH1_5_25_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(65.92, 94.064)), module, INTENSIFIES::PATH1_5_5_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(98.918, 97.521)), module, INTENSIFIES::PATH1_5_83_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(26.707, 11.585)), module, INTENSIFIES::PATH1_8_0_9_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(97.418, 26.501)), module, INTENSIFIES::PATH1_5_8_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(63.064, 35.057)), module, INTENSIFIES::PATH1_8_0_9_7_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(53.375, 76.187)), module, INTENSIFIES::PATH1_8_0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(110.044, 76.084)), module, INTENSIFIES::PATH1_8_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(80.75, 101.603)), module, INTENSIFIES::PATH1_8_5_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.254, 62.619)), module, INTENSIFIES::PATH1_8_9_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.251, 104.885)), module, INTENSIFIES::PATH1_8_9_6_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(109.201, 35.91)), module, INTENSIFIES::PATH1_5_2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(87.126, 48.785)), module, INTENSIFIES::PATH1_5_84_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(21.257, 70.01)), module, INTENSIFIES::PATH1_5_87_LIGHT));
	}
};


Model* modelINTENSIFIES = createModel<INTENSIFIES, INTENSIFIESWidget>("INTENSIFIES");