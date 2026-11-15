#include "plugin.hpp"
#include "TehomExpander.hpp"

// DIR. OUT (slug "TehomOut"): 4hp direct-outputs expander for Tehom, stereo per deck.
// Links to a Tehom anywhere in the patch (nearest by default, or chosen from the
// context menu); it doesn't need to touch Tehom. Each deck's PRE-POST switch picks
// whether its outputs carry the deck before the XY mixer (pre) or the deck's
// contribution to the main mix (post).
struct TehomOut : TehomLinkedModule {
	enum ParamId {
		PREPOST1_PARAM,
		PREPOST2_PARAM,
		PREPOST3_PARAM,
		PREPOST4_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		INPUTS_LEN
	};
	enum OutputId {
		DECK1L_OUTPUT,
		DECK1R_OUTPUT,
		DECK2L_OUTPUT,
		DECK2R_OUTPUT,
		DECK3L_OUTPUT,
		DECK3R_OUTPUT,
		DECK4L_OUTPUT,
		DECK4R_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	TehomOut() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		for (int i = 0; i < 4; i++) {
			std::string deck = "Deck " + std::to_string(i + 1);
			configSwitch(PREPOST1_PARAM + i, 0.f, 1.f, 0.f, deck + " output", {"Pre-mixer", "Post-mixer"});
			configOutput(DECK1L_OUTPUT + 2 * i, deck + " left");
			configOutput(DECK1R_OUTPUT + 2 * i, deck + " right");
		}
	}

	void process(const ProcessArgs& args) override {
		TehomBus* bus = getBus();
		const TehomOutMessage* msg = bus ? bus->readOut(args.frame) : nullptr;

		for (int i = 0; i < 4; i++) {
			float l = 0.f, r = 0.f;
			if (msg) {
				bool post = params[PREPOST1_PARAM + i].getValue() > 0.5f;
				l = post ? msg->postL[i] : msg->preL[i];
				r = post ? msg->postR[i] : msg->preR[i];
			}
			outputs[DECK1L_OUTPUT + 2 * i].setVoltage(l);
			outputs[DECK1R_OUTPUT + 2 * i].setVoltage(r);
		}
	}
};

struct TehomOutWidget : TehomLinkedWidget {
	TehomOutWidget(TehomOut* module) {
		setModule(module);
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/panels/TehomOut.svg"),
			asset::plugin(pluginInstance, "res/panels/TehomOut-dark.svg")
		));

		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// Positions match the "components" layer of res/panels/TehomOut.svg
		const float deckY[4] = {0.f, 27.6f, 55.2f, 82.8f};
		for (int i = 0; i < 4; i++) {
			float dy = deckY[i];
			addParam(createParamCentered<CKSSHorizontal>(mm2px(Vec(12.7f, 18.6f + dy)), module, TehomOut::PREPOST1_PARAM + i));
			addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(5.1f, 31.4f + dy)), module, TehomOut::DECK1L_OUTPUT + 2 * i));
			addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(14.3f, 31.4f + dy)), module, TehomOut::DECK1R_OUTPUT + 2 * i));
		}
	}
};

Model* modelTehomOut = createModel<TehomOut, TehomOutWidget>("TehomOut");
