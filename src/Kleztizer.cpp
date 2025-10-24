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
		configParam(LEADGATE1_PARAM, 0.f, 1.f, 0.f, "Lead 1 Gate");
		configParam(LEADOFFSET2_PARAM, -2.f, 2.f, 0.f, "Lead 2 Note Offset", "v");
		configParam(LEADGATE2_PARAM, 0.f, 1.f, 0.f, "Lead 2 Gate");
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

	bool chordButtonStates[7] = {false};
	dsp::SchmittTrigger chordButtonTriggers[7];
	int chordSelect = 0; 

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

// --- Lead 1 snapping / scale ---
const int freygishScale[7] = {0, 1, 4, 5, 7, 8, 10};  // Freygish scale in semitones

// --- (Optional) Tonic voltage cache ---
float tonicVoltage = 0.f;
 
float lead1CVAttenuated = 0.f;  // After CV input attenuator

// --- Scales (semitone offsets relative to tonic) ---
const int FREYGISH[7]       = {0, 1, 4, 5, 7, 8, 10};
const int MI_SHEBERACH[7]   = {0, 1, 3, 5, 7, 8, 10};  // swapped position
const int ADONAI_MALAKH[7]  = {0, 2, 3, 5, 7, 8, 10};  // swapped position
const int MAGEIN_AVOT[7]    = {0, 2, 4, 5, 7, 8, 10};
const int HARMONIC_MINOR[7] = {0, 2, 3, 5, 7, 8, 11};

void process(const ProcessArgs& args) override {

    for (int i = 0; i < 7; i++) {
        if (chordButtonTriggers[i].process(params[chordButtonParams[i]].getValue())) {
            for (int j = 0; j < 7; j++) chordButtonStates[j] = (i == j);
            chordSelect = i + 1;
        }
        lights[chordButtonLights[i]].setBrightnessSmooth(chordButtonStates[i] ? 1.f : 0.f, args.sampleTime);
    }

    float keyCV = inputs[KEYCV_INPUT].isConnected() ? inputs[KEYCV_INPUT].getVoltage() : 0.f;
    float keyNorm = params[KEY_PARAM].getValue() / 11.f + keyCV / 10.f;
    int keyIndex = clamp(int(roundf(keyNorm * 11.f)), 0, 11);
    tonicVoltage = keyIndex / 12.f;
    outputs[PEDALOUT_OUTPUT].setVoltage(tonicVoltage - 1.f);

    float rawCV = inputs[LEADCV1_INPUT].isConnected() ? inputs[LEADCV1_INPUT].getVoltage() : 0.f;
    float lead1CVAtt = rawCV * params[LEADCV1_PARAM].getValue();
    float totalCV = lead1CVAtt + params[LEADOFFSET1_PARAM].getValue();
    float semitoneOffset = totalCV * 12.f;

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

    int closestNote = currentScale[0];
    float minDiff = fabs(semitoneOffset - currentScale[0]);
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

	float octaveParam = params[LEADOCTAVE1_PARAM].getValue(); // 0..4
	if (inputs[LEADOCTAVECV1_INPUT].isConnected()) {
		octaveParam += (inputs[LEADOCTAVECV1_INPUT].getVoltage() / 5.f) * 4.f; // -5V..5V -> -4..4
	}
	octaveParam = clamp(octaveParam, 0.f, 4.f);
	int octaveShift = (int)roundf(octaveParam) - 2; // -2..+2
	closestNote += octaveShift * 12;
	
	
    outputs[LEADOUT1_OUTPUT].setVoltage(tonicVoltage + closestNote / 12.f);
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
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(97.354, 79.249)), module, Kleztizer::LEADLED1_LIGHT));
	}
};


Model* modelKleztizer = createModel<Kleztizer, KleztizerWidget>("Kleztizer");