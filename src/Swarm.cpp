#include "plugin.hpp"

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
        VCOOUT_OUTPUT,
        ONEOUT_OUTPUT,
        TWOOUT_OUTPUT,
        CLOCKOUT_OUTPUT,
        THREEOUT_OUTPUT,
        FOUROUT_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId {
        VCOLED_LIGHT,
        ONELED_LIGHT,
        TWOLED_LIGHT,
        CLOCKLED_LIGHT,
        THREELED_LIGHT,
        FOURLED_LIGHT,
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

    void process(const ProcessArgs& args) override {
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
