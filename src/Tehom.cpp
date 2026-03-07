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

        // Speed
        SPEED1_PARAM,
        SPEED2_PARAM,
        SPEED3_PARAM,
        SPEED4_PARAM,

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

		// LED Bezels
		LEDBEZEL1_PARAM,
		LEDBEZEL2_PARAM,
		LEDBEZEL3_PARAM,
		LEDBEZEL4_PARAM,

		// XY
		XPOS_PARAM,
        YPOS_PARAM,

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

        // Speed CV
        SPEED1CVIN_INPUT,
        SPEED2CVIN_INPUT,
        SPEED3CVIN_INPUT,
        SPEED4CVIN_INPUT,

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

    // Erase flash (white, overlaid on record buttons)
    RECORD1_FLASH_LIGHT,
    RECORD2_FLASH_LIGHT,
    RECORD3_FLASH_LIGHT,
    RECORD4_FLASH_LIGHT,

    LIGHTS_LEN
};

	Tehom() {
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

	configParam(XPOS_PARAM, 0.f, 1.f, 0.5f, "X Position");
    configParam(YPOS_PARAM, 0.f, 1.f, 0.5f, "Y Position");

    configSwitch(SELECT_PARAM, 0.f, 8.f, 0.f, "Select", {"Vinyl Crackle Clean", "Vinyl Crackle Dirty", "HiFi Tape Hiss", "LoFi Tape Hiss", "60hz Hum", "50hz Hum", "Cafe Ambience", "City Ambience", "Forest Ambience"});

	configParam(SLEW_PARAM, 0.02f, 1.f, 0.02f, "Slew", "ms", 0.f, 1000.f);

	configParam(LEDBEZEL1_PARAM, 0.f, 1.f, 0.f);
	configParam(LEDBEZEL2_PARAM, 0.f, 1.f, 0.f);
	configParam(LEDBEZEL3_PARAM, 0.f, 1.f, 0.f);
	configParam(LEDBEZEL4_PARAM, 0.f, 1.f, 0.f);

	for (int i = 0; i < 4; i++) bezelDragging[i].store(false);

	configParam(WARBLE_PARAM, 0.f, 1.f, 0.f, "Warble", "%", 0.f, 100.f);
	configParam(AMOUNT_PARAM, 0.f, 1.f, 0.f, "Noise Amount", "%", 0.f, 100.f);
    configParam(SIZE_PARAM, 0.f, 1.f, 1.f, "Loop Size", "%", 0.f, 100.f);
    configParam(POSITION_PARAM, 0.f, 1.f, 0.5f, "Loop Position", "%", 0.f, 100.f);
    configParam(XFADE_PARAM, 0.f, 1.f, 0.f, "Crossfade", "%", 0.f, 100.f);
	
	// Source params
	configParam(SOURCE1_PARAM, -1.f, 1.f, 0.f, "Source", "%", 0.f, 100.f);
	configParam(SOURCE2_PARAM, -1.f, 1.f, 0.f, "Source", "%", 0.f, 100.f);
	configParam(SOURCE3_PARAM, -1.f, 1.f, 0.f, "Source", "%", 0.f, 100.f);
	configParam(SOURCE4_PARAM, -1.f, 1.f, 0.f, "Source", "%", 0.f, 100.f);

	// Pitch params
	configParam(SPEED1_PARAM, 0.025f, 1.f, 0.5f, "Speed", "x", 0.f, 2.f);
	configParam(SPEED2_PARAM, 0.025f, 1.f, 0.5f, "Speed", "x", 0.f, 2.f);
	configParam(SPEED3_PARAM, 0.025f, 1.f, 0.5f, "Speed", "x", 0.f, 2.f);
	configParam(SPEED4_PARAM, 0.025f, 1.f, 0.5f, "Speed", "x", 0.f, 2.f);

	// Record switches
	configSwitch(RECORD1_PARAM, 0.f, 1.f, 0.f, "Record");
	configSwitch(RECORD2_PARAM, 0.f, 1.f, 0.f, "Record");
	configSwitch(RECORD3_PARAM, 0.f, 1.f, 0.f, "Record");
	configSwitch(RECORD4_PARAM, 0.f, 1.f, 0.f, "Record");

	// Loop switches
	configSwitch(LOOP1_PARAM, 0.f, 1.f, 1.f, "Looping", {"Off", "On"});
	configSwitch(LOOP2_PARAM, 0.f, 1.f, 1.f, "Looping", {"Off", "On"});
	configSwitch(LOOP3_PARAM, 0.f, 1.f, 1.f, "Looping", {"Off", "On"});
	configSwitch(LOOP4_PARAM, 0.f, 1.f, 1.f, "Looping", {"Off", "On"});

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
	configInput(SPEED1CVIN_INPUT, "Speed CV");
	configInput(SPEED2CVIN_INPUT, "Speed CV");
	configInput(SPEED3CVIN_INPUT, "Speed CV");
	configInput(SPEED4CVIN_INPUT, "Speed CV");

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

void eraseBuffer(int i) {
    if (i < 0 || i > 3) return;
    if (!bufL[i].empty()) std::fill(bufL[i].begin(), bufL[i].end(), 0.f);
    if (!bufR[i].empty()) std::fill(bufR[i].begin(), bufR[i].end(), 0.f);
    writePos[i] = 0;
    readPos[i] = 0.f;
    recordedLength[i] = 0;
    hasContent[i] = false;
    recordState[i] = false;
    playState[i] = false;
    eraseFlash[i] = 0.4f;
}

void resizeBuffers(float sampleRate) {
    currentSampleRate = sampleRate;
    bufferSize = (int)(bufferDuration * sampleRate);
    for (int i = 0; i < 4; i++) {
        bufL[i].assign(bufferSize, 0.f);
        bufR[i].assign(bufferSize, 0.f);
        writePos[i] = 0;
        readPos[i] = 0.f;
        recordedLength[i] = 0;
        hasContent[i] = false;
        recordState[i] = false;
        playState[i] = false;
    }
}

void onSampleRateChange(const SampleRateChangeEvent& e) override {
    resizeBuffers(e.sampleRate);
}

void onReset(const ResetEvent& e) override {
    Module::onReset(e);
    for (int i = 0; i < 4; i++)
        eraseBuffer(i);
    xyFinalX = 0.5f;
    xyFinalY = 0.5f;
}

json_t* dataToJson() override {
    json_t* root = json_object();
    json_object_set_new(root, "bufferDuration", json_real(bufferDuration));
    json_t* ap = json_array(), *apf = json_array(), *rt = json_array();
    for (int i = 0; i < 4; i++) {
        json_array_append_new(ap,  json_boolean(autoPlay[i]));
        json_array_append_new(apf, json_boolean(autoPlayFull[i]));
        json_array_append_new(rt,  json_boolean(retrigger[i]));
    }
    json_object_set_new(root, "autoPlay",     ap);
    json_object_set_new(root, "autoPlayFull", apf);
    json_object_set_new(root, "retrigger",    rt);
    return root;
}

void dataFromJson(json_t* root) override {
    json_t* bd = json_object_get(root, "bufferDuration");
    if (bd) { bufferDuration = (float)json_real_value(bd); if (currentSampleRate > 0.f) resizeBuffers(currentSampleRate); }
    json_t* ap = json_object_get(root, "autoPlay");
    if (ap)  for (int i = 0; i < 4; i++) { json_t* v = json_array_get(ap,  i); if (v) autoPlay[i]     = json_boolean_value(v); }
    json_t* apf = json_object_get(root, "autoPlayFull");
    if (apf) for (int i = 0; i < 4; i++) { json_t* v = json_array_get(apf, i); if (v) autoPlayFull[i]  = json_boolean_value(v); }
    json_t* rt = json_object_get(root, "retrigger");
    if (rt)  for (int i = 0; i < 4; i++) { json_t* v = json_array_get(rt,  i); if (v) retrigger[i]     = json_boolean_value(v); }
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

float eraseFlash[4] = {};
bool autoPlay[4]     = {true, true, true, true};
bool autoPlayFull[4] = {true, true, true, true};
bool retrigger[4]    = {false, false, false, false}; // CV retriggers playhead instead of toggling

std::atomic<bool> xyDragging{false};
float xyFinalX = 0.5f;
float xyFinalY = 0.5f;

// Audio buffers
float bufferDuration = 2.f;   // seconds, user-selectable
float currentSampleRate = 44100.f;
int bufferSize = 0;
std::vector<float> bufL[4];
std::vector<float> bufR[4];
int writePos[4] = {};
float readPos[4] = {};
int recordedLength[4] = {};
bool hasContent[4] = {};

void process(const ProcessArgs& args) override {
    // Ensure buffers are allocated (onSampleRateChange may not fire before first process)
    if (bufferSize == 0)
        resizeBuffers(args.sampleRate);

    // --- RECORD toggles ---
    for (int i = 0; i < 4; i++) {
        bool btn = params[RECORD1_PARAM + i].getValue() > 0.5f;
        bool btnRising = btn && !lastRecordButton[i];
        lastRecordButton[i] = btn;

        bool cv = inputs[RECORD1CVIN_INPUT + i].getVoltage() > 1.f;
        bool cvRising = cv && !lastRecordCV[i];
        lastRecordCV[i] = cv;

        if (btnRising || cvRising) {
            bool wasRecording = recordState[i];
            recordState[i] = !recordState[i];
            if (recordState[i] && !hasContent[i]) {
                writePos[i] = 0;
                recordedLength[i] = 0;
            }
            // Auto-play when recording manually stopped
            if (wasRecording && !recordState[i] && autoPlay[i] && hasContent[i] && !playState[i]) {
                playState[i] = true;
                readPos[i] = playReversed[i] ? (float)(std::max(1, recordedLength[i]) - 1) : 0.f;
            }
        }

        lights[RECORD1_LIGHT + i].setBrightnessSmooth(recordState[i] ? 1.f : 0.f, args.sampleTime);

        // Erase flash — white light fades out over 0.4s
        if (eraseFlash[i] > 0.f) {
            eraseFlash[i] -= args.sampleTime;
            lights[RECORD1_FLASH_LIGHT + i].setBrightness(clamp(eraseFlash[i] / 0.4f, 0.f, 1.f));
        } else {
            eraseFlash[i] = 0.f;
            lights[RECORD1_FLASH_LIGHT + i].setBrightness(0.f);
        }
    }

    // --- PLAY toggles ---
    for (int i = 0; i < 4; i++) {
        bool btn = params[PLAY1_PARAM + i].getValue() > 0.5f;
        bool btnRising = btn && !lastPlayButton[i];
        lastPlayButton[i] = btn;

        bool cv = inputs[PLAY1CVIN_INPUT + i].getVoltage() > 1.f;
        bool cvRising = cv && !lastPlayCV[i];
        lastPlayCV[i] = cv;

        if (cvRising && retrigger[i]) {
            // Retrigger: reset playhead to start (or end if reversed), ensure playing
            playState[i] = true;
            readPos[i] = playReversed[i] ? (float)(std::max(1, recordedLength[i]) - 1) : 0.f;
        } else if (btnRising || cvRising) {
            playState[i] = !playState[i];
            if (playState[i]) {
                readPos[i] = playReversed[i] ? (float)(std::max(1, recordedLength[i]) - 1) : 0.f;
            }
        }

        lights[PLAY1_LIGHT + i].setBrightnessSmooth((playState[i] && !playReversed[i]) ? 1.f : 0.f, args.sampleTime);
        lights[PLAY1_BLUE_LIGHT + i].setBrightnessSmooth((playState[i] && playReversed[i]) ? 1.f : 0.f, args.sampleTime);
    }

    // --- BEZEL SPINNING (pauses audio readPos when held) ---
    for (int i = 0; i < 4; i++) {
        if (playState[i] && !bezelDragging[i].load()) {
            float pitchVal = clamp(params[SPEED1_PARAM + i].getValue() + inputs[SPEED1CVIN_INPUT + i].getVoltage() / 10.f, 0.025f, 1.f);
            float spinSpeed = 0.02f + pitchVal * 0.18f;
            float spinDir = playReversed[i] ? -1.f : 1.f;
            float newValue = params[LEDBEZEL1_PARAM + i].getValue() + spinDir * spinSpeed * args.sampleTime;
            if (newValue > 1.f) newValue -= 1.f;
            if (newValue < 0.f) newValue += 1.f;
            params[LEDBEZEL1_PARAM + i].setValue(newValue);
        }
    }

    // === AUDIO DSP ===
    float inL = inputs[AUDIOLEFTIN_INPUT].getVoltage();
    float inR = inputs[AUDIORIGHTIN_INPUT].isConnected() ? inputs[AUDIORIGHTIN_INPUT].getVoltage() : inL;

    // XY mixer — 2x2 bilinear: each corner owns one channel at full volume
    // ch1=top-left, ch2=top-right, ch3=bottom-left, ch4=bottom-right
    // xyFinalX=0 → left, xyFinalX=1 → right; xyFinalY=1 → top, xyFinalY=0 → bottom
    float vol[4] = {
        (1.f - xyFinalX) * xyFinalY,         // ch1: top-left
        xyFinalX * xyFinalY,                   // ch2: top-right
        (1.f - xyFinalX) * (1.f - xyFinalY),  // ch3: bottom-left
        xyFinalX * (1.f - xyFinalY),           // ch4: bottom-right
    };

    // Per-channel: read loop buffer (with pitch), source-crossfade with input for output + recording.
    // source left (t=0) = hear/record input; source right (t=1) = hear/record loop (sound on sound).
    // bilinear vol[] always sums to 1.0, so unity gain is preserved across XY positions.
    float outL = 0.f, outR = 0.f;
    float chanMixL[4] = {}, chanMixR[4] = {};

    // Global loop window params
    float sizeParam = clamp(params[SIZE_PARAM].getValue() + inputs[SIZECVIN_INPUT].getVoltage() / 10.f, 0.f, 1.f);
    float posParam  = clamp(params[POSITION_PARAM].getValue() + inputs[POSITIONCVIN_INPUT].getVoltage() / 10.f, 0.f, 1.f);

    for (int i = 0; i < 4; i++) {
        float sampL = 0.f, sampR = 0.f;

        if (playState[i] && hasContent[i] && bufferSize > 0 && !bezelDragging[i].load()) {
            int len = recordedLength[i];
            if (len >= 2) {
                // Compute loop window
                int loopSize  = std::max(100, (int)(sizeParam * (float)len));
                loopSize      = std::min(loopSize, len);
                int loopStart = (int)(posParam * (float)(len - loopSize));
                loopStart     = clamp(loopStart, 0, std::max(0, len - loopSize));
                int loopEnd   = loopStart + loopSize;

                // Clamp readPos into window (handles init and window changes mid-playback)
                if (readPos[i] < (float)loopStart) readPos[i] = (float)loopStart;
                if (readPos[i] >= (float)loopEnd)  readPos[i] = (float)(loopEnd - 1);

                int rp0 = clamp((int)readPos[i], loopStart, loopEnd - 1);
                int rp1 = clamp(rp0 + 1, loopStart, loopEnd - 1);
                float frac = readPos[i] - (float)rp0;
                sampL = bufL[i][rp0] + (bufL[i][rp1] - bufL[i][rp0]) * frac;
                sampR = bufR[i][rp0] + (bufR[i][rp1] - bufR[i][rp0]) * frac;

                float speed = clamp(params[SPEED1_PARAM + i].getValue() + inputs[SPEED1CVIN_INPUT + i].getVoltage() / 10.f, 0.025f, 1.f) * 2.f;
                float dir = playReversed[i] ? -1.f : 1.f;
                readPos[i] += dir * speed;

                bool looping = params[LOOP1_PARAM + i].getValue() > 0.5f;
                if (looping) {
                    if (readPos[i] >= (float)loopEnd)   readPos[i] -= (float)loopSize;
                    if (readPos[i] < (float)loopStart)  readPos[i] += (float)loopSize;
                } else {
                    if (readPos[i] >= (float)loopEnd || readPos[i] < (float)loopStart) {
                        playState[i] = false;
                        readPos[i] = playReversed[i] ? (float)(loopEnd - 1) : (float)loopStart;
                    }
                }
            }
        }

        float srcParam = clamp(params[SOURCE1_PARAM + i].getValue() + inputs[SOURCE1CVIN_INPUT + i].getVoltage() / 10.f, -1.f, 1.f);
        float t = (srcParam + 1.f) * 0.5f; // 0 = input, 1 = loop
        chanMixL[i] = inL * (1.f - t) + sampL * t;
        chanMixR[i] = inR * (1.f - t) + sampR * t;

        outL += chanMixL[i] * vol[i];
        outR += chanMixR[i] * vol[i];

        float level = (std::abs(chanMixL[i]) + std::abs(chanMixR[i])) * 0.5f * vol[i];
        lights[BUFFER1LED_LIGHT + i].setBrightnessSmooth(level, args.sampleTime * 20.f);
    }

    outL = clamp(outL, -5.f, 5.f);
    outR = clamp(outR, -5.f, 5.f);

    outputs[AUDIOLEFTOUT_OUTPUT].setVoltage(outL);
    outputs[AUDIORIGHTOUT_OUTPUT].setVoltage(outR);

    // Recording: first take always records inL at full level so playback volume matches input.
    // Overdubs record chanMixL so the source knob blends in loop content (sound on sound).
    for (int i = 0; i < 4; i++) {
        if (!recordState[i] || bufferSize == 0) continue;

        bufL[i][writePos[i]] = chanMixL[i];
        bufR[i][writePos[i]] = chanMixR[i];

        writePos[i]++;
        if (recordedLength[i] < bufferSize) recordedLength[i]++;
        hasContent[i] = true;

        if (writePos[i] >= bufferSize) {
            writePos[i] = 0;
            recordState[i] = false;
            if (autoPlayFull[i] && !playState[i]) {
                playState[i] = true;
                readPos[i] = 0.f;
            }
        }
    }

    // XY Pad — param + CV offset (CV bypassed while dragging), then slewed
    float targetX, targetY;
    if (!xyDragging.load()) {
        float cvX = inputs[XCVIN_INPUT].isConnected() ? inputs[XCVIN_INPUT].getVoltage() / 10.f : 0.f;
        float cvY = inputs[YCVIN_INPUT].isConnected() ? inputs[YCVIN_INPUT].getVoltage() / 10.f : 0.f;
        targetX = clamp(params[XPOS_PARAM].getValue() + cvX, 0.f, 1.f);
        targetY = clamp(params[YPOS_PARAM].getValue() + cvY, 0.f, 1.f);
    } else {
        targetX = params[XPOS_PARAM].getValue();
        targetY = params[YPOS_PARAM].getValue();
    }

    float slewParam = clamp(params[SLEW_PARAM].getValue() + (inputs[SLEWCVIN_INPUT].isConnected() ? inputs[SLEWCVIN_INPUT].getVoltage() / 10.f : 0.f), 0.02f, 1.f);
    float alpha = clamp(args.sampleTime / slewParam, 0.f, 1.f);
    xyFinalX += (targetX - xyFinalX) * alpha;
    xyFinalY += (targetY - xyFinalY) * alpha;

    outputs[XCVOUT_OUTPUT].setVoltage((xyFinalX - 0.5f) * 10.f);
    outputs[YCVOUT_OUTPUT].setVoltage((xyFinalY - 0.5f) * 10.f);
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
	float eraseFlashTimer = 0.f; // counts down in seconds

	void onButton(const ButtonEvent& e) override {
		if (e.button == GLFW_MOUSE_BUTTON_RIGHT && e.action == GLFW_PRESS) {
			if (tehom) {
				tehom->eraseBuffer(channel);
				eraseFlashTimer = 1.2f;
			}
			e.consume(this);
			return;
		}
		LEDBezel::onButton(e);
	}

	void onEnter(const EnterEvent& e) override { isHovered = true; }

	void onLeave(const LeaveEvent& e) override {
		isHovered = false;
		if (tooltip && eraseFlashTimer <= 0.f) {
			APP->scene->removeChild(tooltip);
			delete tooltip;
			tooltip = nullptr;
		}
	}

	void step() override {
		LEDBezel::step();

		float dt = APP->window->getLastFrameDuration();
		if (eraseFlashTimer > 0.f) {
			eraseFlashTimer -= dt;
			// Show "Erased" tooltip while flashing
			if (!tooltip) {
				tooltip = new ui::Tooltip;
				tooltip->text = "Erased";
				APP->scene->addChild(tooltip);
			} else {
				tooltip->text = "Erased";
			}
			if (eraseFlashTimer <= 0.f) {
				eraseFlashTimer = 0.f;
				APP->scene->removeChild(tooltip);
				delete tooltip;
				tooltip = nullptr;
			}
		} else {
			// Normal: show "Recording..." on hover while active
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


////////////////////////////////
// XY PAD DISPLAY
////////////////////////////////
struct QuadLooperXYDisplay : Widget {
    Tehom *module;
    Vec dragPos;
    bool dragging = false;

    void updateFromPos() {
        dragPos.x = clamp(dragPos.x, 0.f, box.size.x);
        dragPos.y = clamp(dragPos.y, 0.f, box.size.y);

        float x = dragPos.x / box.size.x;
        float y = 1.f - (dragPos.y / box.size.y);

        module->params[Tehom::XPOS_PARAM].setValue(x);
        module->params[Tehom::YPOS_PARAM].setValue(y);
    }

    void onButton(const event::Button &e) override {
        if (!module) return;
        if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
            dragging = true;
            module->xyDragging.store(true);

            // Snap param to current CV-modulated position so cursor doesn't jump
            dragPos.x = module->xyFinalX * box.size.x;
            dragPos.y = (1.f - module->xyFinalY) * box.size.y;

            // Move to clicked position
            dragPos = e.pos;
            dragPos.x = clamp(dragPos.x, 0.f, box.size.x);
            dragPos.y = clamp(dragPos.y, 0.f, box.size.y);

            updateFromPos();
            e.consume(this);
        }
        if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_RELEASE) {
            dragging = false;
            module->xyDragging.store(false);
        }
    }

    void onDragMove(const event::DragMove &e) override {
        if (!module || !dragging) return;

        dragPos = dragPos.plus(e.mouseDelta.div(getAbsoluteZoom()));

        dragPos.x = clamp(dragPos.x, 0.f, box.size.x);
        dragPos.y = clamp(dragPos.y, 0.f, box.size.y);

        updateFromPos();
    }

    void onDragEnd(const event::DragEnd &e) override {
        if (module) module->xyDragging.store(false);
        dragging = false;
    }

    void draw(const DrawArgs &args) override {
        float radius = 6.f;

        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, radius);
        nvgFillColor(args.vg, nvgRGB(18, 18, 18));
        nvgFill(args.vg);

        if (!module) return;

        float px = module->xyFinalX * box.size.x;
        float py = (1.f - module->xyFinalY) * box.size.y;

        px = clamp(px, radius, box.size.x - radius);
        py = clamp(py, radius, box.size.y - radius);

        // Crosshair
        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, px, 0);
        nvgLineTo(args.vg, px, box.size.y);
        nvgMoveTo(args.vg, 0, py);
        nvgLineTo(args.vg, box.size.x, py);
        nvgStrokeColor(args.vg, nvgRGB(60, 60, 60));
        nvgStroke(args.vg);

        // Cursor dot
        nvgBeginPath(args.vg);
        nvgCircle(args.vg, px, py, radius);
        nvgFillColor(args.vg, nvgRGB(230, 230, 230));
        nvgFill(args.vg);
    }
};

// Custom channel colors: Cyan, Magenta, Orange, Purple
struct ChanLight0 : GrayModuleLightWidget { ChanLight0() { addBaseColor(SCHEME_CYAN); } };
struct ChanLight1 : GrayModuleLightWidget { ChanLight1() { addBaseColor(SCHEME_PURPLE); } };
struct ChanLight2 : GrayModuleLightWidget { ChanLight2() { addBaseColor(SCHEME_ORANGE); } };
struct ChanLight3 : GrayModuleLightWidget { ChanLight3() { addBaseColor(SCHEME_RED); } };

struct TehomWidget : ModuleWidget {
	TehomWidget(Tehom* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/panels/Tehom.svg")));

		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // === 40x40 XY Pad, centered ===
        auto xy = new QuadLooperXYDisplay();
        xy->module = module;

        float squareSize = 40.f; // 40 x 40 mm
        float xOffset = (152.4f - squareSize) / 2.f; // center horizontally
        float yOffset = (128.5f - squareSize) / 2.f; // center vertically

        xy->box.pos = mm2px(Vec(xOffset, yOffset));
        xy->box.size = mm2px(Vec(squareSize, squareSize));
        addChild(xy);

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
				addChild(createLightCentered<LEDBezelLight<WhiteLight>>(recPos[i], module, Tehom::RECORD1_FLASH_LIGHT + i));
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
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(38.86, 39.243)), module, Tehom::SPEED1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(137.275, 39.243)), module, Tehom::SPEED2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(38.86, 101.014)), module, Tehom::SPEED3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(137.275, 101.014)), module, Tehom::SPEED4_PARAM));
	
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
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(44.33, 23.541)), module, Tehom::SPEED1CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(20.788, 23.541)), module, Tehom::RECORD1CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(32.932, 23.541)), module, Tehom::PLAY1CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(107.848, 23.541)), module, Tehom::SOURCE2CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(142.745, 23.541)), module, Tehom::SPEED2CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(119.202, 23.541)), module, Tehom::RECORD2CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(131.347, 23.541)), module, Tehom::PLAY2CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(9.434, 85.312)), module, Tehom::SOURCE3CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(44.33, 85.312)), module, Tehom::SPEED3CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(20.788, 85.312)), module, Tehom::RECORD3CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(32.932, 85.312)), module, Tehom::PLAY3CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(107.848, 85.312)), module, Tehom::SOURCE4CVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(142.745, 85.312)), module, Tehom::SPEED4CVIN_INPUT));
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

		// Lights — each channel has a distinct color, brightness driven by audio output
		addChild(createLightCentered<LargeLight<ChanLight0>>(mm2px(Vec(26.9, 10.166)), module, Tehom::BUFFER1LED_LIGHT));
		addChild(createLightCentered<LargeLight<ChanLight1>>(mm2px(Vec(125.315, 10.166)), module, Tehom::BUFFER2LED_LIGHT));
		addChild(createLightCentered<LargeLight<ChanLight2>>(mm2px(Vec(26.9, 71.937)), module, Tehom::BUFFER3LED_LIGHT));
		addChild(createLightCentered<LargeLight<ChanLight3>>(mm2px(Vec(125.315, 71.937)), module, Tehom::BUFFER4LED_LIGHT));
	}
    void appendContextMenu(Menu* menu) override {
        ModuleWidget::appendContextMenu(menu);
        auto* tehom = dynamic_cast<Tehom*>(module);
        if (!tehom) return;

        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("Buffer Size (all channels)"));
        const float durations[] = {1.f, 2.f, 5.f, 10.f, 20.f, 30.f, 60.f};
        const char* labels[]    = {"1 second", "2 seconds", "5 seconds", "10 seconds", "20 seconds", "30 seconds", "1 minute"};
        for (int j = 0; j < 7; j++) {
            float dur = durations[j];
            menu->addChild(createCheckMenuItem(
                labels[j], "",
                [=]() { return tehom->bufferDuration == dur; },
                [=]() { tehom->bufferDuration = dur; tehom->resizeBuffers(tehom->currentSampleRate); }
            ));
        }

        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("Play CV — Retrigger mode"));
        for (int i = 0; i < 4; i++) {
            menu->addChild(createCheckMenuItem(
                "Channel " + std::to_string(i + 1), "",
                [=]() { return tehom->retrigger[i]; },
                [=]() { tehom->retrigger[i] = !tehom->retrigger[i]; }
            ));
        }

        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("Auto-Play when recording complete"));
        for (int i = 0; i < 4; i++) {
            menu->addChild(createCheckMenuItem(
                "Channel " + std::to_string(i + 1), "",
                [=]() { return tehom->autoPlay[i]; },
                [=]() { tehom->autoPlay[i] = !tehom->autoPlay[i]; }
            ));
        }

        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("Auto-Play when buffer full"));
        for (int i = 0; i < 4; i++) {
            menu->addChild(createCheckMenuItem(
                "Channel " + std::to_string(i + 1), "",
                [=]() { return tehom->autoPlayFull[i]; },
                [=]() { tehom->autoPlayFull[i] = !tehom->autoPlayFull[i]; }
            ));
        }
    }
};
Model* modelTehom = createModel<Tehom, TehomWidget>("Tehom");