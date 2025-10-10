#include "plugin.hpp"

#include <fstream>
#include <vector>
#include <string>

// --- Minimal WAV file reader (mono, 16-bit or 32-bit float) ---
static bool loadWavetableFile(const std::string& path, std::vector<float>& outSamples) {
	std::ifstream file(path, std::ios::binary);
	if (!file)
		return false;

	char riff[4];
	file.read(riff, 4);
	if (std::strncmp(riff, "RIFF", 4) != 0)
		return false;

	file.seekg(22);
	uint16_t channels;
	file.read(reinterpret_cast<char*>(&channels), 2);
	if (channels != 1)
		return false; // must be mono

	uint32_t sampleRate;
	file.read(reinterpret_cast<char*>(&sampleRate), 4);

	file.seekg(34);
	uint16_t bitsPerSample;
	file.read(reinterpret_cast<char*>(&bitsPerSample), 2);

	file.seekg(36);
	char dataHeader[4];
	while (true) {
		file.read(dataHeader, 4);
		uint32_t chunkSize;
		file.read(reinterpret_cast<char*>(&chunkSize), 4);
		if (std::strncmp(dataHeader, "data", 4) == 0) {
			break;
		}
		file.seekg(chunkSize, std::ios::cur);
	}

	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), {});
	size_t numSamples = buffer.size() / (bitsPerSample / 8);
	outSamples.resize(numSamples);

	if (bitsPerSample == 16) {
		const int16_t* src = reinterpret_cast<const int16_t*>(buffer.data());
		for (size_t i = 0; i < numSamples; i++)
			outSamples[i] = src[i] / 32768.f;
	} else if (bitsPerSample == 32) {
		const float* src = reinterpret_cast<const float*>(buffer.data());
		for (size_t i = 0; i < numSamples; i++)
			outSamples[i] = src[i];
	} else {
		return false;
	}

	return true;
}

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


		std::string path = asset::plugin(pluginInstance, "res/wavetables/StargazerWavetables.wav");

		std::vector<float> samples;
		if (loadWavetableFile(path, samples)) {
			numTables = samples.size() / tableSize;
			wavetables.resize(numTables);
			for (int i = 0; i < numTables; i++) {
				wavetables[i].assign(samples.begin() + i * tableSize,
		        samples.begin() + (i + 1) * tableSize);
			}
		}
	}

	std::vector<std::vector<float>> wavetables; 
	int tableSize = 600;       // samples per frame
	int numTables = 88;        // total tables
	float phase = 0.f;         // oscillator phase
	float sampleRate = 44100.f; // default fallback

void process(const ProcessArgs& args) override {
	if (wavetables.empty()) {
		outputs[OUT_OUTPUT].setVoltage(0.f);
		return;
	}

	// --- Sample rate correction ---
	sampleRate = 1.f / args.sampleTime;

	// --- Frequency control (1–500 Hz base + 1V/oct CV) ---
	float baseFreqParam = params[PITCH_PARAM].getValue(); // 0–1
	float baseFreq = 1.f + baseFreqParam * (500.f - 1.f); // Map knob 0–1 → 1–500 Hz

	// Read pitch CV (1V/oct), default 0 V
	float pitchCV = 0.f;
	if (inputs[PITCHCV_INPUT].isConnected())
		pitchCV = inputs[PITCHCV_INPUT].getVoltage(); // volts

	// Apply exponential 1V/oct scaling
	float freq = baseFreq * std::pow(2.f, pitchCV);

	// Clamp to safe oscillator range
	freq = clamp(freq, 1.f, 500.f);

	// Main Wave Knob + CV 
	//float waveParam = params[MAINWAVE_PARAM].getValue(); // 1–88

	float waveParam = 1.0f + clamp((params[MAINWAVE_PARAM].getValue() - 1.0f) / 87.0f + inputs[WAVECV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f) * 87.0f;
	
	// --- MAINWAVE_PARAM: 1–88 selection (morph) ---
	float wavePos = clamp(waveParam - 1.f, 0.f, numTables - 1.f);
	int i0 = (int)wavePos;
	int i1 = std::min(i0 + 1, numTables - 1);
	float frac = wavePos - i0;

	// --- Phase increment (pitch stable across sample rates) ---
	float phaseInc = freq / sampleRate;
	phase += phaseInc;
	if (phase >= 1.f)
		phase -= 1.f;

	// --- Lookup position within table ---
	float pos = phase * (tableSize - 1);
	int idx0 = (int)pos;
	int idx1 = (idx0 + 1) % tableSize;
	float fracPos = pos - idx0;

	// --- Interpolate within tables ---
	float s00 = wavetables[i0][idx0];
	float s01 = wavetables[i0][idx1];
	float s10 = wavetables[i1][idx0];
	float s11 = wavetables[i1][idx1];

	// Interpolate between samples (horizontal)
	float a = s00 + (s01 - s00) * fracPos;
	float b = s10 + (s11 - s10) * fracPos;

	// Interpolate between tables (vertical morph)
	float sample = a + (b - a) * frac;

	// --- Scale output to ±5 V ---
	sample *= 5.f;

	// --- Output final waveform ---
	outputs[OUT_OUTPUT].setVoltage(sample);
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