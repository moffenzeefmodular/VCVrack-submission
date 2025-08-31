#include "plugin.hpp"


struct INTENSIFIES : Module {
	enum ParamId {
		CARRIERRANGE_PARAM,
		FXBYPASS_PARAM,
		CARRIER_PARAM,
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

	float clockPhase = 0.f;
	bool clockState = false;
	float heldSample = 0.f;
	float baseFreq = 0.f;
	float modulatorPhase = 0.f;
float modulatorSignal = 1.f;

	INTENSIFIES() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(CARRIER_PARAM, 0.f, 1.f, 1.f, "Carrier Frequency", "%", 0.f, 100.f);
		configParam(MODULATOR_PARAM, 0.f, 1.f, 1.f, "Modulator Frequency", "%", 0.f, 100.f);

		configParam(SYNTHVOLUME_PARAM, 0.f, 1.f, 1.f, "Synth Volume", "%", 0.f, 100.f);
		configParam(FXVOLUME_PARAM, 0.f, 1.f, 1.f, "FX Volume", "%", 0.f, 100.f);
		configParam(GAIN_PARAM, 0.f, 1.f, 0.5f, "Gain", "%", 0.f, 100.f);

		configSwitch(CARRIERRANGE_PARAM, 0.f, 2.f, 2.f, "Carrier Frequency Range", {"Low", "Medium", "High"});
		configSwitch(MODULATORRANGE_PARAM, 0.f, 2.f, 0.f, "Modulator Frequency Range", {"Low", "Medium", "High"});

		configSwitch(FXBYPASS_PARAM, 0.f, 1.f, 0.f, "Bypass", {"Off", "On"});
		configSwitch(MODULATORENGAGE_PARAM, 0.f, 1.f, 0.f, "Modulator", {"Off", "On"});
		configSwitch(GAINRANGE_PARAM, 0.f, 1.f, 0.f, "Gain Range", {"20x", "200x"});

		configInput(CARRIERCV_INPUT, "Carrier CV");
		configInput(FXVOLUMECV_INPUT, "FX Volume CV");
		configInput(GAINCV_INPUT, "Gain CV");
		configInput(MODULATORCV_INPUT, "Modulator CV");
		configInput(AUDIOIN_INPUT, "Audio");
		configInput(SYNTHVOLUMECV_INPUT, "Synth Volume CV");

		configOutput(FXOUT_OUTPUT, "FX");
		configOutput(SYNTHOUT_OUTPUT, "Synth");
	}

	void process(const ProcessArgs& args) override {
	// Carrier control
	float carrierKnob = params[CARRIER_PARAM].getValue();
	float carrierCV = inputs[CARRIERCV_INPUT].isConnected() ? inputs[CARRIERCV_INPUT].getVoltage() / 10.f : 0.f;
	float carrierControl = clamp(carrierKnob + carrierCV, 0.f, 1.f);
	int carrierRange = static_cast<int>(params[CARRIERRANGE_PARAM].getValue());

	float carrierBaseFreq = 0.f;
	switch (carrierRange) {
		case 0: carrierBaseFreq = 1.f; break;
		case 1: carrierBaseFreq = 10.f; break;
		default: carrierBaseFreq = 100.f; break;
	}
	float carrierFreq = carrierBaseFreq * std::pow(100.f, carrierControl);
	carrierFreq = clamp(carrierFreq, 0.f, 100000.f);

	// Modulator control
	bool modEnabled = params[MODULATORENGAGE_PARAM].getValue() > 0.5f;
	float modKnob = params[MODULATOR_PARAM].getValue();
	float modCV = inputs[MODULATORCV_INPUT].isConnected() ? inputs[MODULATORCV_INPUT].getVoltage() / 10.f : 0.f;
	float modControl = clamp(modKnob + modCV, 0.f, 1.f);
	int modRange = static_cast<int>(params[MODULATORRANGE_PARAM].getValue());

	float modBaseFreq = 0.f;
	switch (modRange) {
		case 0: modBaseFreq = 50.f; break;
		case 1: modBaseFreq = 100.f; break;
		default: modBaseFreq = 500.f; break;
	}
	float modFreq = modBaseFreq * modControl;  // Linear scaling with modControl, or adjust to taste
	modFreq = clamp(modFreq, 0.f, 500.f);

	// Advance modulator
	modulatorPhase += modFreq * args.sampleTime;
	if (modulatorPhase >= 1.0f) modulatorPhase -= 1.0f;

	bool modulatorHigh = (modulatorPhase < 0.5f);

	bool sampleNow = (!modEnabled || modulatorHigh);

	if (sampleNow) {
		clockPhase += carrierFreq * args.sampleTime;
		if (clockPhase >= 1.0f) {
			clockPhase -= 1.0f;
			heldSample = inputs[AUDIOIN_INPUT].getVoltage();
		}
	}

	float outputSample = sampleNow ? heldSample : 0.f;
	outputs[FXOUT_OUTPUT].setVoltage(clamp(outputSample, -5.f, 5.f));
}
};


struct INTENSIFIESWidget : ModuleWidget {
	INTENSIFIESWidget(INTENSIFIES* module) {
		setModule(module);
	  setPanel(createPanel(
		asset::plugin(pluginInstance, "res/INTENSIFIES.svg"),
		asset::plugin(pluginInstance, "res/INTENSIFIES-dark.svg")
		));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<_3Pos>(mm2px(Vec(48.122, 35.871)), module, INTENSIFIES::CARRIERRANGE_PARAM));
		addParam(createParamCentered<_3Pos>(mm2px(Vec(65.957, 93.144)), module, INTENSIFIES::MODULATORRANGE_PARAM));

		addParam(createParamCentered<_2Pos>(mm2px(Vec(87.182, 77.145)), module, INTENSIFIES::GAINRANGE_PARAM));
		addParam(createParamCentered<_2Pos>(mm2px(Vec(82.287, 35.832)), module, INTENSIFIES::FXBYPASS_PARAM));
		addParam(createParamCentered<_2Pos>(mm2px(Vec(38.99, 62.038)), module, INTENSIFIES::MODULATORENGAGE_PARAM));

		addParam(createParamCentered<Davies1900hLargeBlackKnob>(mm2px(Vec(22.855, 39.741)), module, INTENSIFIES::CARRIER_PARAM));
		addParam(createParamCentered<Davies1900hLargeBlackKnob>(mm2px(Vec(35.823, 87.83)), module, INTENSIFIES::MODULATOR_PARAM));

		addParam(createParamCentered<Davies1900hBlackKnob>(mm2px(Vec(107.163, 50.988)), module, INTENSIFIES::FXVOLUME_PARAM));
		addParam(createParamCentered<Davies1900hBlackKnob>(mm2px(Vec(98.918, 97.521)), module, INTENSIFIES::SYNTHVOLUME_PARAM));
		addParam(createParamCentered<Davies1900hBlackKnob>(mm2px(Vec(67.346, 59.976)), module, INTENSIFIES::GAIN_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(26.707, 11.585)), module, INTENSIFIES::CARRIERCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(97.418, 26.501)), module, INTENSIFIES::FXVOLUMECV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(63.064, 35.057)), module, INTENSIFIES::GAINCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(53.375, 76.187)), module, INTENSIFIES::MODULATORCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(105, 76.084)), module, INTENSIFIES::AUDIOIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(80.75, 101.603)), module, INTENSIFIES::SYNTHVOLUMECV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.254, 62.619)), module, INTENSIFIES::FXOUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.251, 104.885)), module, INTENSIFIES::SYNTHOUT_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(109.201, 35.91)), module, INTENSIFIES::MAINOUTLED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(79.581, 52.375)), module, INTENSIFIES::GAINLED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(21.257, 70.01)), module, INTENSIFIES::MODULATORLED_LIGHT));
	}
};


Model* modelINTENSIFIES = createModel<INTENSIFIES, INTENSIFIESWidget>("INTENSIFIES");