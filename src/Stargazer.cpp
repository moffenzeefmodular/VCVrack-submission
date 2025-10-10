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
		LFO1LEDRED_LIGHT,
		LFO1LEDGREEN_LIGHT,
		LFO2LEDRED_LIGHT,
		LFO2LEDGREEN_LIGHT,
		LFO3LEDRED_LIGHT,
		LFO3LEDGREEN_LIGHT,
		LIGHTS_LEN
	};

	Stargazer() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PITCH_PARAM, 0.f, 1.f, 0.1f, "Pitch", "hz", 500.f, 1.f); // 1hz - 500hz
		configSwitch(SUB_PARAM, 0.f, 1.f, 0.f, "Sub Oscillator", {"Off", "On"}); // Turn osc2 into sub oscillator
		configParam(MAINWAVE_PARAM, 1.f, 88.f, 0.f, "Wavetable Select"); // 1 - 88 morphing wavetable select
		configParam(MIX_PARAM, 0.f, 1.f, 0.f, "Oscillator 2 Volume", "%", 0.f, 100.f); 
		configParam(DETUNE_PARAM, -1.f, 1.f, 0.f, "Oscillator 2 Detune", "hz", 0.f, 5.f); // +/-5hz 

		configParam(FREQ1_PARAM, 0.f, 1.f, 1.f, "Filter 1 Cutoff", "hz", 62.5f, 80.f); // 80hz - 5khz
		configParam(RES1_PARAM, 0.f, 1.f, 0.f, "Filter 1 Resonance", "%", 0.f, 100.f); // Q 1-5

		configParam(ALIAS_PARAM, 1.f, 0.f, 1.f, "Sample Rate", "hz", 878.04f, 20.5f); // 18khz - 1hz 
		configSwitch(REDUX_PARAM, 0.f, 8.f, 0.f, "Bit Depth", {"12", "11", "10", "9", "8", "7", "6", "5", "4"});

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

	// Main wavetable
	std::vector<std::vector<float>> wavetables; 
	int tableSize = 600;       // samples per frame
	int numTables = 88;        // total tables
	float sampleRate = 44100.f; // default fallback

	// --- Oscillator phases ---
float phase1 = 0.f;
float phase2 = 0.f;

// --- Filter 1 state (biquad) ---
float lp1_x1 = 0.f, lp1_x2 = 0.f, lp1_y1 = 0.f, lp1_y2 = 0.f;

// --- Filter 2 state (biquad) ---
float lp2_x1 = 0.f, lp2_x2 = 0.f, lp2_y1 = 0.f, lp2_y2 = 0.f;

// --- AGC states for filter 1 ---
float agcEnv1 = 0.f;
float agcGain1 = 1.f;

// --- AGC parameters (shared) ---
const float agcAttack = 0.001f;   // smoothing when signal rises
const float agcRelease = 0.01f;   // smoothing when signal falls
const float targetPeak = 0.8f;    // target normalized peak

// Alias 
float aliasCounter = 0.f;
float lastSample = 0.f;

// LFO1 member variables
float lfo1Phase = 0.f;
float lfo1LastValue = 0.f;      // last output (already exists)
float lfo1StepCounter = 1.f;    // for stepped random waveform
float lfo1RandValue = 0.f;      // for stepped random waveform
float lfo1Value = 0.f;

// LFO2 member variables
float lfo2Phase = 0.f;
float lfo2LastValue = 0.f;      // last output (already exists)
float lfo2StepCounter = 1.f;    // for stepped random waveform
float lfo2RandValue = 0.f;      // for stepped random waveform
float lfo2Value = 0.f;

// LFO3 member variables
float lfo3Phase = 0.f;
float lfo3LastValue = 0.f;      // last output (already exists)
float lfo3StepCounter = 1.f;    // for stepped random waveform
float lfo3RandValue = 0.f;      // for stepped random waveform
float lfo3Value = 0.f;


void process(const ProcessArgs& args) override {

// MODULATION SECTION 
auto processLFO = [&](int rateParam, int depthParam, int waveParam,
                      int rateCV, int depthCV, int waveCV,
                      float& phase, float& stepCounter, float& randValue,
                      int outId, int ledRedId, int ledGreenId,
                      float* outValue = nullptr) {
    float rate = params[rateParam].getValue();
    if (inputs[rateCV].isConnected()) rate += inputs[rateCV].getVoltage() / 10.f;
    rate = clamp(rate, 0.f, 1.f);
    float freq = 0.05f * powf(50.f / 0.05f, rate);

    float depth = params[depthParam].getValue();
    if (inputs[depthCV].isConnected()) depth += inputs[depthCV].getVoltage() / 10.f;
    depth = clamp(depth, 0.f, 1.f);

    phase += freq * args.sampleTime;
    if (phase >= 1.f) phase -= 1.f;

    int wave = clamp((int)roundf(params[waveParam].getValue() +
                 (inputs[waveCV].isConnected() ? clamp(inputs[waveCV].getVoltage(), -5.f, 5.f)/2.f : 0.f)), 0, 5);

    float value = 0.f;
    switch (wave) {
        case 0: value = sinf(2.f * float(M_PI) * phase); break;
        case 1: value = 1.f - 4.f * fabsf(phase - 0.5f); break;
        case 2: value = 2.f * phase - 1.f; break;
        case 3: value = 1.f - 2.f * phase; break;
        case 4: value = (phase < 0.5f) ? 1.f : -1.f; break;
        case 5:
            if (stepCounter >= 1.f) {
                randValue = 2.f*((float)rand()/RAND_MAX)-1.f;
                stepCounter -= 1.f;
            }
            stepCounter += freq * args.sampleTime * 2.f; // double frequency for stepped random
            value = randValue;
            break;
    }

    value *= depth * 5.f;
    outputs[outId].setVoltage(value);

    // LED logic: green = positive, red = negative
    lights[ledGreenId].setBrightnessSmooth(clamp(value / 5.f, 0.f, 1.f), args.sampleTime);
    lights[ledRedId].setBrightnessSmooth(clamp(-value / 5.f, 0.f, 1.f), args.sampleTime);

    if (outValue)
        *outValue = value;
};

// Call all three LFOs with both LED IDs
processLFO(RATE1_PARAM, DEPTH1_PARAM, WAVE1_PARAM,
           LFO1RATECV_INPUT, LFO1DEPTHCV_INPUT, LFO1WAVECV_INPUT,
           lfo1Phase, lfo1StepCounter, lfo1RandValue,
           LFO1OUT_OUTPUT, LFO1LEDRED_LIGHT, LFO1LEDGREEN_LIGHT, &lfo1Value);

processLFO(RATE2_PARAM, DEPTH2_PARAM, WAVE2_PARAM,
           LFO2RATECV_INPUT, LFO2DEPTHCV_INPUT, LFO2WAVECV_INPUT,
           lfo2Phase, lfo2StepCounter, lfo2RandValue,
           LFO2OUT_OUTPUT, LFO2LEDRED_LIGHT, LFO2LEDGREEN_LIGHT, &lfo2Value);

processLFO(RATE3_PARAM, DEPTH3_PARAM, WAVE3_PARAM,
           LFO3RATECV_INPUT, LFO3DEPTHCV_INPUT, LFO3WAVECV_INPUT,
           lfo3Phase, lfo3StepCounter, lfo3RandValue,
           LFO3OUT_OUTPUT, LFO3LEDRED_LIGHT, LFO3LEDGREEN_LIGHT, &lfo3Value);


	// START OF AUDIO SECTION 
    if (wavetables.empty()) {
        outputs[OUT_OUTPUT].setVoltage(0.f);
        return;
    }

    sampleRate = 1.f / args.sampleTime;

    // --- Frequency control (1–500 Hz base + 1V/oct CV) ---
    float baseFreqParam = params[PITCH_PARAM].getValue();
    float baseFreq = 1.f + baseFreqParam * (500.f - 1.f);
    float pitchCV = inputs[PITCHCV_INPUT].isConnected() ? inputs[PITCHCV_INPUT].getVoltage() : 0.f;
    float freq = clamp(baseFreq * std::pow(2.f, pitchCV), 1.f, 500.f);

    // --- MAINWAVE_PARAM: 1–88 selection (morph) ---
    float waveParam = 1.0f + clamp((params[MAINWAVE_PARAM].getValue() - 1.0f) / 87.0f +
                                   inputs[WAVECV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f) * 87.0f;
    float wavePos = clamp(waveParam - 1.f, 0.f, numTables - 1.f);
    int i0 = (int)wavePos;
    int i1 = std::min(i0 + 1, numTables - 1);
    float frac = wavePos - i0;

    // --- Oscillator 1 ---
    phase1 += freq / sampleRate;
    if (phase1 >= 1.f) phase1 -= 1.f;
    float pos1 = phase1 * (tableSize - 1);
    int idx0 = (int)pos1;
    int idx1 = (idx0 + 1) % tableSize;
    float fracPos = pos1 - idx0;
    float s00 = wavetables[i0][idx0];
    float s01 = wavetables[i0][idx1];
    float s10 = wavetables[i1][idx0];
    float s11 = wavetables[i1][idx1];
    float a = s00 + (s01 - s00) * fracPos;
    float b = s10 + (s11 - s10) * fracPos;
    float osc1 = a + (b - a) * frac;

    // --- Oscillator 2 (detuned + independent volume + sub) ---
    float detune = params[DETUNE_PARAM].getValue();
    if (inputs[DETUNECV_INPUT].isConnected())
        detune += inputs[DETUNECV_INPUT].getVoltage() / 5.f; // ±5V → ±1
    detune = clamp(detune, -1.f, 1.f);
    float freq2 = clamp(freq + detune * 5.f, 1.f, 500.f);

    bool subEnabled = params[SUB_PARAM].getValue() > 0.5f;
    if (inputs[SUBCV_INPUT].isConnected())
        subEnabled = inputs[SUBCV_INPUT].getVoltage() > 0.f;
    if (subEnabled)
        freq2 *= 0.5f;

    phase2 += freq2 / sampleRate;
    if (phase2 >= 1.f) phase2 -= 1.f;
    float pos2 = phase2 * (tableSize - 1);
    int idx20 = (int)pos2;
    int idx21 = (idx20 + 1) % tableSize;
    float fracPos2 = pos2 - idx20;
    float s00_2 = wavetables[i0][idx20];
    float s01_2 = wavetables[i0][idx21];
    float s10_2 = wavetables[i1][idx20];
    float s11_2 = wavetables[i1][idx21];
    float a2_osc = s00_2 + (s01_2 - s00_2) * fracPos2;
    float b2_osc = s10_2 + (s11_2 - s10_2) * fracPos2;
    float osc2 = a2_osc + (b2_osc - a2_osc) * frac;

    // --- Mix knob 0–1 plus CV ---
    float mix = params[MIX_PARAM].getValue();
    if (inputs[MIXCV_INPUT].isConnected())
        mix += inputs[MIXCV_INPUT].getVoltage() / 10.f;
    mix = clamp(mix, 0.f, 1.f);

    // --- Oscillator mixer ---
    float sample = osc1 + osc2 * mix;

    // --- Filter 1 cutoff + resonance ---
	float cutoff = params[FREQ1_PARAM].getValue();
	if (inputs[FREQ1CV_INPUT].isConnected()){
    	cutoff += inputs[FREQ1CV_INPUT].getVoltage() / 10.f;
	}
	cutoff += lfo1Value / 10.f;  // scale ±5V → ±0.5 and sum with cutoff (0–1)
	cutoff = clamp(cutoff, 0.f, 1.f);
	float cutoffHz = 80.f * std::pow(5000.f / 80.f, cutoff);

    float res = params[RES1_PARAM].getValue();
    if (inputs[RES1CV_INPUT].isConnected())
        res += inputs[RES1CV_INPUT].getVoltage() / 10.f;
    res = clamp(res, 0.f, 1.f);
    float Q = 1.f + res * 4.f;

    // --- Filter 1 biquad ---
    float w0 = 2.f * float(M_PI) * cutoffHz / sampleRate;
    float alpha = sinf(w0) / (2.f * Q);
    float cos_w0 = cosf(w0);
    float fb0 = (1.f - cos_w0) / 2.f;
    float fb1 = 1.f - cos_w0;
    float fb2 = (1.f - cos_w0) / 2.f;
    float fa0 = 1.f + alpha;
    float fa1 = -2.f * cos_w0;
    float fa2 = 1.f - alpha;

    float y = (fb0/fa0)*sample + (fb1/fa0)*lp1_x1 + (fb2/fa0)*lp1_x2 - (fa1/fa0)*lp1_y1 - (fa2/fa0)*lp1_y2;
    lp1_x2 = lp1_x1; lp1_x1 = sample;
    lp1_y2 = lp1_y1; lp1_y1 = y;

    // --- AGC 1 ---
    float absY = fabsf(y);
    agcEnv1 += (absY - agcEnv1) * (absY > agcEnv1 ? agcAttack : agcRelease);
    if (agcEnv1 > 0.0001f) agcGain1 = targetPeak / agcEnv1;
    float scaledOutput = clamp(y * agcGain1 * 0.5f, -10.f, 10.f);

    // --- Alias (sample rate reducer) ---
    float aliasKnob = 1.f - params[ALIAS_PARAM].getValue();
    if (inputs[ALIASCV_INPUT].isConnected())
        aliasKnob += inputs[ALIASCV_INPUT].getVoltage() / 10.f;
    aliasKnob = clamp(aliasKnob, 0.f, 1.f);

    float fadeFactor = (aliasKnob <= 0.05f) ? (aliasKnob / 0.05f) : 1.f;
    float aliasRate = 25.f + (18000.f - aliasKnob * (18000.f - 1.f));

    aliasCounter += aliasRate * args.sampleTime;
    if (aliasCounter >= 1.f) {
        lastSample = scaledOutput;
        aliasCounter -= 1.f;
    }
    float finalOutput = scaledOutput * (1.f - fadeFactor) + lastSample * fadeFactor;

    // --- Redux bit reduction ---
    int reduxBitDepth = 12 - (int)params[REDUX_PARAM].getValue();
    reduxBitDepth = clamp(reduxBitDepth, 4, 12);
    if (inputs[REDUXCV_INPUT].isConnected()) {
        float reduxCV = clamp(inputs[REDUXCV_INPUT].getVoltage(), -5.f, 5.f);
        reduxBitDepth = clamp((int)roundf(12.f - ((reduxCV + 5.f)/10.f)*(12.f-4.f)), 4, 12);
    }
    float maxVal = powf(2.f, reduxBitDepth - 1) - 1.f;
    float normalized = clamp(finalOutput / 10.f, -1.f, 1.f);
    float quantized = roundf(normalized * maxVal) / maxVal;
    finalOutput = quantized * 10.f;

    // --- Filter 2 cutoff + resonance (after bit reduction) ---
    float cutoff2 = params[FREQ2_PARAM].getValue();
    if (inputs[FREQ2CV_INPUT].isConnected())
    cutoff2 += inputs[FREQ2CV_INPUT].getVoltage() / 10.f;
	cutoff2 += lfo3Value / 10.f;  // scale ±5V → ±0.5 and sum with cutoff (0–1)
    cutoff2 = clamp(cutoff2, 0.f, 1.f);
    float cutoffHz2 = 80.f * powf(5000.f/80.f, cutoff2);

    float res2 = params[RES2_PARAM].getValue();
    if (inputs[RES2CV_INPUT].isConnected())
    res2 += inputs[RES2CV_INPUT].getVoltage() / 10.f;
    res2 = clamp(res2, 0.f, 1.f);
    float Q2 = 1.f + res2 * 4.f;

    float w0_2 = 2.f * float(M_PI) * cutoffHz2 / sampleRate;
    float alpha2 = sinf(w0_2) / (2.f * Q2);
    float cos_w0_2 = cosf(w0_2);
    float fb0_2 = (1.f - cos_w0_2) / 2.f;
    float fb1_2 = 1.f - cos_w0_2;
    float fb2_2 = (1.f - cos_w0_2) / 2.f;
    float fa0_2 = 1.f + alpha2;
    float fa1_2 = -2.f * cos_w0_2;
    float fa2_2 = 1.f - alpha2;

    float y2 = (fb0_2/fa0_2)*finalOutput + (fb1_2/fa0_2)*lp2_x1 + (fb2_2/fa0_2)*lp2_x2
               - (fa1_2/fa0_2)*lp2_y1 - (fa2_2/fa0_2)*lp2_y2;

    lp2_x2 = lp2_x1; lp2_x1 = finalOutput;
    lp2_y2 = lp2_y1; lp2_y1 = y2;

    float finalOutput2 = clamp(y2 * 0.5f, -10.f, 10.f);

// --- Final gain stage (1x → 100x) ---
float gainKnob = params[GAIN_PARAM].getValue(); // 0–1
float gain = 1.f + gainKnob * (100.f - 1.f);   // map 0–1 to 1→100x

float clipped = clamp(finalOutput2 * gain, -10.f, 10.f);

// --- Volume knob (0–1) applied after gain + clipping ---
float volume = params[VOL_PARAM].getValue(); // 0–1

float outputSignal = clipped * volume * 0.5f;

outputs[OUT_OUTPUT].setVoltage(outputSignal * (1.f - params[DEPTH2_PARAM].getValue() * (lfo2Value / 5.f)));
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

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(8.873, 99.304)), module, Stargazer::LFO1LEDRED_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(8.873, 99.304)), module, Stargazer::LFO1LEDGREEN_LIGHT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(69.881, 80.617)), module, Stargazer::LFO2LEDRED_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(69.881, 80.617)), module, Stargazer::LFO2LEDGREEN_LIGHT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(121.456, 105.787)), module, Stargazer::LFO3LEDRED_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(121.456, 105.787)), module, Stargazer::LFO3LEDGREEN_LIGHT));
	}
};


Model* modelStargazer = createModel<Stargazer, StargazerWidget>("Stargazer");