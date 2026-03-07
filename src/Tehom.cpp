#include "plugin.hpp"

struct Tehom : Module {
    enum ParamId {
        WARBLE_PARAM,
        SELECT_PARAM,
        AMOUNT_PARAM,
        SLEW_PARAM,
        SIZE_PARAM,
        POSITION_PARAM,
        XFADE_PARAM,

        // Source
        SOURCE1_PARAM,
        SOURCE2_PARAM,
        SOURCE3_PARAM,
        SOURCE4_PARAM,

        // Pitch
        PITCH1_PARAM,
        PITCH2_PARAM,
        PITCH3_PARAM,
        PITCH4_PARAM,

        // Record
        RECORD1_PARAM,
        RECORD2_PARAM,
        RECORD3_PARAM,
        RECORD4_PARAM,

        // Loop
        LOOP1_PARAM,
        LOOP2_PARAM,
        LOOP3_PARAM,
        LOOP4_PARAM,

        // Play
        PLAY1_PARAM,
        PLAY2_PARAM,
        PLAY3_PARAM,
        PLAY4_PARAM,

		LEDBEZEL1_PARAM,
		LEDBEZEL2_PARAM,
		LEDBEZEL3_PARAM,
		LEDBEZEL4_PARAM,

        PARAMS_LEN
    };

    enum InputId {
        AUDIOLEFTIN_INPUT,
        AUDIORIGHTIN_INPUT,
        WARBLECVIN_INPUT,
        SELECTCVIN_INPUT,
        AMOUNTCVIN_INPUT,
        RETURN_INPUT,
        XCVIN_INPUT,
        YCVIN_INPUT,
        SLEWCVIN_INPUT,
        SIZECVIN_INPUT,
        POSITIONCVIN_INPUT,
        XFADECVIN_INPUT,

        // Source CV
        SOURCE1CVIN_INPUT,
        SOURCE2CVIN_INPUT,
        SOURCE3CVIN_INPUT,
        SOURCE4CVIN_INPUT,

        // Pitch CV
        PITCH1CVIN_INPUT,
        PITCH2CVIN_INPUT,
        PITCH3CVIN_INPUT,
        PITCH4CVIN_INPUT,

        // Record CV
        RECORD1CVIN_INPUT,
        RECORD2CVIN_INPUT,
        RECORD3CVIN_INPUT,
        RECORD4CVIN_INPUT,

        // Play CV
        PLAY1CVIN_INPUT,
        PLAY2CVIN_INPUT,
        PLAY3CVIN_INPUT,
        PLAY4CVIN_INPUT,

        INPUTS_LEN
    };

    enum OutputId {
        AUDIOLEFTOUT_OUTPUT,
        AUDIORIGHTOUT_OUTPUT,
        SEND_OUTPUT,
        XCVOUT_OUTPUT,
        YCVOUT_OUTPUT,
        OUTPUTS_LEN
    };

  enum LightId {
    BUFFER1LED_LIGHT,
    BUFFER2LED_LIGHT,
    BUFFER3LED_LIGHT,
    BUFFER4LED_LIGHT,
    
    // RECORD lights (red)
    RECORD1_LIGHT,
    RECORD2_LIGHT,
    RECORD3_LIGHT,
    RECORD4_LIGHT,
    
    // PLAY lights (green)
    PLAY1_LIGHT,
    PLAY2_LIGHT,
    PLAY3_LIGHT,
    PLAY4_LIGHT,

    // PLAY lights (blue - reverse mode)
    PLAY1_BLUE_LIGHT,
    PLAY2_BLUE_LIGHT,
    PLAY3_BLUE_LIGHT,
    PLAY4_BLUE_LIGHT,

    LIGHTS_LEN
};

	Tehom() {
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

    configSwitch(SELECT_PARAM, 0.f, 8.f, 0.f, "Select", {"Vinyl Crackle Clean", "Vinyl Crackle Dirty", "HiFi Tape Hiss", "LoFi Tape Hiss", "60hz Hum", "50hz Hum", "Cafe Ambience", "City Ambience", "Forest Ambience"});

	configParam(SLEW_PARAM, 0.f, 1.f, 0.f, "Slew", "ms", 0.f, 1000.f);

	configParam(LEDBEZEL1_PARAM, 0.f, 1.f, 0.f);
	configParam(LEDBEZEL2_PARAM, 0.f, 1.f, 0.f);
	configParam(LEDBEZEL3_PARAM, 0.f, 1.f, 0.f);
	configParam(LEDBEZEL4_PARAM, 0.f, 1.f, 0.f);

	for (int i = 0; i < 4; i++) bezelDragging[i].store(false);

	configParam(WARBLE_PARAM, 0.f, 1.f, 0.f, "Warble", "%", 0.f, 100.f);
	configParam(AMOUNT_PARAM, 0.f, 1.f, 0.f, "Noise Amount", "%", 0.f, 100.f);
    configParam(SIZE_PARAM, 0.f, 1.f, 0.f, "Size", "%", 0.f, 100.f);
    configParam(POSITION_PARAM, 0.f, 1.f, 0.f, "Position", "%", 0.f, 100.f);
    configParam(XFADE_PARAM, 0.f, 1.f, 0.f, "Crossfade", "%", 0.f, 100.f);
	
	// Source params
	configParam(SOURCE1_PARAM, -1.f, 1.f, 0.f, "Source", "%", 0.f, 100.f);
	configParam(SOURCE2_PARAM, -1.f, 1.f, 0.f, "Source", "%", 0.f, 100.f);
	configParam(SOURCE3_PARAM, -1.f, 1.f, 0.f, "Source", "%", 0.f, 100.f);
	configParam(SOURCE4_PARAM, -1.f, 1.f, 0.f, "Source", "%", 0.f, 100.f);

	// Pitch params
	configParam(PITCH1_PARAM, 0.f, 1.f, 0.5f, "Pitch", "%", 0.f, 100.f);
	configParam(PITCH2_PARAM, 0.f, 1.f, 0.5f, "Pitch", "%", 0.f, 100.f);
	configParam(PITCH3_PARAM, 0.f, 1.f, 0.5f, "Pitch", "%", 0.f, 100.f);
	configParam(PITCH4_PARAM, 0.f, 1.f, 0.5f, "Pitch", "%", 0.f, 100.f);

	// Record switches
	configSwitch(RECORD1_PARAM, 0.f, 1.f, 0.f, "Record");
	configSwitch(RECORD2_PARAM, 0.f, 1.f, 0.f, "Record");
	configSwitch(RECORD3_PARAM, 0.f, 1.f, 0.f, "Record");
	configSwitch(RECORD4_PARAM, 0.f, 1.f, 0.f, "Record");

	// Loop switches
	configSwitch(LOOP1_PARAM, 0.f, 1.f, 0.f, "Looping", {"Off", "On"});
	configSwitch(LOOP2_PARAM, 0.f, 1.f, 0.f, "Looping", {"Off", "On"});
	configSwitch(LOOP3_PARAM, 0.f, 1.f, 0.f, "Looping", {"Off", "On"});
	configSwitch(LOOP4_PARAM, 0.f, 1.f, 0.f, "Looping", {"Off", "On"});

	// Play switches
	configSwitch(PLAY1_PARAM, 0.f, 1.f, 0.f, "Play");
	configSwitch(PLAY2_PARAM, 0.f, 1.f, 0.f, "Play");
	configSwitch(PLAY3_PARAM, 0.f, 1.f, 0.f, "Play");
	configSwitch(PLAY4_PARAM, 0.f, 1.f, 0.f, "Play");

    // Audio inputs
    configInput(AUDIOLEFTIN_INPUT, "Audio Left");
    configInput(AUDIORIGHTIN_INPUT, "Audio Right");

    // Global CV inputs
    configInput(WARBLECVIN_INPUT, "Warble CV");
    configInput(SELECTCVIN_INPUT, "Noise Select CV");
    configInput(AMOUNTCVIN_INPUT, "Noise Amount CV");
    configInput(RETURN_INPUT, "Noise Return");
    configInput(XCVIN_INPUT, "X CV");
    configInput(YCVIN_INPUT, "Y CV");
    configInput(SLEWCVIN_INPUT, "Slew CV");
    configInput(SIZECVIN_INPUT, "Size CV");
    configInput(POSITIONCVIN_INPUT, "Position CV");
    configInput(XFADECVIN_INPUT, "Crossfade CV");
	
	// Source CV inputs
	configInput(SOURCE1CVIN_INPUT, "Source CV");
	configInput(SOURCE2CVIN_INPUT, "Source CV");
	configInput(SOURCE3CVIN_INPUT, "Source CV");
	configInput(SOURCE4CVIN_INPUT, "Source CV");

	// Pitch CV inputs
	configInput(PITCH1CVIN_INPUT, "Pitch CV");
	configInput(PITCH2CVIN_INPUT, "Pitch CV");
	configInput(PITCH3CVIN_INPUT, "Pitch CV");
	configInput(PITCH4CVIN_INPUT, "Pitch CV");

	// Record CV inputs
	configInput(RECORD1CVIN_INPUT, "Record CV");
	configInput(RECORD2CVIN_INPUT, "Record CV");
	configInput(RECORD3CVIN_INPUT, "Record CV");
	configInput(RECORD4CVIN_INPUT, "Record CV");

	// Play CV inputs
	configInput(PLAY1CVIN_INPUT, "Play CV");
	configInput(PLAY2CVIN_INPUT, "Play CV");
	configInput(PLAY3CVIN_INPUT, "Play CV");
	configInput(PLAY4CVIN_INPUT, "Play CV");

    // Outputs
    configOutput(AUDIOLEFTOUT_OUTPUT, "Audio Left");
    configOutput(AUDIORIGHTOUT_OUTPUT, "Audio Right");
    configOutput(SEND_OUTPUT, "Noise Send");
    configOutput(XCVOUT_OUTPUT, "X CV");
    configOutput(YCVOUT_OUTPUT, "Y CV");
}

// Current state of record/play toggles
bool recordState[4] = {false, false, false, false};
bool playState[4] = {false, false, false, false};

// Previous button value for rising-edge detection
bool lastRecordButton[4] = {false, false, false, false};
bool lastPlayButton[4] = {false, false, false, false};

// Previous CV state for rising-edge detection
bool lastRecordCV[4] = {false, false, false, false};
bool lastPlayCV[4] = {false, false, false, false};

float ledSpinSpeed[4] = {0.3f, 0.25f, 0.2f, 0.15f};

std::atomic<bool> bezelDragging[4];
bool playReversed[4] = {false, false, false, false};

void process(const ProcessArgs& args) override {
    // --- RECORD toggles ---
    for (int i = 0; i < 4; i++) {
        bool btn = params[RECORD1_PARAM + i].getValue() > 0.5f;
        bool btnRising = btn && !lastRecordButton[i];
        lastRecordButton[i] = btn;

        bool cv = inputs[RECORD1CVIN_INPUT + i].getVoltage() > 1.f;
        bool cvRising = cv && !lastRecordCV[i];
        lastRecordCV[i] = cv;

        if (btnRising || cvRising)
            recordState[i] = !recordState[i];

        // Update light smoothly
        lights[RECORD1_LIGHT + i].setBrightnessSmooth(recordState[i] ? 1.f : 0.f, args.sampleTime);
    }

    // --- PLAY toggles ---
    for (int i = 0; i < 4; i++) {
        bool btn = params[PLAY1_PARAM + i].getValue() > 0.5f;
        bool btnRising = btn && !lastPlayButton[i];
        lastPlayButton[i] = btn;

        bool cv = inputs[PLAY1CVIN_INPUT + i].getVoltage() > 1.f;
        bool cvRising = cv && !lastPlayCV[i];
        lastPlayCV[i] = cv;

        if (btnRising || cvRising)
            playState[i] = !playState[i];

        // Green when playing forward, blue when playing reversed
        lights[PLAY1_LIGHT + i].setBrightnessSmooth((playState[i] && !playReversed[i]) ? 1.f : 0.f, args.sampleTime);
        lights[PLAY1_BLUE_LIGHT + i].setBrightnessSmooth((playState[i] && playReversed[i]) ? 1.f : 0.f, args.sampleTime);

// --- BEZEL SPINNING ---
for (int i = 0; i < 4; i++) {
    // Only spin if PLAY is active and user isn't clicking the bezel
    if (playState[i] && !bezelDragging[i].load()) {
        float pitchVal = params[Tehom::PITCH1_PARAM + i].getValue();

        float minSpeed = 0.02f;
        float maxSpeed = 0.2f;

        float spinSpeed = minSpeed + pitchVal * (maxSpeed - minSpeed);
        float spinDir = playReversed[i] ? -1.f : 1.f;

        float newValue = params[Tehom::LEDBEZEL1_PARAM + i].getValue() + spinDir * spinSpeed * args.sampleTime;

        if (newValue > 1.f) newValue -= 1.f;
        if (newValue < 0.f) newValue += 1.f;

        params[Tehom::LEDBEZEL1_PARAM + i].setValue(newValue);
    }
}

}
}
};


struct ReverseMenuItem : ui::MenuItem {
	Tehom* tehom;
	int channel;
	void onAction(const ActionEvent& e) override {
		tehom->playReversed[channel] = !tehom->playReversed[channel];
	}
	void step() override {
		rightText = CHECKMARK(tehom->playReversed[channel]);
		ui::MenuItem::step();
	}
};

struct SpinningBezelWidget : LEDBezelSilver {
	Tehom* tehom = nullptr;
	int channel = 0;
	ui::Tooltip* tooltip = nullptr;
	bool isHovered = false;

	void onDragStart(const DragStartEvent& e) override {
		if (tehom) tehom->bezelDragging[channel].store(true);
		// don't call base — suppress knob drag behaviour
	}

	void onDragMove(const DragMoveEvent& e) override {
		// suppress value change from dragging
	}

	void onDragEnd(const DragEndEvent& e) override {
		if (tehom) tehom->bezelDragging[channel].store(false);
		// don't call base — suppress knob drag behaviour
	}

	void onEnter(const EnterEvent& e) override {
		isHovered = true;
		// suppress default tooltip
	}

	void onLeave(const LeaveEvent& e) override {
		isHovered = false;
		if (tooltip) {
			APP->scene->removeChild(tooltip);
			delete tooltip;
			tooltip = nullptr;
		}
	}

	void step() override {
		LEDBezelSilver::step();
		bool shouldShow = isHovered && tehom && tehom->bezelDragging[channel].load();
		if (shouldShow && !tooltip) {
			tooltip = new ui::Tooltip;
			tooltip->text = "Paused";
			APP->scene->addChild(tooltip);
		} else if (!shouldShow && tooltip) {
			APP->scene->removeChild(tooltip);
			delete tooltip;
			tooltip = nullptr;
		}
	}

	void appendContextMenu(Menu* menu) override {
		LEDBezelSilver::appendContextMenu(menu);
		menu->addChild(new ui::MenuSeparator);
		auto* item = new ReverseMenuItem;
		item->text = "Reverse";
		item->tehom = tehom;
		item->channel = channel;
		menu->addChild(item);
	}
};

struct RecordWidget : LEDBezel {
	Tehom* tehom = nullptr;
	int channel = 0;
	ui::Tooltip* tooltip = nullptr;
	bool isHovered = false;

	void onButton(const ButtonEvent& e) override {
		if (e.button == GLFW_MOUSE_BUTTON_RIGHT) {
			e.consume(this);
			return;
		}
		LEDBezel::onButton(e);
	}

	void onEnter(const EnterEvent& e) override {
		isHovered = true;
	}

	void onLeave(const LeaveEvent& e) override {
		isHovered = false;
		if (tooltip) {
			APP->scene->removeChild(tooltip);
			delete tooltip;
			tooltip = nullptr;
		}
	}

	void step() override {
		LEDBezel::step();
		bool shouldShow = isHovered && tehom && tehom->recordState[channel];
		if (shouldShow && !tooltip) {
			tooltip = new ui::Tooltip;
			tooltip->text = "Recording ...";
			APP->scene->addChild(tooltip);
		} else if (!shouldShow && tooltip) {
			APP->scene->removeChild(tooltip);
			delete tooltip;
			tooltip = nullptr;
		}
	}
};

struct PlayTooltip : ui::Tooltip {
	Tehom* tehom;
	int channel;
	void step() override {
		if (tehom) {
			if (!tehom->playState[channel])
				text = "Stop";
			else if (tehom->playReversed[channel])
				text = "Reverse";
			else
				text = "Forward";
		}
		ui::Tooltip::step();
	}
};

struct ReversePlayWidget : LEDBezel {
	Tehom* tehom = nullptr;
	int channel = 0;
	ui::Tooltip* tooltip = nullptr;

	void onButton(const ButtonEvent& e) override {
		if (e.button == GLFW_MOUSE_BUTTON_RIGHT && e.action == GLFW_PRESS) {
			if (tehom) tehom->playReversed[channel] = !tehom->playReversed[channel];
			e.consume(this);
			return;
		}
		LEDBezel::onButton(e);
	}

	void onEnter(const EnterEvent& e) override {
		auto* tip = new PlayTooltip;
		tip->tehom = tehom;
		tip->channel = channel;
		APP->scene->addChild(tip);
		tooltip = tip;
	}

	void onLeave(const LeaveEvent& e) override {
		if (tooltip) {
			APP->scene->removeChild(tooltip);
			delete tooltip;
			tooltip = nullptr;
		}
	}
};

struct TehomWidget : ModuleWidget {
	TehomWidget(Tehom* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/panels/Tehom.svg")));

		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// 2 Position switches 
		addParam(createParamCentered<CKSS>(mm2px(Vec(26.901, 54.204)), module, Tehom::LOOP1_PARAM));
		addParam(createParamCentered<CKSS>(mm2px(Vec(125.316, 54.204)), module, Tehom::LOOP2_PARAM));
		addParam(createParamCentered<CKSS>(mm2px(Vec(26.901, 115.975)), module, Tehom::LOOP3_PARAM));
		addParam(createParamCentered<CKSS>(mm2px(Vec(125.316, 115.975)), module, Tehom::LOOP4_PARAM));

		// LEDBezel buttons with lights (RECORD = red, PLAY = green)
		{
			Vec recPos[4] = {
				mm2px(Vec(14.738, 54.176)),
				mm2px(Vec(113.152, 54.176)),
				mm2px(Vec(14.738, 115.947)),
				mm2px(Vec(113.152, 115.947)),
			};
			for (int i = 0; i < 4; i++) {
				auto* rec = createParamCentered<RecordWidget>(recPos[i], module, Tehom::RECORD1_PARAM + i);
				rec->tehom = module;
				rec->channel = i;
				addParam(rec);
				addChild(createLightCentered<LEDBezelLight<RedLight>>(recPos[i], module, Tehom::RECORD1_LIGHT + i));
			}
		}

		{
			Vec playPos[4] = {
				mm2px(Vec(39.026, 54.176)),
				mm2px(Vec(137.441, 54.176)),
				mm2px(Vec(39.026, 115.947)),
				mm2px(Vec(137.441, 115.947)),
			};
			for (int i = 0; i < 4; i++) {
				auto* play = createParamCentered<ReversePlayWidget>(playPos[i], module, Tehom::PLAY1_PARAM + i);
				play->tehom = module;
				play->channel = i;
				addParam(play);
				addChild(createLightCentered<LEDBezelLight<GreenLight>>(playPos[i], module, Tehom::PLAY1_LIGHT + i));
				addChild(createLightCentered<LEDBezelLight<BlueLight>>(playPos[i], module, Tehom::PLAY1_BLUE_LIGHT + i));
			}
		}

		// Small knobs 
		addParam(createParamCentered<Trimpot>(mm2px(Vec(60.972, 36.448)), module, Tehom::WARBLE_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(71.215, 36.448)), module, Tehom::SELECT_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(81.457, 36.448)), module, Tehom::AMOUNT_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(60.972, 114.136)), module, Tehom::SLEW_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(71.215, 114.136)), module, Tehom::SIZE_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(81.457, 114.136)), module, Tehom::POSITION_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(91.8, 114.136)), module, Tehom::XFADE_PARAM));

		// Big Knobs
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.738, 39.243)), module, Tehom::SOURCE1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(113.152, 39.243)), module, Tehom::SOURCE2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.738, 101.014)), module, Tehom::SOURCE3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(113.152, 101.014)), module, Tehom::SOURCE4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(38.86, 39.243)), module, Tehom::PITCH1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(137.275, 39.243)), module, Tehom::PITCH2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(38.86, 101.014)), module, Tehom::PITCH3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(137.275, 101.014)), module, Tehom::PITCH4_PARAM));
	
		// Audio inputs
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(60.972, 11.851)), module, Tehom::AUDIOLEFTIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(71.215, 11.851)), module, Tehom::AUDIORIGHTIN_INPUT));

		// Inputs 
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(60.972, 26.591)), module, Tehom::WARBLECVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(71.215, 26.591)), module, Tehom::SELECTCVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(81.457, 26.591)), module, Tehom::AMOUNTCVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(91.627, 36.448)), module, Tehom::RETURN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(60.968, 90.436)), module, Tehom::XCVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(71.215, 90.436)), module, Tehom::YCVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(60.972, 106.395)), module, Tehom::SLEWCVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(71.215, 106.395)), module, Tehom::SIZECVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(81.457, 106.395)), module, Tehom::POSITIONCVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(91.8, 106.395)), module, Tehom::XFADECVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(9.434, 23.541)), module, Tehom::SOURCE1CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(44.33, 23.541)), module, Tehom::PITCH1CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(20.788, 23.541)), module, Tehom::RECORD1CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(32.932, 23.541)), module, Tehom::PLAY1CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(107.848, 23.541)), module, Tehom::SOURCE2CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(142.745, 23.541)), module, Tehom::PITCH2CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(119.202, 23.541)), module, Tehom::RECORD2CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(131.347, 23.541)), module, Tehom::PLAY2CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(9.434, 85.312)), module, Tehom::SOURCE3CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(44.33, 85.312)), module, Tehom::PITCH3CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(20.788, 85.312)), module, Tehom::RECORD3CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(32.932, 85.312)), module, Tehom::PLAY3CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(107.848, 85.312)), module, Tehom::SOURCE4CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(142.745, 85.312)), module, Tehom::PITCH4CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(119.202, 85.312)), module, Tehom::RECORD4CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(131.347, 85.312)), module, Tehom::PLAY4CVIN_INPUT));

		// Outputs
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(81.457, 11.912)), module, Tehom::AUDIOLEFTOUT_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(91.8, 11.912)), module, Tehom::AUDIORIGHTOUT_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(91.627, 26.591)), module, Tehom::SEND_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(81.41, 90.353)), module, Tehom::XCVOUT_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(91.8, 90.669)), module, Tehom::YCVOUT_OUTPUT));

		// LED Bezels
		for (int i = 0; i < 4; i++) {
			Vec positions[4] = {
				mm2px(Vec(26.901, 10.166)),
				mm2px(Vec(125.316, 10.166)),
				mm2px(Vec(26.901, 71.937)),
				mm2px(Vec(125.316, 71.937)),
			};
			auto* bezel = createParamCentered<SpinningBezelWidget>(positions[i], module, Tehom::LEDBEZEL1_PARAM + i);
			bezel->tehom = module;
			bezel->channel = i;
			addParam(bezel);
		}

		// Lights
		addChild(createLightCentered<LargeLight<RedLight>>(mm2px(Vec(26.9, 10.166)), module, Tehom::BUFFER1LED_LIGHT));
		addChild(createLightCentered<LargeLight<RedLight>>(mm2px(Vec(125.315, 10.166)), module, Tehom::BUFFER2LED_LIGHT));
		addChild(createLightCentered<LargeLight<RedLight>>(mm2px(Vec(26.9, 71.937)), module, Tehom::BUFFER3LED_LIGHT));
		addChild(createLightCentered<LargeLight<RedLight>>(mm2px(Vec(125.315, 71.937)), module, Tehom::BUFFER4LED_LIGHT));
	}
};
Model* modelTehom = createModel<Tehom, TehomWidget>("Tehom");