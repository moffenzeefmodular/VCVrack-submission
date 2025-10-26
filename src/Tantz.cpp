#include "plugin.hpp"
#include "Rhythms.hpp"

struct Tantz : Module {
	enum ParamId {
		RUN_PARAM,
		STYLE_PARAM,
		KICK_PARAM,
		SNARE_PARAM,
		HHCLOSED_PARAM,
		KICKMUTE_PARAM,
		SNAREMUTE_PARAM,
		HHCLOSEDMUTE_PARAM,
		ROTATE_PARAM,
		HHOPEN_PARAM,
		PERC1_PARAM,
		PERC2_PARAM,
		SWING_PARAM,
		PW_PARAM,
		HHOPENMUTE_PARAM,
		PERC1MUTE_PARAM,
		PERC2MUTE_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		RUNCVIN_INPUT,
		STYLECVIN_INPUT,
		ROTATECVIN_INPUT,
		SWINGCVIN_INPUT,
		PWCVIN_INPUT,
		KICKCVIN_INPUT,
		SNARECVIN_INPUT,
		HHCLOSEDCVIN_INPUT,
		CLOCKIN_INPUT,
		HHOPENCVIN_INPUT,
		PERC1CVIN_INPUT,
		PERC2CVIN_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		RESET_OUTPUT,
		KICKOUT_OUTPUT,
		SNAREOUT_OUTPUT,
		HHCLOSEDOUT_OUTPUT,
		HHOPENOUT_OUTPUT,
		PERC1OUT_OUTPUT,
		PERC2OUT_OUTPUT,
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
		RUNLED_LIGHT,
		LIGHTS_LEN
	};

	Tantz() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(KICK_PARAM, 0.f, 7.f, 0.f, "Kick", {"Pattern 1", "Pattern 2", "Pattern 3", "Pattern 4", "Pattern 5", "Pattern 6", "Pattern 7", "Pattern 8"});
		configSwitch(SNARE_PARAM, 0.f, 7.f, 0.f, "Snare", {"Pattern 1", "Pattern 2", "Pattern 3", "Pattern 4", "Pattern 5", "Pattern 6", "Pattern 7", "Pattern 8"});
		configSwitch(HHCLOSED_PARAM, 0.f, 7.f, 0.f, "Hi Hat Closed", {"Pattern 1", "Pattern 2", "Pattern 3", "Pattern 4", "Pattern 5", "Pattern 6", "Pattern 7", "Pattern 8"});
		configSwitch(HHOPEN_PARAM, 0.f, 7.f, 0.f, "Hi Hat Open", {"Pattern 1", "Pattern 2", "Pattern 3", "Pattern 4", "Pattern 5", "Pattern 6", "Pattern 7", "Pattern 8"});
		configSwitch(PERC1_PARAM, 0.f, 7.f, 0.f, "Percussion 1", {"Pattern 1", "Pattern 2", "Pattern 3", "Pattern 4", "Pattern 5", "Pattern 6", "Pattern 7", "Pattern 8"});
		configSwitch(PERC2_PARAM, 0.f, 7.f, 0.f, "Percussion 2", {"Pattern 1", "Pattern 2", "Pattern 3", "Pattern 4", "Pattern 5", "Pattern 6", "Pattern 7", "Pattern 8"});
		
		configSwitch(RUN_PARAM, 0.f, 1.f, 0.f, "Run", {"Start", "Stop"});
		configSwitch(ROTATE_PARAM, 0.f, 5.f, 0.f, "Rotate");
		configParam(SWING_PARAM, 0.f, 1.f, 0.f, "Swing", "%", 0.f, 100.f);
		configParam(PW_PARAM, 0.f, 1.f, 0.5f, "Pulsewidth", "%", 0.f, 100.f);
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

		configSwitch(KICKMUTE_PARAM, 0.f, 1.f, 1.f, "Kick", {"Muted", "Unmuted"});
		configSwitch(SNAREMUTE_PARAM, 0.f, 1.f, 1.f, "Snare", {"Muted", "Unmuted"});
		configSwitch(HHCLOSEDMUTE_PARAM, 0.f, 1.f, 1.f, "Hi Hat Closed", {"Muted", "Unmuted"});
		configSwitch(HHOPENMUTE_PARAM, 0.f, 1.f, 1.f, "Hi Hat Open", {"Muted", "Unmuted"});
		configSwitch(PERC1MUTE_PARAM, 0.f, 1.f, 1.f, "Percussion 1", {"Muted", "Unmuted"});
		configSwitch(PERC2MUTE_PARAM, 0.f, 1.f, 1.f, "Percussion 2", {"Muted", "Unmuted"});

		configInput(RUNCVIN_INPUT, "Run CV");
		configInput(KICKCVIN_INPUT, "Kick CV");
		configInput(SNARECVIN_INPUT, "Snare CV");
		configInput(HHCLOSEDCVIN_INPUT, "Hi Hat Closed CV");
		configInput(HHOPENCVIN_INPUT, "Hi Hat Open CV");
		configInput(PERC1CVIN_INPUT, "Perc 1 CV");
		configInput(PERC2CVIN_INPUT, "Perc 2 CV");
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

    // --- Pulsewidth ---
    float pwKnob = params[PW_PARAM].getValue() * 5.f;
    float pwCV = (inputs[PWCVIN_INPUT].isConnected() ? inputs[PWCVIN_INPUT].getVoltage() : 0.0f);
    float pwSum = (pwKnob + pwCV) / 5.f;
    float pwFinal = clamp(pwSum, 0.0f, 1.0f);

    // --- Swing ---
    float swingKnob = params[SWING_PARAM].getValue();       // 0..1
    float swingCV = (inputs[SWINGCVIN_INPUT].isConnected() ? inputs[SWINGCVIN_INPUT].getVoltage() / 10.0f : 0.0f);
    float swingAmount = clamp(swingKnob + swingCV, 0.0f, 1.0f) * 0.5f;  // 0..0.5 (0–50%)

    // --- Rotation ---
    float rotateKnob = params[ROTATE_PARAM].getValue();
    float rotateCV = (inputs[ROTATECVIN_INPUT].isConnected() ? inputs[ROTATECVIN_INPUT].getVoltage() : 0.0f);
    int rotateSteps = (int)round(clamp(rotateKnob + rotateCV, 0.0f, 5.0f)) % RhythmData::NUM_DRUMS;

    // --- Clock input ---
    float clock = inputs[CLOCKIN_INPUT].isConnected() ? inputs[CLOCKIN_INPUT].getVoltage() : 0.0f;
    static float lastClock = 0.0f;
    static float currentStepTime = 0.0f;   // Time since last step
    static float stepInterval = 0.0f;      // Last measured interval
    static bool stepPending = false;       // True if waiting for swing delay

    currentStepTime += dt;

    if (clock > 1.0f && lastClock <= 1.0f) {
        // New rising edge
        stepInterval = currentStepTime; // measure last step interval
        currentStepTime = 0.0f;

        // Schedule swing delay for odd steps
        stepPending = true;
    }

    lastClock = clock;

    if (stepPending) {
        // Compute delay for this step
        float delay = (currentStep % 2 == 1) ? swingAmount * stepInterval : 0.0f;
        if (currentStepTime >= delay) {
            // Advance step
            currentStep++;
            int style = (int)params[STYLE_PARAM].getValue();
            int seqLength = rhythmData.sequenceLengths[style];
            if (currentStep >= seqLength) currentStep = 0;

       // Reset on final step
			if (currentStep == seqLength - 1)
     		resetGateTimer = getGateLength(pwFinal);

            // Trigger drum gates with rotation
            for (int d = 0; d < RhythmData::NUM_DRUMS; d++) {
                int pattern = (int)params[KICK_PARAM + d].getValue();
                bool trigger = rhythmData.rhythms[style][d][pattern][currentStep];
                if (trigger) {
                    int rotatedIndex = (d + rotateSteps) % RhythmData::NUM_DRUMS;
                    gateTimers[rotatedIndex] = getGateLength(pwFinal);
                }
            }

            // Step fired
            stepPending = false;
        }
    }

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
    for (int d = 0; d < RhythmData::NUM_DRUMS; d++)
        lights[KICKLED_LIGHT + d].setBrightnessSmooth(gateTimers[d] > 0.0f ? 1.0f : 0.0f, dt);

    // --- Reset output and LED ---
    if (resetGateTimer > 0.0f) {
        outputs[RESET_OUTPUT].setVoltage(5.0f);
        lights[RESETLED_LIGHT].setBrightnessSmooth(1.0f, dt);
        resetGateTimer -= dt;
    } else {
        outputs[RESET_OUTPUT].setVoltage(0.0f);
        lights[RESETLED_LIGHT].setBrightnessSmooth(0.0f, dt);
    }
}
};


struct TantzWidget : ModuleWidget {
	TantzWidget(Tantz* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/panels/Tantz.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<VCVBezel>(mm2px(Vec(75.202, 17.812)), module, Tantz::RUN_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(91.647, 17.812)), module, Tantz::STYLE_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(83.988, 42.871)), module, Tantz::ROTATE_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(91.647, 66.929)), module, Tantz::PW_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(75.682, 66.929)), module, Tantz::SWING_PARAM));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.235, 30.086)), module, Tantz::KICK_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.665, 30.088)), module, Tantz::SNARE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(57.199, 30.091)), module, Tantz::HHCLOSED_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.235, 64.67)), module, Tantz::HHOPEN_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.665, 64.67)), module, Tantz::PERC1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(57.824, 64.523)), module, Tantz::PERC2_PARAM));

		addParam(createParamCentered<CKSS>(mm2px(Vec(10.235, 40.625)), module, Tantz::KICKMUTE_PARAM));
		addParam(createParamCentered<CKSS>(mm2px(Vec(10.235, 75.208)), module, Tantz::HHOPENMUTE_PARAM));
		addParam(createParamCentered<CKSS>(mm2px(Vec(34.665, 75.208)), module, Tantz::PERC1MUTE_PARAM));
		addParam(createParamCentered<CKSS>(mm2px(Vec(57.824, 75.061)), module, Tantz::PERC2MUTE_PARAM));
		addParam(createParamCentered<CKSS>(mm2px(Vec(34.665, 40.626)), module, Tantz::SNAREMUTE_PARAM));
		addParam(createParamCentered<CKSS>(mm2px(Vec(57.199, 40.63)), module, Tantz::HHCLOSEDMUTE_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(75.202, 29.812)), module, Tantz::RUNCVIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.647, 29.812)), module, Tantz::STYLECVIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(83.988, 54.871)), module, Tantz::ROTATECVIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(75.682, 78.929)), module, Tantz::SWINGCVIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.647, 78.929)), module, Tantz::PWCVIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(23.94, 100.566)), module, Tantz::KICKCVIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.074, 100.566)), module, Tantz::SNARECVIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(51.161, 100.566)), module, Tantz::HHCLOSEDCVIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.235, 114.234)), module, Tantz::CLOCKIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(23.94, 114.234)), module, Tantz::HHOPENCVIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.074, 114.234)), module, Tantz::PERC1CVIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(51.161, 114.234)), module, Tantz::PERC2CVIN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.235, 100.566)), module, Tantz::RESET_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(64.505, 100.566)), module, Tantz::KICKOUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(77.64, 100.566)), module, Tantz::SNAREOUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(91.726, 100.566)), module, Tantz::HHCLOSEDOUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(64.505, 114.234)), module, Tantz::HHOPENOUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(77.64, 114.234)), module, Tantz::PERC1OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(91.726, 114.234)), module, Tantz::PERC2OUT_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(10.164, 21.565)), module, Tantz::KICKLED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(34.594, 21.566)), module, Tantz::SNARELED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(57.128, 21.57)), module, Tantz::HHCLOSEDLED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(10.164, 56.148)), module, Tantz::HHOPENLED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(34.594, 56.148)), module, Tantz::PERC1LED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(57.754, 56.001)), module, Tantz::PERC2LED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(10.235, 93.103)), module, Tantz::RESETLED_LIGHT));
	}
};


Model* modelTantz = createModel<Tantz, TantzWidget>("Tantz");