#include "plugin.hpp"


struct Stargazer : Module {
	enum ParamId {
		PITCH_PARAM,
		GAIN_PARAM,
		SUB_PARAM,
		DETUNE_PARAM,
		VOL_PARAM,
		MAINWAVE_PARAM,
		REDUX_PARAM,
		MIX_PARAM,
		FREQ1_PARAM,
		FREQ2_PARAM,
		ALIAS_PARAM,
		RES2_PARAM,
		RES1_PARAM,
		DEPTH2_PARAM,
		WAVE1_PARAM,
		DEPTH3_PARAM,
		RATE2_PARAM,
		WAVE2_PARAM,
		RATE1_PARAM,
		RATE3_PARAM,
		WAVE3_PARAM,
		DEPTH1_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		PITCHCV_INPUT,
		SUBCV_INPUT,
		WAVECV_INPUT,
		MIXCV_INPUT,
		DETUNECV_INPUT,
		FREQ1CV_INPUT,
		RES1CV_INPUT,
		ALIASCV_INPUT,
		REDUXCV_INPUT,
		FREQ2CV_INPUT,
		RES2CV_INPUT,
		LFO1WAVECV_INPUT,
		LFO1RATECV_INPUT,
		LFO1DEPTHCV_INPUT,
		LFO2WAVECV_INPUT,
		LFO2RATECV_INPUT,
		LFO2DEPTHCV_INPUT,
		LFO3WAVECV_INPUT,
		LFO3RATECV_INPUT,
		LFO3DEPTHCV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_OUTPUT,
		LFO1OUT_OUTPUT,
		LFO2OUT_OUTPUT,
		LFO3OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LFO2LED_LIGHT,
		LFO1LED_LIGHT,
		LFO3LED_LIGHT,
		LIGHTS_LEN
	};

	Stargazer() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PITCH_PARAM, 0.f, 1.f, 0.5f, "Pitch", "hz", 500.f, 1.f); // 1hz - 500hz
		configSwitch(SUB_PARAM, 0.f, 1.f, 0.f, "Sub Oscillator", {"Off", "On"}); // Turn osc2 into sub oscillator
		configParam(MAINWAVE_PARAM, 1.f, 88.f, 0.f, "Wavetable Select"); // 1 - 88 morphing wavetable select
		configParam(MIX_PARAM, 0.f, 1.f, 0.f, "Oscillator 2 Volume", "%", 0.f, 100.f); 
		configParam(DETUNE_PARAM, -1.f, 1.f, 0.f, "Oscillator 2 Detune", "hz", 0.f, 5.f); // +/-5hz 

		configParam(FREQ1_PARAM, 0.f, 1.f, 1.f, "Filter 1 Cutoff", "hz", 62.5f, 80.f); // 80hz - 5khz
		configParam(RES1_PARAM, 0.f, 1.f, 0.f, "Filter 1 Resonance", "%", 0.f, 100.f); // Q 1-5

		configParam(ALIAS_PARAM, 1.f, 0.f, 0.f, "Sample Rate", "hz", 18000.f, 1.f); // 18khz - 1hz 
		configSwitch(REDUX_PARAM, 0.f, 12.f, 0.f, "Bit Depth", {"12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"});

		configParam(FREQ2_PARAM, 0.f, 1.f, 1.f, "Filter 2 Cutoff", "hz", 62.5f, 80.f); // 80hz - 5khz
		configParam(RES2_PARAM, 0.f, 1.f, 0.f, "Filter 2 Resonance", "%", 0.f, 100.f); // Q 1-5

		configSwitch(WAVE1_PARAM, 0.f, 5.f, 0.f, "LFO 1 Waveshape", {"Sine", "Triangle", "Ramp Up", "Ramp Down", "Square", "Random"} );
		configParam(RATE1_PARAM, 0.f, 1.f, 0.f, "LFO 1 Frequency", "hz", 1000.f, 0.05f); // 0.05hz - 50hz
		configParam(DEPTH1_PARAM, 0.f, 1.f, 0.f, "LFO 1 Depth", "%", 0.f, 100.f);

		configSwitch(WAVE2_PARAM, 0.f, 5.f, 0.f, "LFO 2 Waveshape", {"Sine", "Triangle", "Ramp Up", "Ramp Down", "Square", "Random"});
		configParam(RATE2_PARAM, 0.f, 1.f, 0.f, "LFO 2 Frequency", "hz", 1000.f, 0.05f); // 0.05hz - 50hz
		configParam(DEPTH2_PARAM, 0.f, 1.f, 0.f, "LFO 2 Depth", "%", 0.f, 100.f);

		configSwitch(WAVE3_PARAM, 0.f, 5.f, 0.f, "LFO 3 Waveshape", {"Sine", "Triangle", "Ramp Up", "Ramp Down", "Square", "Random"});
		configParam(RATE3_PARAM, 0.f, 1.f, 0.f, "LFO 3 Frequency", "hz", 1000.f, 0.05f); // 0.05hz - 50hz
		configParam(DEPTH3_PARAM, 0.f, 1.f, 0.f, "LFO 3 Depth", "%", 0.f, 100.f);

		configParam(GAIN_PARAM, 0.f, 1.f, 0.f, "Gain", "x", 100.f, 1.f); // Unity - 100x gain 
		configParam(VOL_PARAM, 0.f, 1.f, 1.f, "Volume", "%", 0.f, 100.f);

		configInput(PITCHCV_INPUT, "1v/Oct");
		configInput(SUBCV_INPUT, "Sub CV");
		configInput(WAVECV_INPUT, "Wave CV");
		configInput(MIXCV_INPUT, "Mix CV");
		configInput(DETUNECV_INPUT, "Detune CV");
		configInput(FREQ1CV_INPUT, "Filter 1 Cutoff CV");
		configInput(RES1CV_INPUT, "Filter 1 Resonance CV");
		configInput(ALIASCV_INPUT, "Alias CV");
		configInput(REDUXCV_INPUT, "Redux CV");
		configInput(FREQ2CV_INPUT, "Filter 2 Cutoff CV");
		configInput(RES2CV_INPUT, "Filter 2 Resonance CV");
		configInput(LFO1WAVECV_INPUT, "LFO 1 Waveshape CV");
		configInput(LFO1RATECV_INPUT, "LFO 1 Rate CV");
		configInput(LFO1DEPTHCV_INPUT, "LFO 1 Depth CV");
		configInput(LFO2WAVECV_INPUT, "LFO 2 Waveshape CV");
		configInput(LFO2RATECV_INPUT, "LFO 2 Rate CV");
		configInput(LFO2DEPTHCV_INPUT, "LFO 2 Depth CV");
		configInput(LFO3WAVECV_INPUT, "LFO 3 Waveshape CV");
		configInput(LFO3RATECV_INPUT, "LFO 3 Rate CV");
		configInput(LFO3DEPTHCV_INPUT, "LFO 3 Depth CV");
		configOutput(OUT_OUTPUT, "Audio");
		configOutput(LFO1OUT_OUTPUT, "LFO 1");
		configOutput(LFO2OUT_OUTPUT, "LFO 2");
		configOutput(LFO3OUT_OUTPUT, "LFO 3");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct StargazerWidget : ModuleWidget {
	StargazerWidget(Stargazer* module) {
		setModule(module);
	setPanel(createPanel(
		asset::plugin(pluginInstance, "res/panels/Stargazer.svg"),
		asset::plugin(pluginInstance, "res/panels/Stargazer-dark.svg")
		));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<StargazerDavies>(mm2px(Vec(26.918, 12.382)), module, Stargazer::PITCH_PARAM));
		addParam(createParamCentered<StargazerDavies>(mm2px(Vec(96.236, 14.975)), module, Stargazer::GAIN_PARAM));
		addParam(createParamCentered<StargazerDavies>(mm2px(Vec(10.681, 32.195)), module, Stargazer::MAINWAVE_PARAM));
		addParam(createParamCentered<StargazerDavies>(mm2px(Vec(122.509, 27.037)), module, Stargazer::VOL_PARAM));

		addParam(createParamCentered<_2Pos>(mm2px(Vec(40.889, 17.855)), module, Stargazer::SUB_PARAM));

		addParam(createParamCentered<MediumKnob>(mm2px(Vec(55.727, 21.557)), module, Stargazer::DETUNE_PARAM));
		addParam(createParamCentered<MediumKnob>(mm2px(Vec(35.809, 36.884)), module, Stargazer::MIX_PARAM));
		addParam(createParamCentered<MediumKnob>(mm2px(Vec(78.258, 33.499)), module, Stargazer::REDUX_PARAM));

		addParam(createParamCentered<Davies1900hLargeBlackKnob>(mm2px(Vec(16.256, 53.107)), module, Stargazer::FREQ1_PARAM));
		addParam(createParamCentered<Davies1900hLargeBlackKnob>(mm2px(Vec(96.873, 53.957)), module, Stargazer::FREQ2_PARAM));

		addParam(createParamCentered<BefacoBigKnobBlack>(mm2px(Vec(60.422, 55.071)), module, Stargazer::ALIAS_PARAM));

		addParam(createParamCentered<StargazerTinyKnob>(mm2px(Vec(124.763, 61.375)), module, Stargazer::RES2_PARAM));
		addParam(createParamCentered<StargazerTinyKnob>(mm2px(Vec(40.412, 65.557)), module, Stargazer::RES1_PARAM));

		addParam(createParamCentered<StargazerLFOKnob>(mm2px(Vec(93.262, 76.91)), module, Stargazer::DEPTH2_PARAM));
		addParam(createParamCentered<StargazerLFOKnob>(mm2px(Vec(15.131, 83.658)), module, Stargazer::WAVE1_PARAM));
		addParam(createParamCentered<StargazerLFOKnob>(mm2px(Vec(122.023, 89.045)), module, Stargazer::DEPTH3_PARAM));
		addParam(createParamCentered<StargazerLFOKnob>(mm2px(Vec(78.326, 89.859)), module, Stargazer::RATE2_PARAM));
		addParam(createParamCentered<StargazerLFOKnob>(mm2px(Vec(54.962, 91.29)), module, Stargazer::WAVE2_PARAM));
		addParam(createParamCentered<StargazerLFOKnob>(mm2px(Vec(23.225, 102.295)), module, Stargazer::RATE1_PARAM));
		addParam(createParamCentered<StargazerLFOKnob>(mm2px(Vec(108.733, 102.098)), module, Stargazer::RATE3_PARAM));
		addParam(createParamCentered<StargazerLFOKnob>(mm2px(Vec(88.822, 109.405)), module, Stargazer::WAVE3_PARAM));
		addParam(createParamCentered<StargazerLFOKnob>(mm2px(Vec(40.523, 112.668)), module, Stargazer::DEPTH1_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(138.373, 12.362)), module, Stargazer::PITCHCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(156.591, 12.362)), module, Stargazer::SUBCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(138.373, 23.389)), module, Stargazer::WAVECV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(147.477, 23.389)), module, Stargazer::MIXCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(156.591, 23.389)), module, Stargazer::DETUNECV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(138.373, 35.081)), module, Stargazer::FREQ1CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(147.477, 35.081)), module, Stargazer::RES1CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(156.591, 35.081)), module, Stargazer::ALIASCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(138.373, 47.866)), module, Stargazer::REDUXCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(147.477, 47.866)), module, Stargazer::FREQ2CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(156.591, 47.866)), module, Stargazer::RES2CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(138.373, 59.694)), module, Stargazer::LFO1WAVECV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(147.477, 59.694)), module, Stargazer::LFO1RATECV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(156.591, 59.694)), module, Stargazer::LFO1DEPTHCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(138.373, 85.37)), module, Stargazer::LFO2WAVECV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(147.477, 85.37)), module, Stargazer::LFO2RATECV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(156.591, 85.37)), module, Stargazer::LFO2DEPTHCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(138.373, 109.911)), module, Stargazer::LFO3WAVECV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(147.477, 109.911)), module, Stargazer::LFO3RATECV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(156.591, 109.911)), module, Stargazer::LFO3DEPTHCV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(121.955, 9.29)), module, Stargazer::OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(147.477, 72.541)), module, Stargazer::LFO1OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(147.477, 98.217)), module, Stargazer::LFO2OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(147.477, 121.17)), module, Stargazer::LFO3OUT_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(69.881, 80.617)), module, Stargazer::LFO2LED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(8.873, 99.304)), module, Stargazer::LFO1LED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(121.456, 105.787)), module, Stargazer::LFO3LED_LIGHT));
	}
};


Model* modelStargazer = createModel<Stargazer, StargazerWidget>("Stargazer");