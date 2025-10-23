#include "plugin.hpp"


struct DitDit : Module {
	enum ParamId {
		SWITCH_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		BANG_INPUT,
		RESET_INPUT,
		STARTSTOP_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		DIT_OUTPUT,
		DAH_OUTPUT,
		MORSE_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		DITLED_LIGHT,
		DAHLED_LIGHT,
		MORSELED_LIGHT,
		LIGHTS_LEN
	};

	DitDit() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(SWITCH_PARAM, 0.f, 1.f, 0.f, "Start/Stop", {"Stop", "Start"});
		configInput(BANG_INPUT, "Bang!");
		configInput(RESET_INPUT, "Reset");
		configInput(STARTSTOP_INPUT, "Start/Stop CV");
		configOutput(DIT_OUTPUT, "Dit");
		configOutput(DAH_OUTPUT, "Dah");
		configOutput(MORSE_OUTPUT, "Morse");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct DitDitWidget : ModuleWidget {
	DitDitWidget(DitDit* module) {
		setModule(module);
setPanel(createPanel(
		asset::plugin(pluginInstance, "res/panels/DitDit.svg"),
		asset::plugin(pluginInstance, "res/panels/DitDit-dark.svg")
		));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<CKSS>(mm2px(Vec(20.32, 51.353)), module, DitDit::SWITCH_PARAM));

		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(8.611, 79.136)), module, DitDit::BANG_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(20.32, 79.136)), module, DitDit::RESET_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(32.029, 79.136)), module, DitDit::STARTSTOP_INPUT));

		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(8.611, 100.963)), module, DitDit::DIT_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(20.32, 100.963)), module, DitDit::DAH_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(32.029, 100.963)), module, DitDit::MORSE_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(8.611, 91.04)), module, DitDit::DITLED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.32, 91.04)), module, DitDit::DAHLED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(32.029, 91.04)), module, DitDit::MORSELED_LIGHT));
	}
};


Model* modelDitDit = createModel<DitDit, DitDitWidget>("DitDit");