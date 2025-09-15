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

	float phase = 0; 

void process(const ProcessArgs& args) override {
	const float minFreq = 13.75f;
	const float maxFreq = 440.f;

	float pitchKnob = params[PITCH_PARAM].getValue();
	float basePitch = rescale(pitchKnob, 0.f, 1.f, log2(minFreq), log2(maxFreq));
	float pitch = basePitch;

	if (inputs[PITCHCVIN_INPUT].isConnected()) {
		pitch += clamp(inputs[PITCHCVIN_INPUT].getVoltage(), -5.f, 5.f);
	}

	pitch = clamp(pitch, log2(minFreq), log2(maxFreq));

	if (params[NOTESHZ_PARAM].getValue() > 0.5f) {
		pitch = std::round(pitch * 12.f) / 12.f;
	}

	// Frequencies: Root, Sub, Octave, +5th, +2 Octaves
	float freqs[5] = {
		std::pow(2.f, pitch),                   // Root
		std::pow(2.f, pitch - 1.f),             // Suboctave
		std::pow(2.f, pitch + 1.f),             // +1 Octave
		std::pow(2.f, pitch + 19.f / 12.f),     // +1 Octave + 5th
		std::pow(2.f, pitch + 2.f)              // +2 Octaves
	};

	static float phases[5] = {0.f};

	float dt = args.sampleTime;
	float voices[5];
	for (int i = 0; i < 5; ++i) {
		phases[i] += freqs[i] * dt;
		if (phases[i] >= 1.f)
			phases[i] -= 1.f;
		voices[i] = (phases[i] < 0.5f) ? 5.f : -5.f;
	}

	// Harmonics knob + CV logic
	float harmParam = params[HARMONICS_PARAM].getValue(); // 0.0–1.0
	float harmCV = inputs[HARMONICSCVIN_INPUT].isConnected() ? inputs[HARMONICSCVIN_INPUT].getVoltage() / 10.f : 0.f;
	float harm = clamp(harmParam + harmCV, 0.f, 1.f);

	// Root always on at 0.2 gain
	float mix = voices[0] * 0.2f;

	// Sequential fade-in of the other 4 oscillators
	for (int i = 1; i < 5; ++i) {
		float start = 0.2f * i;
		float gain = clamp((harm - start) / 0.2f, 0.f, 1.f) * 0.2f;
		mix += voices[i] * gain;
	}

	mix = clamp(mix, -5.f, 5.f);
	outputs[AUDIOOUT_OUTPUT].setVoltage(mix);
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