#include "plugin.hpp"

struct Kleztizer : Module {
	enum ParamId {
		KEY_PARAM,
		MODE_PARAM,
		CHORDBUTTON1_PARAM,
		CHORDBUTTON2_PARAM,
		CHORDBUTTON3_PARAM,
		CHORDBUTTON4_PARAM,
		CHORDBUTTON5_PARAM,
		CHORDBUTTON6_PARAM,
		CHORDBUTTON7_PARAM,
		CHORDVOICING_PARAM,
		INVERSION_PARAM,
		LEADCV1_PARAM,
		LEADOCTAVE1_PARAM,
		LEADCV2_PARAM,
		LEADOCTAVE2_PARAM,
		LEADOFFSET1_PARAM,
		LEADGATE1_PARAM,
		LEADOFFSET2_PARAM,
		LEADGATE2_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		KEYCV_INPUT,
		MODECV_INPUT,
		CHORDVOICINGCV_INPUT,
		CHORDINVERSIONCV_INPUT,
		CHORDSELECTCV_INPUT,
		LEADCV1_INPUT,
		LEADOCTAVECV1_INPUT,
		LEADCV2_INPUT,
		LEADOCTAVECV2_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		LEADGATEOUT1_OUTPUT,
		LEADGATEOUT2_OUTPUT,
		PEDALOUT_OUTPUT,
		CHORDROOTOUT_OUTPUT,
		CHORDOUT1_OUTPUT,
		CHORDOUT2_OUTPUT,
		CHORDOUT3_OUTPUT,
		CHORDOUT4_OUTPUT,
		LEADOUT1_OUTPUT,
		LEADOUT2_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LEADLED1_LIGHT,
		LEADLED_LIGHT,
		CHORDBUTTON1LED_LIGHT,
		CHORDBUTTON2LED_LIGHT,
		CHORDBUTTON3LED_LIGHT,
		CHORDBUTTON4LED_LIGHT,
		CHORDBUTTON5LED_LIGHT,
		CHORDBUTTON6LED_LIGHT,
		CHORDBUTTON7LED_LIGHT,
		LIGHTS_LEN
	};

	Kleztizer() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(KEY_PARAM, 0.f, 11.f, 2.f, "Key", {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"});
		configSwitch(MODE_PARAM, 0.f, 4.f, 0.f, "Mode", {"Freygish", "Mi Sheberach", "Adonai Malakh", "Magein Avot", "Harmonic Minor"});
		configSwitch(CHORDBUTTON1_PARAM, 0.f, 1.f, 0.f, "Chord 1");
		configSwitch(CHORDBUTTON2_PARAM, 0.f, 1.f, 0.f, "Chord 2");
		configSwitch(CHORDBUTTON3_PARAM, 0.f, 1.f, 0.f, "Chord 3");
		configSwitch(CHORDBUTTON4_PARAM, 0.f, 1.f, 0.f, "Chord 4");
		configSwitch(CHORDBUTTON5_PARAM, 0.f, 1.f, 0.f, "Chord 5");
		configSwitch(CHORDBUTTON6_PARAM, 0.f, 1.f, 0.f, "Chord 6");
		configSwitch(CHORDBUTTON7_PARAM, 0.f, 1.f, 0.f, "Chord 7");
		configSwitch(CHORDVOICING_PARAM, 0.f, 3.f, 0.f, "Voicing", {"Close", "Drop 2", "Drop 3", "Open"});
		configSwitch(INVERSION_PARAM, 0.f, 3.f, 0.f, "Inversion", {"Root", "1st", "2nd", "3rd"});
		configParam(LEADCV1_PARAM, 0.f, 1.f, 1.f, "Lead 1 CV Amount", "%", 0.f, 100.f);
		configSwitch(LEADOCTAVE1_PARAM, 0.f, 4.f, 2.f, "Lead 1 Octave", {"-2", "-1", "0", "+1", "+2"});
		configParam(LEADCV2_PARAM, 0.f, 1.f, 1.f, "Lead 2 CV Amount", "%", 0.f, 100.f);
		configSwitch(LEADOCTAVE2_PARAM, 0.f, 4.f, 2.f, "Lead 2 Octave", {"-2", "-1", "0", "+1", "+2"});
		configParam(LEADOFFSET1_PARAM, -2.f, 2.f, 0.f, "Lead 1 Note Offset", "v");
		configSwitch(LEADGATE1_PARAM, 0.f, 1.f, 0.f, "Lead 1", {"Gate", "Trig"});
		configParam(LEADOFFSET2_PARAM, -2.f, 2.f, 0.f, "Lead 2 Note Offset", "v");
		configSwitch(LEADGATE2_PARAM, 0.f, 1.f, 0.f, "Lead 2", {"Gate", "Trig"});
		configInput(KEYCV_INPUT, "Key CV");
		configInput(MODECV_INPUT, "Mode CV");
		configInput(CHORDVOICINGCV_INPUT, "Voicing CV");
		configInput(CHORDINVERSIONCV_INPUT, "Inversion CV");
		configInput(CHORDSELECTCV_INPUT, "Chord Select CV");
		configInput(LEADCV1_INPUT, "Lead 1 CV");
		configInput(LEADOCTAVECV1_INPUT, "Lead 1 Octave CV");
		configInput(LEADCV2_INPUT, "Lead 2 CV");
		configInput(LEADOCTAVECV2_INPUT, "Lead 2 Octave CV");
		configOutput(LEADGATEOUT1_OUTPUT, "Lead 1 Gate");
		configOutput(LEADGATEOUT2_OUTPUT, "Lead 2 Gate");
		configOutput(PEDALOUT_OUTPUT, "Pedal");
		configOutput(CHORDROOTOUT_OUTPUT, "Chord Root");
		configOutput(CHORDOUT1_OUTPUT, "Voice 1");
		configOutput(CHORDOUT2_OUTPUT, "Voice 2");
		configOutput(CHORDOUT3_OUTPUT, "Voice 3");
		configOutput(CHORDOUT4_OUTPUT, "Voice 4");
		configOutput(LEADOUT1_OUTPUT, "Lead 1");
		configOutput(LEADOUT2_OUTPUT, "Lead 2");
	}

	bool chordButtonStates[7] = {true};
	dsp::SchmittTrigger chordButtonTriggers[7];
	int chordSelect = 1; 

	const int chordButtonParams[7] = {
    CHORDBUTTON1_PARAM,
    CHORDBUTTON2_PARAM,
    CHORDBUTTON3_PARAM,
    CHORDBUTTON4_PARAM,
    CHORDBUTTON5_PARAM,
    CHORDBUTTON6_PARAM,
    CHORDBUTTON7_PARAM
};

const int chordButtonLights[7] = {
    CHORDBUTTON1LED_LIGHT,
    CHORDBUTTON2LED_LIGHT,
    CHORDBUTTON3LED_LIGHT,
    CHORDBUTTON4LED_LIGHT,
    CHORDBUTTON5LED_LIGHT,
    CHORDBUTTON6LED_LIGHT,
    CHORDBUTTON7LED_LIGHT
};

// --- (Optional) Tonic voltage cache ---
float tonicVoltage = 0.f;

// --- Scales (semitone offsets relative to tonic) ---
const int FREYGISH[7]       = {0, 1, 4, 5, 7, 8, 10};   // C – Db – E – F – G – Ab – Bb
const int MI_SHEBERACH[7]   = {0, 2, 3, 6, 7, 9, 10};   // C – D – Eb – F# – G – A – Bb
const int ADONAI_MALAKH[7]  = {0, 2, 4, 5, 7, 9, 10};   // C – D – E – F – G – A – Bb
const int MAGEIN_AVOT[7]    = {0, 2, 5, 6, 7, 9, 10};   // C – D – F – F# – G – A – Bb
const int HARMONIC_MINOR[7] = {0, 2, 3, 5, 7, 8, 11};   // C – D – Eb – F – G – Ab – B

// --- FREYGISH chords in C ---
const int FREYGISH_CHORDS[7][4] = {
    {0, 4, 7, 10},   // I: C7 (C E G Bb) – tonic dominant 7th
    {1, 5, 8, 0},    // ♭II: Dbmaj (Db F Ab C) – predominant
    {4, 7, 10, 2},   // #III dim: E° (E G Bb C) – rare diminished
    {5, 8, 0, 3},    // iv: Fm (F Ab C Eb) – predominant minor
    {7, 10, 2, 5},   // v dim: G° (G Bb D F) – dominant diminished
    {0, 0, 0, 0},    // VI+ skipped
    {10, 1, 5, 8}    // ♭VII: Bb minor (Bb Db F Ab) – dominant function
};

// --- MI SHEBERACH chords in C ---
const int MI_SHEBERACH_CHORDS[7][4] = {
    {0, 3, 7, 0},    // I: Cm (C Eb G C) – tonic minor
    {2, 6, 9, 0},    // II dom: D7 (D F# A C) – dominant 7th
    {3, 6, 10, 0},   // III: Ebmaj (Eb G Bb C) – major
    {6, 9, 0, 3},    // IV: F#m7b5 (F# A C E) – minor 7 flat 5
    {7, 10, 2, 5},   // V: Gm (G Bb D F) – minor predominant
    {9, 0, 4, 7},    // VI: Am (A C E G) – minor tonic
    {10, 2, 5, 9}    // VII: Bbmaj (Bb D F A) – major dominant
};

// --- ADONAI MALAKH chords in C ---
const int ADONAI_MALAKH_CHORDS[7][4] = {
    {0, 4, 7, 10},   // I: C7 (C E G Bb) – tonic dominant 7th
    {2, 5, 9, 0},    // II: Dm (D F A C) – minor predominant
    {4, 8, 11, 0},   // III: Edim (E G# B D) – diminished
    {5, 9, 12, 0},   // IV: Fmaj (F A C E) – major
    {7, 11, 14, 0},  // V: G7 (G B D F) – dominant 7th
    {9, 0, 4, 7},    // VI: Am (A C E G) – minor
    {10, 2, 5, 9}    // VII: Bbmaj (Bb D F A) – major
};

// --- MAGEIN AVOT chords in C ---
const int MAGEIN_AVOT_CHORDS[7][4] = {
    {0, 3, 7, 10},   // I: Cm7 (C Eb G Bb) – minor 7th
    {2, 5, 9, 0},    // II: Dm (D F A C) – minor
    {5, 8, 0, 3},    // III: Fdim (F Ab C Eb) – diminished
    {6, 9, 0, 3},    // IV: F#m7b5 (F# A C E) – minor 7 flat 5
    {7, 11, 14, 0},  // V: G7 (G B D F) – dominant 7th
    {9, 0, 4, 7},    // VI: Am (A C E G) – minor
    {10, 2, 5, 9}    // VII: Bbmaj (Bb D F A) – major
};

// --- HARMONIC MINOR chords in C ---
const int HARMONIC_MINOR_CHORDS[7][4] = {
    {0, 3, 7, 0},    // I: Cm (C Eb G C) – tonic minor
    {2, 5, 9, 0},    // II dim: Ddim (D F Ab C) – diminished
    {4, 8, 11, 0},   // III+: E+ (E G# B D) – augmented
    {5, 8, 12, 0},   // IV: Fm (F Ab C F) – minor
    {7, 11, 14, 0},  // V7: G7 (G B D F) – dominant 7th
    {9, 0, 4, 7},    // VI: Abmaj (Ab C Eb G) – major
    {11, 2, 5, 9}    // VII dim: Bdim (B D F A) – diminished
};


// --- Lead processing state ---
float leadGateTimer[2] = {0.f, 0.f};
float leadLedTimer[2]  = {0.f, 0.f};
int   lastQuantizedNote[2] = {-999, -999};



void process(const ProcessArgs& args) override {

// --- Chord buttons + CV ---
for (int i = 0; i < 7; i++) {
    // Handle physical button presses
    if (chordButtonTriggers[i].process(params[chordButtonParams[i]].getValue())) {
        for (int j = 0; j < 7; j++) chordButtonStates[j] = (i == j);
        chordSelect = i + 1;
    }
}

// Handle CV input (overrides buttons if connected)
if (inputs[CHORDSELECTCV_INPUT].isConnected()) {
    float cv = clamp(inputs[CHORDSELECTCV_INPUT].getVoltage(), -5.f, 5.f);
    // Map -5..+5 → 1..7 manually
    int cvChord = int(roundf(((cv + 5.f) / 10.f) * 6.f)) + 1; // ((cv+5)/10)*6 → 0..6, +1 → 1..7
    chordSelect = clamp(cvChord, 1, 7);
    for (int j = 0; j < 7; j++)
        chordButtonStates[j] = (j == chordSelect - 1);
}

// Update button LEDs
for (int i = 0; i < 7; i++) {
    lights[chordButtonLights[i]].setBrightnessSmooth(chordButtonStates[i] ? 1.f : 0.f, args.sampleTime);
}


	// Key and mode 
    float keyCV = inputs[KEYCV_INPUT].isConnected() ? inputs[KEYCV_INPUT].getVoltage() : 0.f;
    float keyNorm = params[KEY_PARAM].getValue() / 11.f + keyCV / 10.f;
    int keyIndex = clamp(int(roundf(keyNorm * 11.f)), 0, 11);
    tonicVoltage = keyIndex / 12.f;
    outputs[PEDALOUT_OUTPUT].setVoltage(tonicVoltage - 2.f); // 2 octaves down

    float modeCV = inputs[MODECV_INPUT].isConnected() ? inputs[MODECV_INPUT].getVoltage() : 0.f;
    float modeNorm = params[MODE_PARAM].getValue() / 4.f + modeCV / 10.f;
    int modeIndex = clamp(int(roundf(modeNorm * 4.f)), 0, 4);

    const int* currentScale = nullptr;
    switch(modeIndex) {
        case 0: currentScale = FREYGISH; break;
        case 1: currentScale = MI_SHEBERACH; break;
        case 2: currentScale = ADONAI_MALAKH; break;
        case 3: currentScale = MAGEIN_AVOT; break;
        case 4: currentScale = HARMONIC_MINOR; break;
    }

// --- Chord output based on selected button + inversion + voicing ---
const int (*chordTable)[4] = nullptr;
switch (modeIndex) {
    case 0: chordTable = FREYGISH_CHORDS; break;
    case 1: chordTable = MI_SHEBERACH_CHORDS; break;
    case 2: chordTable = ADONAI_MALAKH_CHORDS; break;
    case 3: chordTable = MAGEIN_AVOT_CHORDS; break;
    case 4: chordTable = HARMONIC_MINOR_CHORDS; break;
}

if (chordSelect > 0 && chordTable) {
    int chordIndex = chordSelect - 1;

    // --- Inversion knob + CV ---
    float invParam = params[INVERSION_PARAM].getValue();
    if (inputs[CHORDINVERSIONCV_INPUT].isConnected()) {
        invParam += (inputs[CHORDINVERSIONCV_INPUT].getVoltage() / 5.f) * 3.f;
    }
    int inversion = clamp((int)roundf(invParam), 0, 3);  // 0 = root, 1 = 1st, 2 = 2nd, 3 = 3rd

    // --- Voicing knob + CV ---
    float voiceParam = params[CHORDVOICING_PARAM].getValue();
    if (inputs[CHORDVOICINGCV_INPUT].isConnected()) {
        voiceParam += (inputs[CHORDVOICINGCV_INPUT].getVoltage() / 5.f) * 3.f;
    }
    int voicing = clamp((int)roundf(voiceParam), 0, 3); // 0=Close,1=Drop2,2=Drop3,3=Open

    // --- Apply inversion first: rotate chord voices ---
    int chordVoices[4];
    for (int v = 0; v < 4; v++) {
        int voiceIndex = (v + inversion) % 4;
        chordVoices[v] = chordTable[chordIndex][voiceIndex];
    }

    // --- Apply voicing: reorder notes ---
    int finalVoices[4];
    switch (voicing) {
        case 0: // Close (no change)
            for (int v = 0; v < 4; v++) finalVoices[v] = chordVoices[v];
            break;
        case 1: // Drop 2: second highest note down an octave
            finalVoices[0] = chordVoices[1] - 12;
            finalVoices[1] = chordVoices[0];
            finalVoices[2] = chordVoices[2];
            finalVoices[3] = chordVoices[3];
            break;
        case 2: // Drop 3: third highest note down an octave
            finalVoices[0] = chordVoices[2] - 12;
            finalVoices[1] = chordVoices[0];
            finalVoices[2] = chordVoices[1];
            finalVoices[3] = chordVoices[3];
            break;
        case 3: // Open: first and third note down an octave
            finalVoices[0] = chordVoices[0] - 12;
            finalVoices[1] = chordVoices[1];
            finalVoices[2] = chordVoices[2] - 12;
            finalVoices[3] = chordVoices[3];
            break;
    }

    // --- Output chord voices ---
    for (int v = 0; v < 4; v++) {
        float chordVoltage = tonicVoltage + finalVoices[v] / 12.f;
        outputs[CHORDOUT1_OUTPUT + v].setVoltage(chordVoltage);
    }

} else {
    // No button pressed: clear chord outputs
    for (int v = 0; v < 4; v++) outputs[CHORDOUT1_OUTPUT + v].setVoltage(0.f);
}

// --- Lead processing loop (Lead 1 + Lead 2) ---
for (int lead = 0; lead < 2; lead++) {
	// Map per-lead params/IO
	int LEADCV_INPUT_ID       = (lead == 0) ? LEADCV1_INPUT       : LEADCV2_INPUT;
	int LEADOCTAVECV_INPUT_ID = (lead == 0) ? LEADOCTAVECV1_INPUT : LEADOCTAVECV2_INPUT;
	int LEADCV_PARAM_ID       = (lead == 0) ? LEADCV1_PARAM       : LEADCV2_PARAM;
	int LEADOCTAVE_PARAM_ID   = (lead == 0) ? LEADOCTAVE1_PARAM   : LEADOCTAVE2_PARAM;
	int LEADOFFSET_PARAM_ID   = (lead == 0) ? LEADOFFSET1_PARAM   : LEADOFFSET2_PARAM;
	int LEADGATE_PARAM_ID     = (lead == 0) ? LEADGATE1_PARAM     : LEADGATE2_PARAM;
	int LEADOUT_OUTPUT_ID     = (lead == 0) ? LEADOUT1_OUTPUT     : LEADOUT2_OUTPUT;
	int LEADGATEOUT_OUTPUT_ID = (lead == 0) ? LEADGATEOUT1_OUTPUT : LEADGATEOUT2_OUTPUT;
	int LEADLED_LIGHT_ID      = (lead == 0) ? LEADLED1_LIGHT      : LEADLED_LIGHT;

	// --- CV + quantization ---
	float rawCV = inputs[LEADCV_INPUT_ID].isConnected() ? inputs[LEADCV_INPUT_ID].getVoltage() : 0.f;
	float leadCVAtt = rawCV * params[LEADCV_PARAM_ID].getValue();
	float totalCV = leadCVAtt + params[LEADOFFSET_PARAM_ID].getValue();
	float semitoneOffset = totalCV * 12.f;

	// Find closest scale degree
	int closestNote = 0;
	float minDiff = 9999.f;
	for (int octave = -2; octave <= 2; octave++) {
		for (int i = 0; i < 7; i++) {
			float candidate = currentScale[i] + octave * 12;
			float diff = fabs(semitoneOffset - candidate);
			if (diff < minDiff) {
				minDiff = diff;
				closestNote = candidate;
			}
		}
	}

	// Octave shift
	float octaveParam = params[LEADOCTAVE_PARAM_ID].getValue();
	if (inputs[LEADOCTAVECV_INPUT_ID].isConnected()) {
		octaveParam += (inputs[LEADOCTAVECV_INPUT_ID].getVoltage() / 5.f) * 4.f;
	}
	octaveParam = clamp(octaveParam, 0.f, 4.f);
	int octaveShift = (int)roundf(octaveParam) - 2;
	closestNote += octaveShift * 12;

	// Output quantized pitch
	outputs[LEADOUT_OUTPUT_ID].setVoltage(tonicVoltage + closestNote / 12.f);

	// --- Gate / LED timing ---
	bool isTrigMode = (params[LEADGATE_PARAM_ID].getValue() > 0.5f);
	float trigDur  = 0.005f;
	float gateDur  = 0.5f;

	if (closestNote != lastQuantizedNote[lead]) {
		lastQuantizedNote[lead] = closestNote;
		leadGateTimer[lead] = isTrigMode ? trigDur : gateDur;
		leadLedTimer[lead]  = isTrigMode ? trigDur : gateDur;
	}

	// Gate output
	if (leadGateTimer[lead] > 0.f) {
		leadGateTimer[lead] -= args.sampleTime;
		outputs[LEADGATEOUT_OUTPUT_ID].setVoltage(5.f);
	} else {
		outputs[LEADGATEOUT_OUTPUT_ID].setVoltage(0.f);
	}

	// LED output
	if (leadLedTimer[lead] > 0.f) {
		leadLedTimer[lead] -= args.sampleTime;
		lights[LEADLED_LIGHT_ID].setBrightnessSmooth(1.f, args.sampleTime);
	} else {
		lights[LEADLED_LIGHT_ID].setBrightnessSmooth(0.f, args.sampleTime);
	}
}
}
};


struct KleztizerWidget : ModuleWidget {
	KleztizerWidget(Kleztizer* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/panels/Kleztizer.svg")));

		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(42.667, 12.888)), module, Kleztizer::KEY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(68.488, 12.888)), module, Kleztizer::MODE_PARAM));


		addParam(createParamCentered<VCVBezel>(mm2px(Vec(14.286, 43.158)), module, Kleztizer::CHORDBUTTON1_PARAM));
   	    addParam(createLightParamCentered<VCVLightBezel<>>(mm2px(Vec(14.286, 43.158)), module, Kleztizer::CHORDBUTTON1_PARAM,Kleztizer::CHORDBUTTON1LED_LIGHT));

		addParam(createParamCentered<VCVBezel>(mm2px(Vec(26.475, 43.158)), module, Kleztizer::CHORDBUTTON2_PARAM));
		addParam(createLightParamCentered<VCVLightBezel<>>(mm2px(Vec(26.475, 43.158)), module, Kleztizer::CHORDBUTTON2_PARAM,Kleztizer::CHORDBUTTON2LED_LIGHT));

		addParam(createParamCentered<VCVBezel>(mm2px(Vec(38.663, 43.158)), module, Kleztizer::CHORDBUTTON3_PARAM));
   	    addParam(createLightParamCentered<VCVLightBezel<>>(mm2px(Vec(38.663, 43.158)), module, Kleztizer::CHORDBUTTON3_PARAM,Kleztizer::CHORDBUTTON3LED_LIGHT));

		addParam(createParamCentered<VCVBezel>(mm2px(Vec(50.851, 43.158)), module, Kleztizer::CHORDBUTTON4_PARAM));
   	    addParam(createLightParamCentered<VCVLightBezel<>>(mm2px(Vec(50.851, 43.158)), module, Kleztizer::CHORDBUTTON4_PARAM,Kleztizer::CHORDBUTTON4LED_LIGHT));

		addParam(createParamCentered<VCVBezel>(mm2px(Vec(63.039, 43.158)), module, Kleztizer::CHORDBUTTON5_PARAM));
   	    addParam(createLightParamCentered<VCVLightBezel<>>(mm2px(Vec(63.039, 43.158)), module, Kleztizer::CHORDBUTTON5_PARAM,Kleztizer::CHORDBUTTON5LED_LIGHT));

		addParam(createParamCentered<VCVBezel>(mm2px(Vec(75.227, 43.158)), module, Kleztizer::CHORDBUTTON6_PARAM));
   	    addParam(createLightParamCentered<VCVLightBezel<>>(mm2px(Vec(75.227, 43.158)), module, Kleztizer::CHORDBUTTON6_PARAM,Kleztizer::CHORDBUTTON6LED_LIGHT));

		addParam(createParamCentered<VCVBezel>(mm2px(Vec(87.415, 43.158)), module, Kleztizer::CHORDBUTTON7_PARAM));
		addParam(createLightParamCentered<VCVLightBezel<>>(mm2px(Vec(87.415, 43.158)), module, Kleztizer::CHORDBUTTON7_PARAM,Kleztizer::CHORDBUTTON7LED_LIGHT));


		addParam(createParamCentered<CKSS>(mm2px(Vec(38.49, 86.696)), module, Kleztizer::LEADGATE1_PARAM));
		addParam(createParamCentered<CKSS>(mm2px(Vec(89.465, 86.696)), module, Kleztizer::LEADGATE2_PARAM));

		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.67, 79.249)), module, Kleztizer::LEADCV1_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(30.073, 79.249)), module, Kleztizer::LEADOCTAVE1_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(69.644, 79.249)), module, Kleztizer::LEADCV2_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(81.047, 79.249)), module, Kleztizer::LEADOCTAVE2_PARAM));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.867, 54.852)), module, Kleztizer::CHORDVOICING_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(81.708, 54.852)), module, Kleztizer::INVERSION_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.182, 86.696)), module, Kleztizer::LEADOFFSET1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(59.156, 86.696)), module, Kleztizer::LEADOFFSET2_PARAM));

		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(32.235, 12.888)), module, Kleztizer::KEYCV_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(57.911, 12.888)), module, Kleztizer::MODECV_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(20.0, 54.852)), module, Kleztizer::CHORDVOICINGCV_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(92.502, 54.852)), module, Kleztizer::CHORDINVERSIONCV_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(51.443, 62.876)), module, Kleztizer::CHORDSELECTCV_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(18.67, 94.8)), module, Kleztizer::LEADCV1_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(30.073, 94.8)), module, Kleztizer::LEADOCTAVECV1_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(69.644, 94.8)), module, Kleztizer::LEADCV2_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(81.047, 94.8)), module, Kleztizer::LEADOCTAVECV2_INPUT));

		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(46.25, 86.696)), module, Kleztizer::LEADGATEOUT1_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(97.224, 86.696)), module, Kleztizer::LEADGATEOUT2_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(7.556, 115.947)), module, Kleztizer::PEDALOUT_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(19.665, 115.947)), module, Kleztizer::CHORDROOTOUT_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(31.774, 115.947)), module, Kleztizer::CHORDOUT1_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(43.883, 115.947)), module, Kleztizer::CHORDOUT2_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(55.992, 115.947)), module, Kleztizer::CHORDOUT3_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(68.102, 115.947)), module, Kleztizer::CHORDOUT4_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(80.211, 115.947)), module, Kleztizer::LEADOUT1_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(92.32, 115.947)), module, Kleztizer::LEADOUT2_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(46.38, 79.249)), module, Kleztizer::LEADLED1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(97.354, 79.249)), module, Kleztizer::LEADLED_LIGHT));
	}
};


Model* modelKleztizer = createModel<Kleztizer, KleztizerWidget>("Kleztizer");