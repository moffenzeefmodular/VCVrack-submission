#include "plugin.hpp"
#include <string>
#include <cstring>
#include <cmath>
#include <cctype>
#include <atomic>
#include <mutex>

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
    int         gapTicksRemaining    = 0;  // silent clock ticks left for letter/word gap
    int         elementTicksRemaining = 0;  // ticks remaining to hold current element (blocks state machine)
    int         pendingGapTicks       = 0;  // gap ticks to apply after elementTicksRemaining finishes
    bool        pendingBlank          = false; // blank display when pending gap is applied

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
    // noClockTimer written every sample by audio, read by UI — use atomic float
    // so the UI always sees a valid value without locking on every sample.
    std::atomic<float> noClockTimer{0.0f};

    // ── Mode ──────────────────────────────────────────────────
    int  currentMode       = 0;   // 0=entry, 1=playback
    bool prevSwitchHigh    = false;
    bool prevResetHigh     = false;

    // ── Display state (written by audio, read by UI) ──────────
    char displayChar           = ' ';
    char displayMorsePattern[33] = {};
    int  displayMorsePos       = 0;

    // ── Thread safety ─────────────────────────────────────────
    // textMutex: UI thread always locks for textBuffer reads/writes.
    //            Audio thread uses try_lock on rising edges only — never blocks.
    // displayMutex: Audio thread locks briefly on state changes to update display
    //               vars. UI thread locks during drawLayer. Low contention (~60Hz).
    // bufEmpty: Cached atomic bool — avoids scanning 84 bytes every audio sample.
    mutable std::mutex textMutex;
    mutable std::mutex displayMutex;
    std::atomic<bool>  bufEmpty{true};

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

    // Fast O(1) check used by audio thread — reads the cached atomic.
    bool isBufferEmpty() const {
        return bufEmpty.load(std::memory_order_acquire);
    }

    // Scans textBuffer and updates bufEmpty. Call from UI thread after any
    // textBuffer modification so the audio thread always sees a fresh value.
    void updateBufEmpty() {
        for (int i = 0; i < MAX_CHARS; i++) {
            char c = textBuffer[i];
            if (c != 0 && c != ' ' && c != '\n') {
                bufEmpty.store(false, std::memory_order_release);
                return;
            }
        }
        bufEmpty.store(true, std::memory_order_release);
    }

    void resetPos() {
        currentMorseIndex     = 0;
        charPulseCount        = 0;
        currentMorseCode      = nullptr;
        endReached            = false;
        flagReset             = true;
        gapTicksRemaining     = 0;
        elementTicksRemaining = 0;
        pendingGapTicks       = 0;
        pendingBlank          = false;
        dotOutputHigh         = false;
        dashOutputHigh        = false;
        morseOutputHigh       = false;
        outputEndTime         = engineTime;
        {
            std::lock_guard<std::mutex> lock(displayMutex);
            displayChar = ' ';
            memset(displayMorsePattern, 0, sizeof(displayMorsePattern));
            displayMorsePos = 0;
        }
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
        {
            std::lock_guard<std::mutex> lock(textMutex);
            strncpy(textBuffer, savedSlots[slot], MAX_CHARS);
            textBuffer[MAX_CHARS] = 0;
        }
        updateBufEmpty();
        resetPos();
    }

    void saveSlot(int slot) {
        if (slot < 0 || slot >= NUM_SLOTS) return;
        std::lock_guard<std::mutex> lock(textMutex);
        strncpy(savedSlots[slot], textBuffer, MAX_CHARS);
        savedSlots[slot][MAX_CHARS] = 0;
    }

    void onReset() override {
        {
            std::lock_guard<std::mutex> lock(textMutex);
            memset(textBuffer, 0, sizeof(textBuffer));
        }
        bufferLoop    = true;
        letterSpacing = true;
        wordSpacing   = true;
        dahDitWeight  = false;
        pulsewidth    = false;
        currentMode   = 0;
        bufEmpty.store(true, std::memory_order_release);
        resetPos();
    }

    void loadRandomQuote() {
        int idx;
        do {
            idx = (int)(rand() % NUM_QUOTES);
        } while (idx == lastQuoteIndex && NUM_QUOTES > 1);
        lastQuoteIndex = idx;
        {
            std::lock_guard<std::mutex> lock(textMutex);
            strncpy(textBuffer, QUOTES[idx], MAX_CHARS);
            textBuffer[MAX_CHARS] = 0;
        }
        updateBufEmpty();
        resetPos();
    }

    void process(const ProcessArgs& args) override {
        engineTime  += args.sampleTime;
        // noClockTimer is atomic<float> — use relaxed ops (only UI reads, no sync needed)
        noClockTimer.store(
            noClockTimer.load(std::memory_order_relaxed) + (float)args.sampleTime,
            std::memory_order_relaxed);

        // ── Mode switch ───────────────────────────────────────
        bool switchHigh   = params[SWITCH_PARAM].getValue() > 0.5f;
        bool startStopCV  = inputs[STARTSTOP_INPUT].getVoltage() > 1.0f;
        bool effectiveSwitch = switchHigh || startStopCV;

        if (effectiveSwitch && !prevSwitchHigh) {
            // Enter playback mode regardless of buffer state
            currentMode = 1;
            resetPos();
            initClock    = true;
            noClockTimer.store(0.0f, std::memory_order_relaxed);
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
            noClockTimer.store(0.0f, std::memory_order_relaxed);

            if (initClock) {
                lastClockTime = engineTime;
                clockPeriod   = MAX_CLOCK_PERIOD;
                lastDuration  = MAX_CLOCK_PERIOD;
                initClock     = false;
                flagReset     = false;
            } else {
                double measured = engineTime - lastClockTime;
                lastClockTime   = engineTime;
                if (measured > 0.005 && measured < 5.0) {
                    clockPeriod  = measured;
                    lastDuration = measured;
                }
            }

            if (!endReached && !flagReset) {
                // ── State machine: clock-tick based element timing ─────
                //
                // Each clock rising edge = 1 dit unit (T).
                // Dit  = 1 tick.  Dah (3:1) = 3 ticks.  Dah (2:1) = 2 ticks.
                // Gate mode:    output HIGH for 50% of element window (0.5 × N × T).
                // Trigger mode: output HIGH for MIN_PULSE_WIDTH; still blocks N ticks.
                // Inter-element gap: 1 tick.  Letter gap: 3 ticks.  Word gap: 7 ticks.
                //
                // Thread safety: display vars are written to local working copies here
                // (audio thread is their sole writer, so reading them is safe without a
                // lock). They are published to the shared displayMutex-protected vars
                // once at the end of this block so the UI thread always sees a consistent
                // snapshot. textBuffer is only accessed in the "advance" branch under
                // try_lock(textMutex) — if the UI thread is writing, we skip that tick.

                // Working copies of display state for this tick
                char   wDispChar = displayChar;     // audio-thread-local read is safe
                char   wDispPattern[33];
                memcpy(wDispPattern, displayMorsePattern, sizeof(wDispPattern));
                int    wDispPos     = displayMorsePos;
                bool   wDispChanged = false;

                if (elementTicksRemaining > 0) {
                    // Hold: still within current element's tick window — don't advance
                    elementTicksRemaining--;
                    if (elementTicksRemaining == 0 && pendingGapTicks > 0) {
                        gapTicksRemaining = pendingGapTicks;
                        pendingGapTicks   = 0;
                        // pendingBlank deferred to first gap tick so display stays up
                        // while output is still active.
                    }
                } else if (gapTicksRemaining > 0) {
                    // Silent gap tick (inter-element, letter, or word spacing)
                    gapTicksRemaining--;
                    dotOutputHigh   = false;
                    dashOutputHigh  = false;
                    morseOutputHigh = false;
                    if (pendingBlank) {
                        wDispChar    = ' ';
                        memset(wDispPattern, 0, sizeof(wDispPattern));
                        wDispPos     = 0;
                        wDispChanged = true;
                        pendingBlank = false;
                    }
                } else {
                    // Advance state machine — needs textBuffer access.
                    // Use try_lock: if UI is writing we skip this tick rather than block.
                    if (textMutex.try_lock()) {
                        while (currentMorseIndex < MAX_CHARS) {
                            char c = textBuffer[currentMorseIndex];

                            if (c == 0) {
                                if (bufferLoop) {
                                    currentMorseIndex = 0;
                                    charPulseCount    = 0;
                                    currentMorseCode  = nullptr;
                                    wDispChar    = ' ';
                                    memset(wDispPattern, 0, sizeof(wDispPattern));
                                    wDispPos     = 0;
                                    wDispChanged = true;
                                    c = textBuffer[0];
                                    if (c == 0) { endReached = true; break; }
                                } else {
                                    endReached = true;
                                    break;
                                }
                            }

                            if (c == ' ' || c == '\n') {
                                incrementIndex();
                                dotOutputHigh   = false;
                                dashOutputHigh  = false;
                                morseOutputHigh = false;
                                if (wordSpacing) {
                                    gapTicksRemaining = 6;
                                    wDispChar    = ' ';
                                    memset(wDispPattern, 0, sizeof(wDispPattern));
                                    wDispPos     = 0;
                                    wDispChanged = true;
                                } else {
                                    gapTicksRemaining = 2;
                                }
                                break;
                            }

                            const char* morse = getMorse(c);
                            if (!morse) { incrementIndex(); continue; }

                            if (currentMorseCode != morse) {
                                currentMorseCode = morse;
                                charPulseCount   = 0;
                                wDispChar        = (char)toupper((unsigned char)c);
                                memset(wDispPattern, 0, sizeof(wDispPattern));
                                wDispPos         = 0;
                                wDispChanged     = true;
                            }

                            if (charPulseCount < (int)strlen(morse)) {
                                bool dot = (morse[charPulseCount] == '.');
                                isDot    = dot;

                                int numTicks = dot ? 1 : (dahDitWeight ? 2 : 3);
                                elementTicksRemaining = numTicks - 1;

                                double pulseDur;
                                if (pulsewidth) {
                                    pulseDur = MIN_PULSE_WIDTH;
                                } else {
                                    pulseDur = std::max(clockPeriod * numTicks * 0.5,
                                                        MIN_PULSE_WIDTH);
                                }

                                outputEndTime   = engineTime + pulseDur;
                                dotOutputHigh   = dot;
                                dashOutputHigh  = !dot;
                                morseOutputHigh = true;

                                if (wDispPos < 31) {
                                    wDispPattern[wDispPos++] = dot ? '.' : '_';
                                    wDispPattern[wDispPos]   = 0;
                                }
                                wDispChanged = true;

                                charPulseCount++;
                                bool isLastElement = (charPulseCount >= (int)strlen(morse));

                                if (isLastElement) {
                                    incrementIndex();
                                    pendingGapTicks = letterSpacing ? 3 : 1;
                                    pendingBlank    = letterSpacing;
                                } else {
                                    pendingGapTicks = 1;
                                    pendingBlank    = false;
                                }

                                if (elementTicksRemaining == 0) {
                                    gapTicksRemaining = pendingGapTicks;
                                    pendingGapTicks   = 0;
                                }
                            } else {
                                incrementIndex();
                            }
                            break;
                        }
                        textMutex.unlock();
                    }
                    // try_lock failed → skip this tick, keep current state
                }

                // Publish display state once under displayMutex
                if (wDispChanged) {
                    std::lock_guard<std::mutex> dlock(displayMutex);
                    displayChar = wDispChar;
                    memcpy(displayMorsePattern, wDispPattern, sizeof(displayMorsePattern));
                    displayMorsePos = wDispPos;
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
        {
            std::lock_guard<std::mutex> lock(textMutex);
            json_object_set_new(rootJ, "textBuffer", json_string(textBuffer));
        }
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
        if (j) {
            std::lock_guard<std::mutex> lock(textMutex);
            strncpy(textBuffer, json_string_value(j), MAX_CHARS);
            textBuffer[MAX_CHARS] = 0;
        }

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
        updateBufEmpty();
    }
};

// ============================================================
// Display Widget — double-click to type
// ============================================================
struct DitDitDisplay : LedDisplay {
    DitDit* module;
    bool  editing          = false;
    bool  clearOnFirstChar = false;
    bool  blinkOn          = true;
    float blinkTimer       = 0.f;
    float timeSinceLastClick = 999.f;
    int   cursorPos        = 0;   // insertion point within textBuffer
    int   scrollLine       = 0;   // first visible line

    static const int CHARS_PER_LINE = 15;
    static const int NUM_LINES      = 4;

    void updateScroll() {
        int cursorRow = cursorPos / CHARS_PER_LINE;
        if (cursorRow < scrollLine)                    scrollLine = cursorRow;
        if (cursorRow >= scrollLine + NUM_LINES)       scrollLine = cursorRow - NUM_LINES + 1;
        if (scrollLine < 0) scrollLine = 0;
    }

    // ── Event handling ────────────────────────────────────────

    void onHover(const HoverEvent& e) override { e.consume(this); }

    void onButton(const ButtonEvent& e) override {
        if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT) {
            if (timeSinceLastClick < 0.4f && module && module->currentMode == 0) {
                editing          = true;
                clearOnFirstChar = module->isBufferEmpty();
                cursorPos        = (int)strlen(module->textBuffer);
                updateScroll();
                APP->event->setSelectedWidget(this);
            }
            timeSinceLastClick = 0.f;
            e.consume(this);
        }
    }

    void onDeselect(const DeselectEvent& e) override {
        editing = false;
    }

    // Insert a single character at cursorPos (shared by text and key handlers)
    void insertChar(char c) {
        if (!module) return;
        {
            std::lock_guard<std::mutex> lock(module->textMutex);
            if (clearOnFirstChar) {
                memset(module->textBuffer, 0, MAX_CHARS + 1);
                cursorPos        = 0;
                clearOnFirstChar = false;
            }
            int len = (int)strlen(module->textBuffer);
            if (len < MAX_CHARS) {
                for (int i = len; i > cursorPos; i--)
                    module->textBuffer[i] = module->textBuffer[i - 1];
                module->textBuffer[cursorPos] = c;
                module->textBuffer[len + 1]   = 0;
                cursorPos++;
            }
        }
        module->updateBufEmpty();
        updateScroll();
    }

    void onSelectText(const SelectTextEvent& e) override {
        if (!editing || !module) { e.consume(this); return; }
        char c = (char)toupper((unsigned char)e.codepoint);
        if (getMorse(c) != nullptr)
            insertChar(c);
        e.consume(this);
    }

    void onSelectKey(const SelectKeyEvent& e) override {
        if (!editing || !module) { e.consume(this); return; }
        if (e.action == GLFW_PRESS || e.action == GLFW_REPEAT) {
            int len = (int)strlen(module->textBuffer);
            switch (e.key) {
                case GLFW_KEY_ESCAPE:
                    {
                        std::lock_guard<std::mutex> lock(module->textMutex);
                        memset(module->textBuffer, 0, MAX_CHARS + 1);
                    }
                    module->updateBufEmpty();
                    cursorPos = 0;
                    updateScroll();
                    editing = false;
                    APP->event->setSelectedWidget(nullptr);
                    break;
                case GLFW_KEY_ENTER:
                case GLFW_KEY_KP_ENTER:
                    editing = false;
                    APP->event->setSelectedWidget(nullptr);
                    break;
                case GLFW_KEY_SPACE:
                    insertChar(' ');
                    break;
                case GLFW_KEY_TAB:
                    for (int i = 0; i < 4; i++) insertChar(' ');
                    break;
                case GLFW_KEY_BACKSPACE:
                    clearOnFirstChar = false;
                    if (cursorPos > 0) {
                        {
                            std::lock_guard<std::mutex> lock(module->textMutex);
                            for (int i = cursorPos - 1; i < len - 1; i++)
                                module->textBuffer[i] = module->textBuffer[i + 1];
                            module->textBuffer[len - 1] = 0;
                        }
                        module->updateBufEmpty();
                        cursorPos--;
                        updateScroll();
                    }
                    break;
                case GLFW_KEY_DELETE:
                    clearOnFirstChar = false;
                    if (cursorPos < len) {
                        {
                            std::lock_guard<std::mutex> lock(module->textMutex);
                            for (int i = cursorPos; i < len - 1; i++)
                                module->textBuffer[i] = module->textBuffer[i + 1];
                            module->textBuffer[len - 1] = 0;
                        }
                        module->updateBufEmpty();
                        updateScroll();
                    }
                    break;
                case GLFW_KEY_LEFT:
                    clearOnFirstChar = false;
                    if (cursorPos > 0) { cursorPos--; updateScroll(); }
                    break;
                case GLFW_KEY_RIGHT:
                    clearOnFirstChar = false;
                    if (cursorPos < len) { cursorPos++; updateScroll(); }
                    break;
                case GLFW_KEY_UP:
                    clearOnFirstChar = false;
                    cursorPos = std::max(0, cursorPos - CHARS_PER_LINE);
                    updateScroll();
                    break;
                case GLFW_KEY_DOWN:
                    clearOnFirstChar = false;
                    cursorPos = std::min(len, cursorPos + CHARS_PER_LINE);
                    updateScroll();
                    break;
                case GLFW_KEY_HOME:
                    clearOnFirstChar = false;
                    cursorPos = 0;
                    updateScroll();
                    break;
                case GLFW_KEY_END:
                    clearOnFirstChar = false;
                    cursorPos = len;
                    updateScroll();
                    break;
                default: break;
            }
        }
        e.consume(this);
    }

    void step() override {
        LedDisplay::step();
        float dt = APP->window->getLastFrameDuration();
        timeSinceLastClick += dt;
        // Drive blink from wall clock so it always toggles reliably
        blinkOn = (std::fmod(glfwGetTime(), 1.0) < 0.5);
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
            bool empty = module->bufEmpty.load(std::memory_order_acquire);

            if (empty && !editing) {
                // Placeholder hint
                nvgFontSize(args.vg, 5.f);
                nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
                nvgFillColor(args.vg, oledDim);
                nvgText(args.vg, cx, cy - 5, "DOUBLE-CLICK", nullptr);
                nvgText(args.vg, cx, cy + 3, "TO TYPE", nullptr);
            } else {
                // Snapshot textBuffer under lock so UI reads are consistent
                char bufSnap[MAX_CHARS + 1];
                {
                    std::lock_guard<std::mutex> tlock(module->textMutex);
                    memcpy(bufSnap, module->textBuffer, MAX_CHARS + 1);
                }

                // Show text buffer in 4 lines of 15 chars
                nvgFontSize(args.vg, 5.f);
                nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);
                nvgFillColor(args.vg, oled);

                char drawnLines[NUM_LINES][CHARS_PER_LINE + 2] = {};
                for (int line = 0; line < NUM_LINES; line++) {
                    int absLine  = line + scrollLine;
                    int startIdx = absLine * CHARS_PER_LINE;
                    for (int col = 0; col < CHARS_PER_LINE; col++) {
                        int idx = startIdx + col;
                        char c = (idx < MAX_CHARS) ? bufSnap[idx] : 0;
                        drawnLines[line][col] = (c != 0 && c != '\n') ? (char)toupper((unsigned char)c) : ' ';
                    }
                    drawnLines[line][CHARS_PER_LINE] = 0;
                    nvgText(args.vg, 2, 7 + line * 8, drawnLines[line], nullptr);
                }

                // Blinking cursor — exact pixel position via nvgTextBounds (terminal style)
                if (editing && blinkOn) {
                    int absRow    = cursorPos / CHARS_PER_LINE;
                    int cursorCol = cursorPos % CHARS_PER_LINE;
                    int visRow    = absRow - scrollLine;

                    if (visRow >= 0 && visRow < NUM_LINES) {
                        char prefix[CHARS_PER_LINE + 2] = {};
                        int  rowStart = absRow * CHARS_PER_LINE;
                        for (int i = 0; i < cursorCol; i++) {
                            char c = bufSnap[rowStart + i];
                            prefix[i] = c ? (char)toupper((unsigned char)c) : ' ';
                        }
                        prefix[cursorCol] = 0;

                        float bounds[4] = {};
                        nvgTextBounds(args.vg, 2.f, 7.f + visRow * 8.f,
                                      prefix, nullptr, bounds);
                        float cursorX = bounds[2];
                        float cursorY = 7.f + visRow * 8.f;

                        char nextCh[2] = {0, 0};
                        int  len2 = (int)strlen(bufSnap);
                        nextCh[0] = (cursorPos < len2)
                                    ? (char)toupper((unsigned char)bufSnap[cursorPos])
                                    : ' ';
                        float nb[4] = {};
                        nvgTextBounds(args.vg, cursorX, cursorY, nextCh, nullptr, nb);
                        float charW = nb[2] - nb[0];
                        if (charW < 1.f) charW = 3.5f;

                        nvgBeginPath(args.vg);
                        nvgRect(args.vg, cursorX, cursorY + 1.f, charW, 1.f);
                        nvgFillColor(args.vg, oled);
                        nvgFill(args.vg);
                    }
                }
            }

        } else {
            // ── Playback mode ─────────────────────────────────
            nvgFillColor(args.vg, oled);

            // Status messages when something is missing
            {
                bool bangConnected = module->inputs[DitDit::BANG_INPUT].isConnected();
                bool isEmpty       = module->bufEmpty.load(std::memory_order_acquire);
                const char* msg    = nullptr;
                if (isEmpty)               msg = "NO TEXT INPUT";
                else if (!bangConnected)   msg = "NO CLOCK INPUT";

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

            // Take a snapshot of audio-written display vars under the display mutex
            char snapChar;
            char snapPattern[33];
            bool snapEndReached;
            // noClockTimer is atomic<float> — read without holding displayMutex
            float snapNoClockTimer = module->noClockTimer.load(std::memory_order_relaxed);
            {
                std::lock_guard<std::mutex> dlock(module->displayMutex);
                snapChar       = module->displayChar;
                memcpy(snapPattern, module->displayMorsePattern, sizeof(snapPattern));
                snapEndReached = module->endReached;
            }

            // Current character — centered, upper area
            nvgFontSize(args.vg, 12.f);
            nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
            char charStr[2] = {snapChar, 0};
            nvgText(args.vg, cx, 10.f, charStr, nullptr);

            // Morse pattern — space after each dash, centered below
            char spaced[64] = {};
            int sp = 0;
            for (int i = 0; snapPattern[i] && sp < 61; i++) {
                char el = snapPattern[i];
                spaced[sp++] = el;
                if (el == '_') spaced[sp++] = ' ';
            }
            spaced[sp] = 0;
            nvgFontSize(args.vg, 11.f);
            nvgText(args.vg, cx, 19.f, spaced, nullptr);

            nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);
            if (snapEndReached) {
                nvgFontSize(args.vg, 5.f);
                nvgFillColor(args.vg, orange);
                nvgText(args.vg, 2, 36.f, "END", nullptr);
            }
            if (snapNoClockTimer > 7.0) {
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
    DitDitDisplay* displayWidget = nullptr;

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
        displayWidget     = display;
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

    void onHoverKey(const HoverKeyEvent& e) override {
        if (e.action == GLFW_PRESS) {
            DitDit* m = dynamic_cast<DitDit*>(this->module);
            // Only intercept when the text editor is not open
            if (m && !(displayWidget && displayWidget->editing)) {
                int slot = -1;
                if (e.key >= GLFW_KEY_1     && e.key <= GLFW_KEY_8)     slot = e.key - GLFW_KEY_1;
                if (e.key >= GLFW_KEY_KP_1  && e.key <= GLFW_KEY_KP_8)  slot = e.key - GLFW_KEY_KP_1;
                if (slot >= 0 && m->savedSlots[slot][0] != 0) {
                    m->loadSlot(slot);
                    e.consume(this);
                    return;
                }
            }
        }
        ModuleWidget::onHoverKey(e);
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
            {
                std::lock_guard<std::mutex> lock(module->textMutex);
                memset(module->textBuffer, 0, sizeof(module->textBuffer));
            }
            module->bufEmpty.store(true, std::memory_order_release);
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
