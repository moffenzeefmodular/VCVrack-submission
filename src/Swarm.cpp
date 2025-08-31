#include "plugin.hpp"


// === CHIRP CLASS CONVERSION (FROM ARDUINO) === //
class Chirp {
private:
    float sinceStepSamples = 0.0f;
    int currentStep = 0;
    int seqLength = 16;
    int gates[64];
    int lastFreeze = 0;
    int currentFreeze = 0;

public:
    float timeRatio = 1.0f;
    int freeze = 0;

    Chirp() {
        for (int i = 0; i < 64; i++) {
            gates[i] = rand() % 2;
        }
    }

    void reset() {
        sinceStepSamples = 0.0f;
        currentStep = 0;
    }

    int update(float basePeriodMs, float width, float sampleRate) {
        const float msPerSample = 1.0f / (sampleRate * 0.001f);
        const float stepDurationMs = basePeriodMs * timeRatio;
        const float stepDurationSamples = stepDurationMs / msPerSample;

        lastFreeze = currentFreeze;
        currentFreeze = freeze;

        if (currentFreeze > lastFreeze) {
            seqLength = (rand() % 64) + 1;
        }

        if (sinceStepSamples >= stepDurationSamples) {
            sinceStepSamples -= stepDurationSamples;
            currentStep++;
            if (currentStep >= seqLength)
                currentStep = 0;

            if (freeze == 0) {
                const int density = 20; // % chance
                gates[rand() % seqLength] = (rand() % 100 <= density);
            }
        }

        sinceStepSamples += 1.0f;

        if ((sinceStepSamples < (stepDurationSamples * width)) && gates[currentStep] == 1) {
            return 1;
        } else {
            return 0;
        }
    }
};

struct Swarm : Module {
    enum ParamId {
        TIMECOARSE_PARAM,
        RANGESWITCH_PARAM,
        TIMEFINE_PARAM,
        WIDTH_PARAM,
        LOOPONE_PARAM,
        LOOPTWO_PARAM,
        LOOPTHREE_PARAM,
        LOOPFOUR_PARAM,
        PARAMS_LEN
    };
    enum InputId {
        TIMECVIN_INPUT,
        WIDTHCVIN_INPUT,
        INPUTS_LEN
    };
    enum OutputId {
        ONEOUT_OUTPUT,
        TWOOUT_OUTPUT,
        THREEOUT_OUTPUT,
        FOUROUT_OUTPUT,
        CLOCKOUT_OUTPUT,
        VCOOUT_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId {
        ONELED_LIGHT,
        TWOLED_LIGHT,
        THREELED_LIGHT,
        FOURLED_LIGHT,
        CLOCKLED_LIGHT,
        VCOLED_LIGHT,
        LIGHTS_LEN
    };

    Swarm() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(TIMECOARSE_PARAM, 0.f, 1.f, 0.5f, "Coarse Speed", "%", 0.f, 100.f);
        configSwitch(RANGESWITCH_PARAM, 0.f, 1.f, 0.f, "Range", {"Slow", "Fast"});
        configParam(TIMEFINE_PARAM, -20.f, 20.f, 0.f, "Fine Speed Adjust", "ms");
        configParam(WIDTH_PARAM, 0.f, 1.f, 0.5f, "Width", "%", 10.f, 5.f);
        configSwitch(LOOPONE_PARAM, 0.f, 1.f, 0.f, "Loop Ch. 1", {"Off", "On"});
        configSwitch(LOOPTWO_PARAM, 0.f, 1.f, 0.f, "Loop Ch. 2", {"Off", "On"});
        configSwitch(LOOPTHREE_PARAM, 0.f, 1.f, 0.f, "Loop Ch. 3", {"Off", "On"});
        configSwitch(LOOPFOUR_PARAM, 0.f, 1.f, 0.f, "Loop Ch. 4", {"Off", "On"});
        configInput(TIMECVIN_INPUT, "Time CV");
        configInput(WIDTHCVIN_INPUT, "Width CV");
        configOutput(VCOOUT_OUTPUT, "VCO");
        configOutput(ONEOUT_OUTPUT, "Ch. 1");
        configOutput(TWOOUT_OUTPUT, "Ch. 2");
        configOutput(CLOCKOUT_OUTPUT, "Clock");
        configOutput(THREEOUT_OUTPUT, "Ch. 3");
        configOutput(FOUROUT_OUTPUT, "Ch. 4");
    }

    
    float clockTimer = 0.0f;
    float pulseWidth = 0.5f;
    bool mainOutputHigh = false;

    Chirp chirps[4];
    bool chirpsInitialized = false;

    void process(const ProcessArgs& args) override {
        // === Parameter Reading and Normalization ===
        float pulseWidthParam = params[WIDTH_PARAM].getValue();
        float coarseKnob = params[TIMECOARSE_PARAM].getValue();
        float fineAdjust = params[TIMEFINE_PARAM].getValue();
        float rangeSwitch = params[RANGESWITCH_PARAM].getValue();

        float timeCV = inputs[TIMECVIN_INPUT].getVoltage();
        float widthCV = inputs[WIDTHCVIN_INPUT].getVoltage();

        float timeCVNorm = clamp(timeCV, -5.f, 5.f);
        float widthCVNorm = clamp(widthCV, -5.f, 5.f);

        float summedTime = coarseKnob * 10.0f + timeCVNorm;
        summedTime = clamp(summedTime, -5.f, 5.f);

        float periodMs;
        if (rangeSwitch < 0.5f) {
            periodMs = 500.0f + (summedTime * (150.0f - 500.0f)); // Slow range
            periodMs = clamp(periodMs, 150.0f, 500.0f);
        } else {
            periodMs = 150.0f - (summedTime * (150.0f - 15.0f)); // Fast range
            periodMs = clamp(periodMs, 15.0f, 150.0f);
        }

        periodMs = clamp(periodMs - fineAdjust, 15.0f, 500.0f);

        float samplesPerMs = args.sampleRate * 0.001f;
        float periodSamples = periodMs * samplesPerMs;

        pulseWidth = clamp(pulseWidthParam + widthCVNorm * 0.1f, 0.05f, 0.5f);
        float pulseSamples = periodSamples * pulseWidth;

        // === Main Clock Logic ===
        clockTimer += args.sampleTime * args.sampleRate;
        if (clockTimer >= periodSamples) {
            clockTimer -= periodSamples;
        }

        mainOutputHigh = (clockTimer < pulseSamples);
        outputs[CLOCKOUT_OUTPUT].setVoltage(mainOutputHigh ? 5.f : 0.f);
        lights[CLOCKLED_LIGHT].setBrightnessSmooth(mainOutputHigh, args.sampleTime);

        // === Chirp Integration ===
        if (!chirpsInitialized) {
            chirps[0].timeRatio = 1.3f;
            chirps[1].timeRatio = 3.7f;
            chirps[2].timeRatio = 5.5f;
            chirps[3].timeRatio = 7.9f;
            chirpsInitialized = true;
        }

    for (int i = 0; i < 4; i++) {
    chirps[i].freeze = (params[LOOPONE_PARAM + i].getValue() > 0.5f) ? 1 : 0;

    int gate = chirps[i].update(periodMs, pulseWidth, args.sampleRate);
    outputs[ONEOUT_OUTPUT + i].setVoltage(gate ? 5.f : 0.f);
    lights[ONELED_LIGHT + i].setBrightnessSmooth(gate, args.sampleTime);
}
    }
};

struct SwarmWidget : ModuleWidget {
    SwarmWidget(Swarm* module) {
        setModule(module);
        setPanel(createPanel(
            asset::plugin(pluginInstance, "res/Swarm.svg"),
            asset::plugin(pluginInstance, "res/Swarm-dark.svg")
        ));
        
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<RoundBigBlackKnob>(mm2px(Vec(20.616, 16.629)), module, Swarm::TIMECOARSE_PARAM));
        addParam(createParamCentered<CKSS>(mm2px(Vec(35.099, 27.847)), module, Swarm::RANGESWITCH_PARAM));
        addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(20.616, 35.046)), module, Swarm::TIMEFINE_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.616, 53.459)), module, Swarm::WIDTH_PARAM));
        addParam(createParamCentered<CKSS>(mm2px(Vec(5.702, 72.999)), module, Swarm::LOOPONE_PARAM));
        addParam(createParamCentered<CKSS>(mm2px(Vec(15.327, 72.999)), module, Swarm::LOOPTWO_PARAM));
        addParam(createParamCentered<CKSS>(mm2px(Vec(25.351, 72.999)), module, Swarm::LOOPTHREE_PARAM));
        addParam(createParamCentered<CKSS>(mm2px(Vec(35.099, 72.999)), module, Swarm::LOOPFOUR_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.702, 27.246)), module, Swarm::TIMECVIN_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.702, 53.463)), module, Swarm::WIDTHCVIN_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.702, 92.263)), module, Swarm::VCOOUT_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.616, 92.263)), module, Swarm::ONEOUT_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.378, 92.263)), module, Swarm::TWOOUT_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.702, 109.747)), module, Swarm::CLOCKOUT_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.616, 109.747)), module, Swarm::THREEOUT_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.378, 109.747)), module, Swarm::FOUROUT_OUTPUT));

        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(5.702, 84.018)), module, Swarm::VCOLED_LIGHT));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.602, 84.018)), module, Swarm::ONELED_LIGHT));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(35.378, 84.018)), module, Swarm::TWOLED_LIGHT));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(5.702, 102.163)), module, Swarm::CLOCKLED_LIGHT));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.602, 102.163)), module, Swarm::THREELED_LIGHT));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(35.378, 102.163)), module, Swarm::FOURLED_LIGHT));
    }
};

Model* modelSwarm = createModel<Swarm, SwarmWidget>("Swarm");
