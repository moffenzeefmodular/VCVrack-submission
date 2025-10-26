#include "plugin.hpp"
#include "Rhythms.hpp"

struct Tantz : Module {
	enum ParamId {
		KICK_PARAM,
		SNARE_PARAM,
		HHCLOSED_PARAM,
		HHOPEN_PARAM,
		PERC1_PARAM,
		PERC2_PARAM,
		ROTATE_PARAM,
		SWING_PARAM,
		PW_PARAM,
		STYLE_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		ROTATECVIN_INPUT,
		SWINGCVIN_INPUT,
		PWCVIN_INPUT,
		CLOCKIN_INPUT,
		STYLECVIN_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		KICKOUT_OUTPUT,
		SNAREOUT_OUTPUT,
		HHCLOSEDOUT_OUTPUT,
		HHOPENOUT_OUTPUT,
		PERC1OUT_OUTPUT,
		PERC2OUT_OUTPUT,
		RESET_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		KICKLED_LIGHT,
		SNARELED_LIGHT,
		HHCLOSEDLED_LIGHT,
		HHOPENLED_LIGHT,
		PERC1LED_LIGHT,
		PERC2LED_LIGHT,
		RESETLED_LIGHT,
		LIGHTS_LEN
	};

	Tantz() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(KICK_PARAM, 0.f, 7.f, 0.f, "Kick", {"Pattern 1", "Pattern 2", "Pattern 3", "Pattern 4", "Pattern 5", "Pattern 6", "Pattern 7", "Pattern 8"});
		configSwitch(SNARE_PARAM, 0.f, 7.f, 0.f, "Snare", {"Pattern 1", "Pattern 2", "Pattern 3", "Pattern 4", "Pattern 5", "Pattern 6", "Pattern 7", "Pattern 8"});
		configSwitch(HHCLOSED_PARAM, 0.f, 7.f, 0.f, "Hi Hat Closed", {"Pattern 1", "Pattern 2", "Pattern 3", "Pattern 4", "Pattern 5", "Pattern 6", "Pattern 7", "Pattern 8"});
		configSwitch(HHOPEN_PARAM, 0.f, 7.f, 0.f, "Hi Hat Open", {"Pattern 1", "Pattern 2", "Pattern 3", "Pattern 4", "Pattern 5", "Pattern 6", "Pattern 7", "Pattern 8"});
		configSwitch(PERC1_PARAM, 0.f, 7.f, 0.f, "Perc 1", {"Pattern 1", "Pattern 2", "Pattern 3", "Pattern 4", "Pattern 5", "Pattern 6", "Pattern 7", "Pattern 8"});
		configSwitch(PERC2_PARAM, 0.f, 7.f, 0.f, "Perc 2", {"Pattern 1", "Pattern 2", "Pattern 3", "Pattern 4", "Pattern 5", "Pattern 6", "Pattern 7", "Pattern 8"});
		configSwitch(ROTATE_PARAM, 0.f, 5.f, 0.f, "Rotate");
		configParam(SWING_PARAM, 0.f, 1.f, 0.f, "Swing", "%", 0.f, 100.f);
		configParam(PW_PARAM, 0.f, 1.f, 0.f, "Pulsewidth", "%", 0.f, 100.f);
		configSwitch(STYLE_PARAM, 0.f, 7.f, 0.f, "Rhythm Style", {"Bulgar", "Sher", "Khosidl", "Skotshne", "Hora", "Zhok", "Araber", "Terkisher", "In Zibn"});
		configInput(ROTATECVIN_INPUT, "Rotate CV");
		configInput(SWINGCVIN_INPUT, "Swing CV");
		configInput(PWCVIN_INPUT, "Pulsewidth CV");
		configInput(CLOCKIN_INPUT, "Clock");
		configInput(STYLECVIN_INPUT, "Style CV");
		configOutput(KICKOUT_OUTPUT, "Kick Gate");
		configOutput(SNAREOUT_OUTPUT, "Snare Gate");
		configOutput(HHCLOSEDOUT_OUTPUT, "Hi Hat Closed Gate");
		configOutput(HHOPENOUT_OUTPUT, "Hi Hat Open Gate");
		configOutput(PERC1OUT_OUTPUT, "Percussion 1 Gate");
		configOutput(PERC2OUT_OUTPUT, "Percusison 2 Gate");
		configOutput(RESET_OUTPUT, "Reset Gate");
	}

// Rhythm storage
	RhythmData rhythmData;

	// Sequencer state
	int currentStep = 0;
	float lastClock = 0.0f;
	float gateTimers[RhythmData::NUM_DRUMS] = {};
	float resetGateTimer = 0.0f;

	// Pulsewidth limits (seconds)
	float minGateSec = 0.005f; // 5ms
	float maxGateSec = 0.1f; // 1-0ms

	// Convert PW knob 0–1 to 5–50ms gate length (seconds)
	float getGateLength(float pw) {
		return minGateSec + pw * (maxGateSec - minGateSec);
	}

	void process(const ProcessArgs& args) override {
		float dt = args.sampleTime;

		// PW knob 
		float pwKnob = params[PW_PARAM].getValue() * 5.f;       // 0–1 from knob
		float pwCV = (inputs[PWCVIN_INPUT].isConnected() ? inputs[PWCVIN_INPUT].getVoltage() : 0.0f);
		float pwSum = (pwKnob + pwCV) / 5.f;
		float pwFinal = clamp(pwSum, 0.0f, 1.0f);

		// --- Clock input ---
		float clock = inputs[CLOCKIN_INPUT].isConnected() ? inputs[CLOCKIN_INPUT].getVoltage() : 0.0f;

		// Rising edge detection
		if (clock > 1.0f && lastClock <= 1.0f) {
			currentStep++;
			int style = (int)params[STYLE_PARAM].getValue();
			int seqLength = rhythmData.sequenceLengths[style];

			if (currentStep >= seqLength) currentStep = 0;

			// Reset output on downbeat
			if (currentStep == 0) {
				resetGateTimer = getGateLength(pwFinal);
			}

			// Trigger drum gates
			for (int d = 0; d < RhythmData::NUM_DRUMS; d++) {
				int pattern = (int)params[KICK_PARAM + d].getValue();
				if (rhythmData.rhythms[style][d][pattern][currentStep]) {
					gateTimers[d] = getGateLength(pwFinal);
				}
			}
		}
		lastClock = clock;

	// --- Output gates ---
for (int d = 0; d < RhythmData::NUM_DRUMS; d++) {
    if (gateTimers[d] > 0.0f) {
        outputs[KICKOUT_OUTPUT + d].setVoltage(5.0f);
        gateTimers[d] -= dt;
    } else {
        outputs[KICKOUT_OUTPUT + d].setVoltage(0.0f);
    }
}

// --- Drum LEDs ---
for (int d = 0; d < RhythmData::NUM_DRUMS; d++) {
    if (gateTimers[d] > 0.0f) {
        lights[KICKLED_LIGHT + d].setBrightnessSmooth(1.0f, args.sampleTime);
    } else {
        lights[KICKLED_LIGHT + d].setBrightnessSmooth(0.0f, args.sampleTime);
    }
}

// --- Reset output ---
if (resetGateTimer > 0.0f) {
    outputs[RESET_OUTPUT].setVoltage(5.0f);
    resetGateTimer -= dt;
} else {
    outputs[RESET_OUTPUT].setVoltage(0.0f);
}

// --- Reset LED ---
if (resetGateTimer > 0.0f) {
    lights[RESETLED_LIGHT].setBrightnessSmooth(1.0f, args.sampleTime);
} else {
    lights[RESETLED_LIGHT].setBrightnessSmooth(0.0f, args.sampleTime);
}
}
};


struct TantzWidget : ModuleWidget {
	TantzWidget(Tantz* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/panels/Tantz.svg")));

		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(9.352, 24.497)), module, Tantz::KICK_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(24.06, 24.497)), module, Tantz::SNARE_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(38.767, 24.497)), module, Tantz::HHCLOSED_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(9.352, 53.719)), module, Tantz::HHOPEN_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(24.06, 53.719)), module, Tantz::PERC1_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(38.767, 53.719)), module, Tantz::PERC2_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(9.352, 81.09)), module, Tantz::ROTATE_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(24.06, 81.09)), module, Tantz::SWING_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(38.767, 81.09)), module, Tantz::PW_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(24.06, 107.201)), module, Tantz::STYLE_PARAM));

		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(9.352, 90.041)), module, Tantz::ROTATECVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(24.06, 90.041)), module, Tantz::SWINGCVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(38.767, 90.041)), module, Tantz::PWCVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(9.352, 116.152)), module, Tantz::CLOCKIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(24.06, 116.152)), module, Tantz::STYLECVIN_INPUT));

		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(9.352, 33.448)), module, Tantz::KICKOUT_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(24.06, 33.448)), module, Tantz::SNAREOUT_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(38.767, 33.448)), module, Tantz::HHCLOSEDOUT_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(9.352, 62.67)), module, Tantz::HHOPENOUT_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(24.06, 62.67)), module, Tantz::PERC1OUT_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(38.767, 62.67)), module, Tantz::PERC2OUT_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(38.767, 116.152)), module, Tantz::RESET_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(9.281, 17.563)), module, Tantz::KICKLED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(23.989, 17.563)), module, Tantz::SNARELED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(38.697, 17.563)), module, Tantz::HHCLOSEDLED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(9.281, 46.785)), module, Tantz::HHOPENLED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(23.989, 46.785)), module, Tantz::PERC1LED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(38.697, 46.785)), module, Tantz::PERC2LED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(38.767, 108.876)), module, Tantz::RESETLED_LIGHT));
	}
};


Model* modelTantz = createModel<Tantz, TantzWidget>("Tantz");