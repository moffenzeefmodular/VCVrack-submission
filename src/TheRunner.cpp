#include "plugin.hpp"


struct TheRunner : Module {
	enum ParamId {
		GAIN_PARAM,
		VOLUME_PARAM,
		ANIMATE_PARAM,
		RANGE_PARAM,
		CHORUS_PARAM,
		HARMONICS_PARAM,
		CUTOFF_PARAM,
		RESONANCE_PARAM,
		PITCH_PARAM,
		NOTESHZ_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		VOLUMECVIN_INPUT,
		ANIMATECVIN_INPUT,
		CHORUSCVIN_INPUT,
		HARMONICSCVIN_INPUT,
		CUTOFFCVIN_INPUT,
		RESONANCECVIN_INPUT,
		PITCHCVIN_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		AUDIOOUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		ANIMATELED_LIGHT,
		LIGHTS_LEN
	};

	TheRunner() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(GAIN_PARAM, 0.f, 1.f, 0.f, "Gain", "x", 0.f, 20.0f);
		configParam(VOLUME_PARAM, 0.f, 1.f, 0.f, "Volume", "%", 0.f, 100.f);
		configParam(ANIMATE_PARAM, 0.f, 1.f, 0.f, "Animate", "%", 0.f, 100.f);
		configSwitch(RANGE_PARAM, 0.f, 1.f, 0.f, "Range", {"Low", "High"});
		configSwitch(CHORUS_PARAM, 0.f, 1.f, 0.f, "Chorus", {"Off", "On"});
		configParam(HARMONICS_PARAM, 0.f, 1.f, 0.f, "Harmonics", "%", 0.f, 100.f);
		configParam(CUTOFF_PARAM, 0.f, 1.f, 0.f, "Cutoff", "hz", 62.5f, 80.f); // should be 80-5000
		configParam(RESONANCE_PARAM, 0.f, 1.f, 0.f, "Resonance", "%", 0.f, 100.f);
		configParam(PITCH_PARAM, 0.f, 1.f, 0.f, "Pitch", "hz", 32.f, 13.75f); // should be 13.7-440
		configSwitch(NOTESHZ_PARAM, 0.f, 1.f, 0.f, "Quantize", {"Off", "On"});
		configInput(VOLUMECVIN_INPUT, "Volume CV");
		configInput(ANIMATECVIN_INPUT, "Animate CV");
		configInput(CHORUSCVIN_INPUT, "Chorus CV");
		configInput(HARMONICSCVIN_INPUT, "Harmonics CV");
		configInput(CUTOFFCVIN_INPUT, "Cutoff CV");
		configInput(RESONANCECVIN_INPUT, "Resonance CV");
		configInput(PITCHCVIN_INPUT, "1v/Oct");
		configOutput(AUDIOOUT_OUTPUT, "Audio");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct TheRunnerWidget : ModuleWidget {
	TheRunnerWidget(TheRunner* module) {
		setModule(module);
setPanel(createPanel(
		asset::plugin(pluginInstance, "res/TheRunner.svg"),
		asset::plugin(pluginInstance, "res/TheRunner-dark.svg")
		));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<_2Pos>(mm2px(Vec(40.936, 49.41)), module, TheRunner::RANGE_PARAM));
		addParam(createParamCentered<_2Pos>(mm2px(Vec(66.526, 49.41)), module, TheRunner::CHORUS_PARAM));
		addParam(createParamCentered<_2Pos>(mm2px(Vec(53.754, 108.921)), module, TheRunner::NOTESHZ_PARAM));

		addParam(createParamCentered<Davies1900hLargeBlackKnob>(mm2px(Vec(28.141, 108.921)), module, TheRunner::PITCH_PARAM));

		addParam(createParamCentered<Davies1900hBlackKnob>(mm2px(Vec(15.173, 19.704)), module, TheRunner::GAIN_PARAM));
		addParam(createParamCentered<Davies1900hBlackKnob>(mm2px(Vec(40.79, 19.704)), module, TheRunner::VOLUME_PARAM));
		addParam(createParamCentered<Davies1900hBlackKnob>(mm2px(Vec(15.346, 49.41)), module, TheRunner::ANIMATE_PARAM));

		addParam(createParamCentered<Davies1900hBlackKnob>(mm2px(Vec(15.346, 79.116)), module, TheRunner::HARMONICS_PARAM));
		addParam(createParamCentered<Davies1900hBlackKnob>(mm2px(Vec(40.936, 79.116)), module, TheRunner::CUTOFF_PARAM));
		addParam(createParamCentered<Davies1900hBlackKnob>(mm2px(Vec(66.526, 79.116)), module, TheRunner::RESONANCE_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.657, 19.636)), module, TheRunner::VOLUMECVIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.657, 35.311)), module, TheRunner::ANIMATECVIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.657, 50.993)), module, TheRunner::CHORUSCVIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.657, 66.678)), module, TheRunner::HARMONICSCVIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.657, 82.377)), module, TheRunner::CUTOFFCVIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.657, 98.06)), module, TheRunner::RESONANCECVIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.657, 113.745)), module, TheRunner::PITCHCVIN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(66.45, 19.704)), module, TheRunner::AUDIOOUT_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(28.085, 33.711)), module, TheRunner::ANIMATELED_LIGHT));
	}
};


Model* modelTheRunner = createModel<TheRunner, TheRunnerWidget>("TheRunner");