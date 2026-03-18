#include "plugin.hpp"
#include <string>
#include <cstring>
#include <cmath>
#include <cctype>

// ============================================================
// Morse code tables (IATA / ITU standard)
// 52 characters: A-Z, 0-9, + 16 punctuation
// ============================================================
static const int NUM_MORSE_CHARS = 52;
static const char MORSE_CHARS[NUM_MORSE_CHARS + 1] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,?!/';:-@()=+&\"";

static const char* MORSE_CODE[NUM_MORSE_CHARS] = {
    // A-Z
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
    "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
    "..-", "...-", ".--", "-..-", "-.--", "--..",
    // 0-9
    "-----", ".----", "..---", "...--", "....-",
    ".....", "-....", "--...", "---..", "----.",
    // Punctuation: . , ? ! / ' ; : - @ ( ) = + & "
    ".-.-.-", "--..--", "..--..", "-.-.--", "-..-.",
    ".----.", "-.-.-.", "---...", "-....-", ".--.-.",
    "-.--.", "-.--.-", "-...-", ".-.-.", ".-...", ".-..-.",
};

static const char* getMorse(char c) {
    c = (char)toupper((unsigned char)c);
    for (int i = 0; i < NUM_MORSE_CHARS; i++) {
        if (MORSE_CHARS[i] == c) return MORSE_CODE[i];
    }
    return nullptr;
}

// ============================================================
// Random quotes (time-related phrases, ported from PROGMEM)
// ============================================================
static const char* QUOTES[] = {
    "TIME FLIES", "TICK TOCK", "NOW OR NEVER", "TEMPUS FUGIT",
    "CARPE DIEM", "TOMORROW NEVER COMES", "TIME IS MONEY",
    "BETTER LATE THAN NEVER", "THE EARLY BIRD", "NO TIME LIKE THE PRESENT",
    "TIME WAITS FOR NO ONE", "LOST TIME IS NEVER FOUND AGAIN",
    "TIME HEALS ALL WOUNDS", "IN THE NICK OF TIME", "ONCE IN A BLUE MOON",
    "KILLING TIME", "AHEAD OF TIME", "BEHIND THE TIMES", "JUST IN TIME",
    "AT THE ELEVENTH HOUR", "THE SANDS OF TIME", "DAYLIGHT SAVING",
    "AGAINST THE CLOCK", "RACE AGAINST TIME", "HALF TIME",
    "OVERTIME", "PRIME TIME", "FACE TIME", "REAL TIME", "SPARE TIME",
    "NIGHT OWL", "EARLY BIRD", "MIDNIGHT OIL", "GOLDEN HOUR",
    "WITCHING HOUR", "ZERO HOUR", "RUSH HOUR", "HAPPY HOUR",
    "LONG TIME NO SEE", "OLD AS TIME", "TIMELESS", "VINTAGE",
    "ANCIENT HISTORY", "MODERN TIMES", "NEW WAVE", "OLD SCHOOL",
    "PAST PRESENT FUTURE", "THE FOURTH DIMENSION", "RELATIVITY",
    "ENTROPY ALWAYS INCREASES", "SECOND BY SECOND", "HOUR BY HOUR",
};
static const int NUM_QUOTES = (int)(sizeof(QUOTES) / sizeof(QUOTES[0]));

// ============================================================
// Module constants
// ============================================================
static const int MAX_CHARS    = 84;
static const int NUM_SLOTS    = 8;
static const double MAX_CLOCK_PERIOD = 0.045; // 45ms default
static const double MIN_PULSE_WIDTH  = 0.010; // 10ms minimum

// ============================================================
// DitDit Module
// ============================================================
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

    // ── Text storage ──────────────────────────────────────────
    char textBuffer[MAX_CHARS + 1] = {};
    char savedSlots[NUM_SLOTS][MAX_CHARS + 1] = {};

    // ── Persistent settings (also driven by params) ───────────
    bool bufferLoop     = true;
    bool letterSpacing  = true;
    bool wordSpacing    = true;
    bool dahDitWeight   = false; // false=3:1 (default), true=2:1
    bool pulsewidth     = false; // false=gate (default), true=trigger

    // ── Morse playback state ──────────────────────────────────
    int         currentMorseIndex = 0;
    int         charPulseCount    = 0;
    const char* currentMorseCode  = nullptr;
    bool        endReached        = false;
    bool        flagReset         = false;
    int         gapTicksRemaining = 0;  // silent clock ticks left for letter/word gap

    // ── Output state ──────────────────────────────────────────
    bool dotOutputHigh   = false;
    bool dashOutputHigh  = false;
    bool morseOutputHigh = false;
    bool isDot           = false;

    // ── Timing (seconds) ─────────────────────────────────────
    double engineTime     = 0.0;
    double outputEndTime  = 0.0;

    // ── Clock measurement ─────────────────────────────────────
    bool   prevBangHigh    = false;
    double lastClockTime   = 0.0;
    double clockPeriod     = MAX_CLOCK_PERIOD;
    double lastDuration    = MAX_CLOCK_PERIOD;
    bool   initClock       = true;
    double noClockTimer    = 0.0;

    // ── Mode ──────────────────────────────────────────────────
    int  currentMode       = 0;   // 0=entry, 1=playback
    bool prevSwitchHigh    = false;
    bool prevResetHigh     = false;

    // ── Display state (read by widget) ────────────────────────
    char displayChar           = ' ';
    char displayMorsePattern[33] = {};
    int  displayMorsePos       = 0;

    // ── Random quote tracking ─────────────────────────────────
    int lastQuoteIndex = -1;

    DitDit() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configSwitch(SWITCH_PARAM, 0.f, 1.f, 0.f, "Start/Stop", {"Stop", "Start"});
        configInput(BANG_INPUT,    "Bang! (Clock)");
        configInput(RESET_INPUT,   "Reset");
        configInput(STARTSTOP_INPUT, "Start/Stop CV");
        configOutput(DIT_OUTPUT,   "Dit");
        configOutput(DAH_OUTPUT,   "Dah");
        configOutput(MORSE_OUTPUT, "Morse");
        memset(textBuffer, 0, sizeof(textBuffer));
        memset(savedSlots, 0, sizeof(savedSlots));
        memset(displayMorsePattern, 0, sizeof(displayMorsePattern));
    }

    bool isBufferEmpty() const {
        for (int i = 0; i < MAX_CHARS; i++) {
            char c = textBuffer[i];
            if (c != 0 && c != ' ' && c != '\n') return false;
        }
        return true;
    }

    void resetPos() {
        currentMorseIndex = 0;
        charPulseCount    = 0;
        currentMorseCode  = nullptr;
        endReached          = false;
        flagReset           = true;
        gapTicksRemaining   = 0;
        dotOutputHigh       = false;
        dashOutputHigh    = false;
        morseOutputHigh   = false;
        outputEndTime     = engineTime;
        displayChar       = ' ';
        memset(displayMorsePattern, 0, sizeof(displayMorsePattern));
        displayMorsePos   = 0;
    }

    // Advance to the next character in the text buffer
    void incrementIndex() {
        currentMorseIndex++;
        charPulseCount   = 0;
        currentMorseCode = nullptr;

        // Skip null terminators at end
        while (currentMorseIndex < MAX_CHARS && textBuffer[currentMorseIndex] == 0) {
            currentMorseIndex++;
        }

        if (currentMorseIndex >= MAX_CHARS) {
            if (bufferLoop) {
                currentMorseIndex = 0;
                charPulseCount   = 0;
                currentMorseCode = nullptr;
            } else {
                endReached = true;
            }
        }
    }

    void loadSlot(int slot) {
        if (slot < 0 || slot >= NUM_SLOTS) return;
        strncpy(textBuffer, savedSlots[slot], MAX_CHARS);
        textBuffer[MAX_CHARS] = 0;
        resetPos();
    }

    void saveSlot(int slot) {
        if (slot < 0 || slot >= NUM_SLOTS) return;
        strncpy(savedSlots[slot], textBuffer, MAX_CHARS);
        savedSlots[slot][MAX_CHARS] = 0;
    }

    void onReset() override {
        memset(textBuffer, 0, sizeof(textBuffer));
        bufferLoop    = true;
        letterSpacing = true;
        wordSpacing   = true;
        dahDitWeight  = false;
        pulsewidth    = false;
        currentMode   = 0;
        resetPos();
    }

    void loadRandomQuote() {
        int idx;
        do {
            idx = (int)(rand() % NUM_QUOTES);
        } while (idx == lastQuoteIndex && NUM_QUOTES > 1);
        lastQuoteIndex = idx;
        strncpy(textBuffer, QUOTES[idx], MAX_CHARS);
        textBuffer[MAX_CHARS] = 0;
        resetPos();
    }

    void process(const ProcessArgs& args) override {
        engineTime  += args.sampleTime;
        noClockTimer += args.sampleTime;

        // ── Timing factors ────────────────────────────────────
        double dashPercent = dahDitWeight ? 190.0 : 290.0; // false=3:1 default, true=2:1
        double dotDuration  = std::max(clockPeriod * 0.50,           MIN_PULSE_WIDTH);
        double dashDuration = std::max(clockPeriod * dashPercent / 100.0, MIN_PULSE_WIDTH);
        // letter gap = 3 dits total (minus the inter-element gap already accounted)
        // word gap   = 7 dits total

        // ── Mode switch ───────────────────────────────────────
        bool switchHigh   = params[SWITCH_PARAM].getValue() > 0.5f;
        bool startStopCV  = inputs[STARTSTOP_INPUT].getVoltage() > 1.0f;
        bool effectiveSwitch = switchHigh || startStopCV;

        if (effectiveSwitch && !prevSwitchHigh) {
            // Enter playback mode regardless of buffer state
            currentMode = 1;
            resetPos();
            initClock    = true;
            noClockTimer = 0.0;
        } else if (!effectiveSwitch && prevSwitchHigh) {
            currentMode      = 0;
            dotOutputHigh    = false;
            dashOutputHigh   = false;
            morseOutputHigh  = false;
        }
        prevSwitchHigh = effectiveSwitch;

        // ── Reset input ───────────────────────────────────────
        bool resetHigh = inputs[RESET_INPUT].getVoltage() > 1.0f;
        if (resetHigh && !prevResetHigh) {
            resetPos();
        }
        prevResetHigh = resetHigh;

        // ── Bang/Clock rising edge ────────────────────────────
        bool bangHigh    = inputs[BANG_INPUT].getVoltage() > 1.0f;
        bool risingEdge  = bangHigh && !prevBangHigh;
        prevBangHigh     = bangHigh;

        if (risingEdge && currentMode == 1 && !isBufferEmpty()) {
            noClockTimer = 0.0;

            if (initClock) {
                lastClockTime = engineTime;
                clockPeriod   = MAX_CLOCK_PERIOD;
                lastDuration  = MAX_CLOCK_PERIOD;
                initClock     = false;
                flagReset     = false;
            } else {
                // Measure clock period, accept reasonable values
                double measured = engineTime - lastClockTime;
                lastClockTime   = engineTime;
                if (measured > 0.005 && measured < 5.0) {
                    clockPeriod  = measured;
                    lastDuration = measured;
                    // Recalculate durations with new period
                    dotDuration  = std::max(clockPeriod * 0.50,           MIN_PULSE_WIDTH);
                    dashDuration = std::max(clockPeriod * dashPercent / 100.0, MIN_PULSE_WIDTH);
                }
            }

            if (!endReached && !flagReset) {
                // ── State machine: advance morse output ───────
                //
                // Each clock rising edge = 1 dit unit.
                // Gaps are implemented by counting silent ticks:
                //   letter gap = 3 dits → 2 extra silent ticks (1 is implicit from inter-element)
                //   word gap   = 7 dits → 6 extra silent ticks

                if (gapTicksRemaining > 0) {
                    // Consume one silent tick for letter/word spacing
                    gapTicksRemaining--;
                    dotOutputHigh   = false;
                    dashOutputHigh  = false;
                    morseOutputHigh = false;
                } else {
                    // Find the next encodable character
                    while (currentMorseIndex < MAX_CHARS) {
                        char c = textBuffer[currentMorseIndex];

                        if (c == 0) {
                            // End of message
                            if (bufferLoop) {
                                currentMorseIndex = 0;
                                charPulseCount    = 0;
                                currentMorseCode  = nullptr;
                                displayChar = ' ';
                                memset(displayMorsePattern, 0, sizeof(displayMorsePattern));
                                displayMorsePos = 0;
                                c = textBuffer[0];
                                if (c == 0) { endReached = true; break; }
                            } else {
                                endReached = true;
                                break;
                            }
                        }

                        if (c == ' ' || c == '\n') {
                            // Word/line gap
                            incrementIndex();
                            dotOutputHigh   = false;
                            dashOutputHigh  = false;
                            morseOutputHigh = false;
                            if (wordSpacing) gapTicksRemaining = 6; // 7 dits - 1 implicit = 6
                            else             gapTicksRemaining = 2; // fall back to letter gap
                            break;
                        }

                        const char* morse = getMorse(c);
                        if (!morse) {
                            incrementIndex();
                            continue;
                        }

                        // Starting a new character?
                        if (currentMorseCode != morse) {
                            currentMorseCode = morse;
                            charPulseCount   = 0;
                            displayChar      = (char)toupper((unsigned char)c);
                            memset(displayMorsePattern, 0, sizeof(displayMorsePattern));
                            displayMorsePos  = 0;
                        }

                        if (charPulseCount < (int)strlen(morse)) {
                            bool dot        = (morse[charPulseCount] == '.');
                            isDot           = dot;
                            double pulseDur = dot ? dotDuration : dashDuration;
                            if (pulsewidth) pulseDur = MIN_PULSE_WIDTH; // trigger mode when true

                            outputEndTime   = engineTime + pulseDur;
                            dotOutputHigh   = dot;
                            dashOutputHigh  = !dot;
                            morseOutputHigh = true;

                            if (displayMorsePos < 31) {
                                displayMorsePattern[displayMorsePos++] = dot ? '.' : '_';
                                displayMorsePattern[displayMorsePos]   = 0;
                            }

                            charPulseCount++;

                            if (charPulseCount >= (int)strlen(morse)) {
                                // Finished this character — apply letter gap then advance
                                if (letterSpacing) gapTicksRemaining = 2; // 3 dits - 1 = 2
                                incrementIndex();
                            }
                        } else {
                            incrementIndex();
                        }
                        break;
                    }
                }
            } else if (endReached) {
                dotOutputHigh   = false;
                dashOutputHigh  = false;
                morseOutputHigh = false;
            }

            flagReset = false;
        }

        // ── Pulse end: drop outputs when timing expires ───────
        if (engineTime >= outputEndTime) {
            dotOutputHigh   = false;
            dashOutputHigh  = false;
            morseOutputHigh = false;
        }

        // ── Set outputs ───────────────────────────────────────
        bool active = (currentMode == 1) && !isBufferEmpty();
        outputs[DIT_OUTPUT].setVoltage(  (active && dotOutputHigh)   ? 5.f : 0.f);
        outputs[DAH_OUTPUT].setVoltage(  (active && dashOutputHigh)  ? 5.f : 0.f);
        outputs[MORSE_OUTPUT].setVoltage((active && morseOutputHigh) ? 5.f : 0.f);

        // ── Lights ────────────────────────────────────────────
        lights[DITLED_LIGHT].setBrightnessSmooth(  dotOutputHigh   ? 1.f : 0.f, args.sampleTime);
        lights[DAHLED_LIGHT].setBrightnessSmooth(  dashOutputHigh  ? 1.f : 0.f, args.sampleTime);
        lights[MORSELED_LIGHT].setBrightnessSmooth(morseOutputHigh ? 1.f : 0.f, args.sampleTime);
    }

    json_t* dataToJson() override {
        json_t* rootJ = json_object();
        json_object_set_new(rootJ, "textBuffer",    json_string(textBuffer));
        json_object_set_new(rootJ, "bufferLoop",    json_boolean(bufferLoop));
        json_object_set_new(rootJ, "letterSpacing", json_boolean(letterSpacing));
        json_object_set_new(rootJ, "wordSpacing",   json_boolean(wordSpacing));
        json_object_set_new(rootJ, "dahDitWeight",  json_boolean(dahDitWeight));
        json_object_set_new(rootJ, "pulsewidth",    json_boolean(pulsewidth));

        json_t* slotsJ = json_array();
        for (int i = 0; i < NUM_SLOTS; i++) {
            json_array_append_new(slotsJ, json_string(savedSlots[i]));
        }
        json_object_set_new(rootJ, "savedSlots", slotsJ);
        return rootJ;
    }

    void dataFromJson(json_t* rootJ) override {
        json_t* j;
        j = json_object_get(rootJ, "textBuffer");
        if (j) { strncpy(textBuffer, json_string_value(j), MAX_CHARS); textBuffer[MAX_CHARS] = 0; }

        j = json_object_get(rootJ, "bufferLoop");
        if (j) bufferLoop = json_boolean_value(j);
        j = json_object_get(rootJ, "letterSpacing");
        if (j) letterSpacing = json_boolean_value(j);
        j = json_object_get(rootJ, "wordSpacing");
        if (j) wordSpacing = json_boolean_value(j);
        j = json_object_get(rootJ, "dahDitWeight");
        if (j) dahDitWeight = json_boolean_value(j);
        j = json_object_get(rootJ, "pulsewidth");
        if (j) pulsewidth = json_boolean_value(j);

        j = json_object_get(rootJ, "savedSlots");
        if (j && json_is_array(j)) {
            for (int i = 0; i < NUM_SLOTS && i < (int)json_array_size(j); i++) {
                json_t* slotJ = json_array_get(j, i);
                if (slotJ) {
                    strncpy(savedSlots[i], json_string_value(slotJ), MAX_CHARS);
                    savedSlots[i][MAX_CHARS] = 0;
                }
            }
        }
    }
};

// ============================================================
// Display Widget — double-click to type
// ============================================================
struct DitDitDisplay : LedDisplay {
    DitDit* module;
    bool  editing          = false;
    bool  clearOnFirstChar = false;  // first keypress clears existing text
    bool  blinkOn          = true;
    float blinkTimer       = 0.f;
    float timeSinceLastClick = 999.f;

    static const int CHARS_PER_LINE = 15;
    static const int NUM_LINES      = 4;

    // ── Event handling ────────────────────────────────────────

    // Must consume hover so we receive button events
    void onHover(const HoverEvent& e) override { e.consume(this); }

    void onButton(const ButtonEvent& e) override {
        if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT) {
            if (timeSinceLastClick < 0.4f && module && module->currentMode == 0) {
                // Double-click → start editing
                editing          = true;
                clearOnFirstChar = true;
                APP->event->setSelectedWidget(this);
            }
            timeSinceLastClick = 0.f;
            e.consume(this);
        }
    }

    void onDeselect(const DeselectEvent& e) override {
        editing = false;
    }

    void onSelectText(const SelectTextEvent& e) override {
        if (!editing || !module) { e.consume(this); return; }
        char c = (char)toupper((unsigned char)e.codepoint);
        if (getMorse(c) != nullptr || c == ' ') {
            if (clearOnFirstChar) {
                memset(module->textBuffer, 0, MAX_CHARS + 1);
                clearOnFirstChar = false;
            }
            int len = (int)strlen(module->textBuffer);
            if (len < MAX_CHARS) {
                module->textBuffer[len]     = c;
                module->textBuffer[len + 1] = 0;
            }
        }
        e.consume(this);
    }

    void onSelectKey(const SelectKeyEvent& e) override {
        if (!editing || !module) { e.consume(this); return; }
        if (e.action == GLFW_PRESS || e.action == GLFW_REPEAT) {
            switch (e.key) {
                case GLFW_KEY_ESCAPE:
                case GLFW_KEY_ENTER:
                    editing = false;
                    APP->event->setSelectedWidget(nullptr);
                    break;
                case GLFW_KEY_BACKSPACE: {
                    clearOnFirstChar = false;
                    int len = (int)strlen(module->textBuffer);
                    if (len > 0) module->textBuffer[len - 1] = 0;
                    break;
                }
                default: break;
            }
        }
        e.consume(this);
    }

    void step() override {
        LedDisplay::step();
        float dt = APP->window->getLastFrameDuration();
        timeSinceLastClick += dt;
        blinkTimer += dt;
        if (blinkTimer >= 0.5f) {
            blinkTimer -= 0.5f;
            blinkOn = !blinkOn;
        }
    }

    // ── Drawing ───────────────────────────────────────────────

    void drawLayer(const DrawArgs& args, int layer) override {
        if (layer != 1) { LedDisplay::drawLayer(args, layer); return; }

        nvgScissor(args.vg, RECT_ARGS(args.clipBox));

        // Background
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
        nvgFillColor(args.vg, nvgRGB(0x08, 0x08, 0x08));
        nvgFill(args.vg);

        nvgFontFaceId(args.vg, APP->window->uiFont->handle);
        NVGcolor oled     = nvgRGB(0xB8, 0xCC, 0xFF);
        NVGcolor oledDim  = nvgRGBAf(0.72f, 0.80f, 1.0f, 0.35f);  // dim placeholder
        NVGcolor orange   = nvgRGB(0xff, 0x80, 0x00);
        float    cx       = box.size.x * 0.5f;
        float    cy       = box.size.y * 0.5f;

        if (!module) {
            // Browser/preview
            nvgFontSize(args.vg, 6.f);
            nvgFillColor(args.vg, oled);
            nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
            nvgText(args.vg, cx, cy - 7, "DIT DIT", nullptr);
            nvgText(args.vg, cx, cy,     "MORSE CODE", nullptr);
            nvgText(args.vg, cx, cy + 7, "GENERATOR", nullptr);
            nvgResetScissor(args.vg);
            LedDisplay::drawLayer(args, layer);
            return;
        }

        if (module->currentMode == 0) {
            // ── Stop/entry mode ───────────────────────────────
            bool empty = module->isBufferEmpty();

            if (empty && !editing) {
                // Placeholder hint
                nvgFontSize(args.vg, 5.f);
                nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
                nvgFillColor(args.vg, oledDim);
                nvgText(args.vg, cx, cy - 5, "DOUBLE-CLICK", nullptr);
                nvgText(args.vg, cx, cy + 3, "TO TYPE", nullptr);
            } else {
                // Show text buffer in 4 lines of 15 chars
                nvgFontSize(args.vg, 5.f);
                nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);
                nvgFillColor(args.vg, oled);

                for (int line = 0; line < NUM_LINES; line++) {
                    char lineStr[CHARS_PER_LINE + 2] = {};
                    int startIdx = line * CHARS_PER_LINE;
                    for (int col = 0; col < CHARS_PER_LINE; col++) {
                        int idx = startIdx + col;
                        char c = (idx < MAX_CHARS) ? module->textBuffer[idx] : 0;
                        lineStr[col] = (c != 0 && c != '\n') ? (char)toupper((unsigned char)c) : ' ';
                    }
                    lineStr[CHARS_PER_LINE] = 0;
                    nvgText(args.vg, 2, 7 + line * 8, lineStr, nullptr);
                }

                // Blinking cursor after last character when editing
                if (editing && blinkOn) {
                    int len = (int)strlen(module->textBuffer);
                    int col = len % CHARS_PER_LINE;
                    int row = len / CHARS_PER_LINE;
                    // Approximate char width at font size 5 ≈ 3.8px
                    float cx2 = 2.f + col * 3.8f;
                    float cy2 = 7.f + row * 8.f;
                    nvgBeginPath(args.vg);
                    nvgRect(args.vg, cx2, cy2, 3.f, 1.f);
                    nvgFillColor(args.vg, oled);
                    nvgFill(args.vg);
                }
            }

        } else {
            // ── Playback mode ─────────────────────────────────
            nvgFillColor(args.vg, oled);

            // Status messages when something is missing
            {
                bool bangConnected = module->inputs[DitDit::BANG_INPUT].isConnected();
                bool bufEmpty      = module->isBufferEmpty();
                const char* msg    = nullptr;
                if (bufEmpty)                  msg = "NO TEXT INPUT";
                else if (!bangConnected)       msg = "NO CLOCK INPUT";

                if (msg) {
                    nvgFontSize(args.vg, 6.f);
                    nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
                    nvgFillColor(args.vg, oled);
                    nvgText(args.vg, cx, cy, msg, nullptr);
                    nvgResetScissor(args.vg);
                    LedDisplay::drawLayer(args, layer);
                    return;
                }
            }

            // Current character — centered, upper area
            nvgFontSize(args.vg, 12.f);
            nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
            char charStr[2] = {module->displayChar, 0};
            nvgText(args.vg, cx, 10.f, charStr, nullptr);

            // Morse pattern — space after each dash, centered below
            char spaced[64] = {};
            int sp = 0;
            for (int i = 0; module->displayMorsePattern[i] && sp < 61; i++) {
                char el = module->displayMorsePattern[i];
                spaced[sp++] = el;
                if (el == '_') spaced[sp++] = ' ';
            }
            spaced[sp] = 0;
            nvgFontSize(args.vg, 11.f);
            nvgText(args.vg, cx, 19.f, spaced, nullptr);

            nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);
            if (module->endReached) {
                nvgFontSize(args.vg, 5.f);
                nvgFillColor(args.vg, orange);
                nvgText(args.vg, 2, 36.f, "END", nullptr);
            }
            if (module->noClockTimer > 7.0) {
                nvgFontSize(args.vg, 5.f);
                nvgFillColor(args.vg, orange);
                nvgText(args.vg, 22, 36.f, "NO CLOCK", nullptr);
            }
        }

        nvgResetScissor(args.vg);
        LedDisplay::drawLayer(args, layer);
    }
};

// ============================================================
// Widget
// ============================================================
struct DitDitWidget : ModuleWidget {
    DitDitWidget(DitDit* module) {
        setModule(module);
        setPanel(createPanel(
            asset::plugin(pluginInstance, "res/panels/DitDit.svg"),
            asset::plugin(pluginInstance, "res/panels/DitDit-dark.svg")
        ));

        // Screws
        addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // ── Display: positioned over the OLED cutout in the SVG panel ──
        // Cutout coords from DitDit.svg: x=8.611 y=14.890 w=23.414 h=10.000 (mm)
        auto* display = createWidget<DitDitDisplay>(mm2px(Vec(8.611, 14.890)));
        display->box.size = mm2px(Vec(23.414, 10.0));
        display->module   = module;
        addChild(display);

        // ── Start/Stop switch (original position) ────────────
        addParam(createParamCentered<CKSS>(mm2px(Vec(20.32, 51.353)), module, DitDit::SWITCH_PARAM));

        // ── Inputs ───────────────────────────────────────────
        addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(8.611,  79.136)), module, DitDit::BANG_INPUT));
        addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(20.32,  79.136)), module, DitDit::RESET_INPUT));
        addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(32.029, 79.136)), module, DitDit::STARTSTOP_INPUT));

        // ── Lights ───────────────────────────────────────────
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(8.611,  91.04)), module, DitDit::DITLED_LIGHT));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.32,  91.04)), module, DitDit::DAHLED_LIGHT));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(32.029, 91.04)), module, DitDit::MORSELED_LIGHT));

        // ── Outputs ──────────────────────────────────────────
        addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(8.611,  100.963)), module, DitDit::DIT_OUTPUT));
        addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(20.32,  100.963)), module, DitDit::DAH_OUTPUT));
        addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(32.029, 100.963)), module, DitDit::MORSE_OUTPUT));
    }

    void appendContextMenu(Menu* menu) override {
        DitDit* module = dynamic_cast<DitDit*>(this->module);
        if (!module) return;

        // ── Settings toggles ─────────────────────────────────
        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("Settings"));

        menu->addChild(createBoolPtrMenuItem("Loop", "", &module->bufferLoop));
        menu->addChild(createBoolPtrMenuItem("Letter spacing", "", &module->letterSpacing));
        menu->addChild(createBoolPtrMenuItem("Word spacing", "", &module->wordSpacing));
        menu->addChild(createBoolPtrMenuItem("Dah:Dit Ratio 2:1", "", &module->dahDitWeight));
        menu->addChild(createBoolPtrMenuItem("Morse Output: Trigger", "", &module->pulsewidth));

        // ── Text ─────────────────────────────────────────────
        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("Text"));

        // ── Random quote ─────────────────────────────────────
        menu->addChild(createMenuItem("Load random quote", "", [=]() {
            module->loadRandomQuote();
        }));

        // ── Clear text ───────────────────────────────────────
        menu->addChild(createMenuItem("Clear text", "", [=]() {
            memset(module->textBuffer, 0, sizeof(module->textBuffer));
            module->resetPos();
        }));

        // ── Delete all memory slots ───────────────────────────
        menu->addChild(createMenuItem("Delete all memory slots", "", [=]() {
            memset(module->savedSlots, 0, sizeof(module->savedSlots));
        }));

        // ── Save slots ───────────────────────────────────────
        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("Save to slot"));
        for (int i = 0; i < NUM_SLOTS; i++) {
            std::string label = "Slot " + std::to_string(i + 1);
            // Show preview if slot has content
            if (module->savedSlots[i][0] != 0) {
                char preview[16] = {};
                strncpy(preview, module->savedSlots[i], 12);
                preview[12] = 0;
                label += "  [" + std::string(preview) + "...]";
            }
            menu->addChild(createMenuItem(label, "", [=]() {
                module->saveSlot(i);
            }));
        }

        // ── Load slots ───────────────────────────────────────
        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("Load from slot"));
        for (int i = 0; i < NUM_SLOTS; i++) {
            std::string label = "Slot " + std::to_string(i + 1);
            bool empty = (module->savedSlots[i][0] == 0);
            if (!empty) {
                char preview[16] = {};
                strncpy(preview, module->savedSlots[i], 12);
                preview[12] = 0;
                label += "  [" + std::string(preview) + "...]";
            } else {
                label += "  [empty]";
            }
            menu->addChild(createMenuItem(label, "", [=]() {
                if (!empty) module->loadSlot(i);
            }));
        }
    }
};

Model* modelDitDit = createModel<DitDit, DitDitWidget>("DitDit");
