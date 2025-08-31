#include "plugin.hpp"


struct INTENSIFIES : Module {
	enum ParamId {
		CARRIER_PARAM,
		FXBYPASS_PARAM,
		CARRIERRANGE_PARAM,
		FXVOLUME_PARAM,
		GAIN_PARAM,
		MODULATORENGAGE_PARAM,
		GAINRANGE_PARAM,
		MODULATOR_PARAM,
		MODULATORRANGE_PARAM,
		SYNTHVOLUME_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CARRIERCV_INPUT,
		FXVOLUMECV_INPUT,
		GAINCV_INPUT,
		MODULATORCV_INPUT,
		AUDIOIN_INPUT,
		SYNTHVOLUMECV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		FXOUT_OUTPUT,
		SYNTHOUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		MAINOUTLED_LIGHT,
		GAINLED_LIGHT,
		MODULATORLED_LIGHT,
		LIGHTS_LEN
	};

	INTENSIFIES() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(CARRIER_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FXBYPASS_PARAM, 0.f, 1.f, 0.f, "");
		configParam(CARRIERRANGE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FXVOLUME_PARAM, 0.f, 1.f, 0.f, "");
		configParam(GAIN_PARAM, 0.f, 1.f, 0.f, "");
		configParam(MODULATORENGAGE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(GAINRANGE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(MODULATOR_PARAM, 0.f, 1.f, 0.f, "");
		configParam(MODULATORRANGE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SYNTHVOLUME_PARAM, 0.f, 1.f, 0.f, "");
		configInput(CARRIERCV_INPUT, "");
		configInput(FXVOLUMECV_INPUT, "");
		configInput(GAINCV_INPUT, "");
		configInput(MODULATORCV_INPUT, "");
		configInput(AUDIOIN_INPUT, "");
		configInput(SYNTHVOLUMECV_INPUT, "");
		configOutput(FXOUT_OUTPUT, "");
		configOutput(SYNTHOUT_OUTPUT, "");
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

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(18.44, 34.222)), module, INTENSIFIES::CARRIER_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(75.02, 33.684)), module, INTENSIFIES::FXBYPASS_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(48.122, 35.871)), module, INTENSIFIES::CARRIERRANGE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(107.163, 50.988)), module, INTENSIFIES::FXVOLUME_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(67.346, 59.976)), module, INTENSIFIES::GAIN_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(38.99, 62.038)), module, INTENSIFIES::MODULATORENGAGE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(87.182, 77.145)), module, INTENSIFIES::GAINRANGE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.278, 93.051)), module, INTENSIFIES::MODULATOR_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(65.92, 94.064)), module, INTENSIFIES::MODULATORRANGE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(98.918, 97.521)), module, INTENSIFIES::SYNTHVOLUME_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(26.707, 11.585)), module, INTENSIFIES::CARRIERCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(97.418, 26.501)), module, INTENSIFIES::FXVOLUMECV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(63.064, 35.057)), module, INTENSIFIES::GAINCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(53.375, 76.187)), module, INTENSIFIES::MODULATORCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(110.044, 76.084)), module, INTENSIFIES::AUDIOIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(80.75, 101.603)), module, INTENSIFIES::SYNTHVOLUMECV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.254, 62.619)), module, INTENSIFIES::FXOUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.251, 104.885)), module, INTENSIFIES::SYNTHOUT_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(109.201, 35.91)), module, INTENSIFIES::MAINOUTLED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(87.126, 48.785)), module, INTENSIFIES::GAINLED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(21.257, 70.01)), module, INTENSIFIES::MODULATORLED_LIGHT));
	}
};


Model* modelINTENSIFIES = createModel<INTENSIFIES, INTENSIFIESWidget>("INTENSIFIES");