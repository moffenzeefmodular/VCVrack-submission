#include "plugin.hpp"
#include <cstring>

// stb_vorbis OGG decoder (compiled as C in stb_vorbis.c)
extern "C" {
    int stb_vorbis_decode_filename(const char *filename, int *channels, int *sample_rate, short **output);
}

// Base64 encode/decode for saving audio buffer content in patch JSON
static const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static std::string b64Encode(const void* data, size_t len) {
    const uint8_t* b = (const uint8_t*)data;
    std::string out;
    out.reserve(((len + 2) / 3) * 4);
    for (size_t i = 0; i < len; i += 3) {
        uint32_t v = (uint32_t)b[i] << 16;
        if (i + 1 < len) v |= (uint32_t)b[i+1] << 8;
        if (i + 2 < len) v |= (uint32_t)b[i+2];
        out += b64chars[(v >> 18) & 0x3f];
        out += b64chars[(v >> 12) & 0x3f];
        out += (i + 1 < len) ? b64chars[(v >> 6) & 0x3f] : '=';
        out += (i + 2 < len) ? b64chars[(v >> 0) & 0x3f] : '=';
    }
    return out;
}
static std::vector<uint8_t> b64Decode(const char* s, size_t slen) {
    static const int8_t T[128] = {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
        52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
        15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
        -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
    };
    std::vector<uint8_t> out;
    out.reserve(slen * 3 / 4);
    for (size_t i = 0; i + 3 < slen; i += 4) {
        uint8_t a = (uint8_t)s[i], b = (uint8_t)s[i+1], c = (uint8_t)s[i+2], d = (uint8_t)s[i+3];
        if (a >= 128 || b >= 128) break;
        uint32_t v = ((uint32_t)T[a] << 18) | ((uint32_t)T[b] << 12)
                   | ((c != '=' && c < 128) ? (uint32_t)T[c] << 6 : 0)
                   | ((d != '=' && d < 128) ? (uint32_t)T[d]       : 0);
        out.push_back((v >> 16) & 0xff);
        if (c != '=') out.push_back((v >> 8) & 0xff);
        if (d != '=') out.push_back(v & 0xff);
    }
    return out;
}

struct MediaSwitchQuantity : SwitchQuantity {
    std::string getString() override {
        int i = (int)std::round(getValue());
        i = clamp(i, 0, (int)labels.size() - 1);
        return labels[i];
    }
};

struct Tehom : Module {
    enum ParamId {
        WARBLE_PARAM,
        MEDIA_PARAM,
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
        MEDIACVIN_INPUT,
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

    configSwitch<MediaSwitchQuantity>(MEDIA_PARAM, 0.f, 7.f, 0.f, "Type", {"Mic Preamp", "Reel To Reel", "Cassette", "VHS", "Vinyl Clean", "Vinyl Dirty", "8mm Film", "16mm Film"});

	configParam(SLEW_PARAM, 0.02f, 1.f, 0.02f, "Slew", "ms", 0.f, 1000.f);

	configParam(LEDBEZEL1_PARAM, 0.f, 1.f, 0.f);
	configParam(LEDBEZEL2_PARAM, 0.f, 1.f, 0.f);
	configParam(LEDBEZEL3_PARAM, 0.f, 1.f, 0.f);
	configParam(LEDBEZEL4_PARAM, 0.f, 1.f, 0.f);
	// LEDBezel spin position should not change on Randomize or Init
	for (int i = 0; i < 4; i++)
		paramQuantities[LEDBEZEL1_PARAM + i]->randomizeEnabled = false;

	for (int i = 0; i < 4; i++) bezelDragging[i].store(false);

	configParam(WARBLE_PARAM, 0.f, 1.f, 0.f, "Warble", "%", 0.f, 100.f);
	configParam(AMOUNT_PARAM, 0.f, 1.f, 0.f, "Noise Amount", "%", 0.f, 100.f);
    configParam(SIZE_PARAM, 0.f, 1.f, 1.f, "Loop Size", "%", 0.f, 100.f);
    configParam(POSITION_PARAM, 0.f, 1.f, 0.f, "Loop Position", "%", 0.f, 100.f);
    configParam(XFADE_PARAM, 0.f, 1.f, 0.f, "Crossfade", "%", 0.f, 100.f);
	
	// Source params
	configParam(SOURCE1_PARAM, -1.f, 1.f, 0.f, "Source", "%", 0.f, 100.f);
	configParam(SOURCE2_PARAM, -1.f, 1.f, 0.f, "Source", "%", 0.f, 100.f);
	configParam(SOURCE3_PARAM, -1.f, 1.f, 0.f, "Source", "%", 0.f, 100.f);
	configParam(SOURCE4_PARAM, -1.f, 1.f, 0.f, "Source", "%", 0.f, 100.f);

	// Pitch params
	configParam(SPEED1_PARAM, 0.f, 1.f, 0.5f, "Speed", "x", 0.f, 2.f);
	configParam(SPEED2_PARAM, 0.f, 1.f, 0.5f, "Speed", "x", 0.f, 2.f);
	configParam(SPEED3_PARAM, 0.f, 1.f, 0.5f, "Speed", "x", 0.f, 2.f);
	configParam(SPEED4_PARAM, 0.f, 1.f, 0.5f, "Speed", "x", 0.f, 2.f);

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
    configInput(MEDIACVIN_INPUT, "Media Select CV");
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

    loadOggFiles();
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
    // Apply any buffer data deferred from dataFromJson (runs before sample rate is known)
    for (int i = 0; i < 4; i++) {
        if (!pendingBuf[i].pending) continue;
        int len = std::min(pendingBuf[i].len, bufferSize);
        if (len > 0 && (int)pendingBuf[i].L.size() >= len && (int)pendingBuf[i].R.size() >= len) {
            std::copy(pendingBuf[i].L.begin(), pendingBuf[i].L.begin() + len, bufL[i].begin());
            std::copy(pendingBuf[i].R.begin(), pendingBuf[i].R.begin() + len, bufR[i].begin());
            recordedLength[i] = len;
            hasContent[i]     = pendingBuf[i].hasContent;
            playState[i]      = pendingBuf[i].playState;
            playReversed[i]   = pendingBuf[i].playReversed;
            readPos[i]        = clamp(pendingBuf[i].readPos, 0.f, (float)(len - 1));
        }
        pendingBuf[i] = PendingBuf{};  // clear
    }
}

void onReset(const ResetEvent& e) override {
    // Preserve bezel spin positions — they should not reset on Init
    float bezelPos[4];
    for (int i = 0; i < 4; i++)
        bezelPos[i] = params[LEDBEZEL1_PARAM + i].getValue();
    Module::onReset(e);
    for (int i = 0; i < 4; i++)
        params[LEDBEZEL1_PARAM + i].setValue(bezelPos[i]);
    bufferDuration = 2.f;
    if (currentSampleRate > 0.f) resizeBuffers(currentSampleRate);
    for (int i = 0; i < 4; i++) {
        eraseBuffer(i);
        autoPlay[i]         = true;
        autoPlayFull[i]     = true;
        playCVMode[i]       = 0;
        playReversed[i]     = false;
        continuousRecord[i] = false;
    }
    xyFinalX = 0.5f;
    xyFinalY = 0.5f;
    showCrosshairs    = false;
    persist           = true;
    bgScrollSpeed     = 2;
    bgScrollRight     = true;
    noiseAuxPreFader  = true;
    oggPlayPos        = 0.f;
    currentOggIdx     = -1;
}

json_t* dataToJson() override {
    json_t* root = json_object();

    // Settings
    json_object_set_new(root, "bufferDuration",   json_real(bufferDuration));
    json_object_set_new(root, "showCrosshairs",   json_boolean(showCrosshairs));
    json_object_set_new(root, "persist",          json_boolean(persist));
    json_object_set_new(root, "bgScrollSpeed",    json_integer(bgScrollSpeed));
    json_object_set_new(root, "bgScrollRight",    json_boolean(bgScrollRight));
    json_object_set_new(root, "noiseAuxPreFader", json_boolean(noiseAuxPreFader));

    // Per-channel toggles
    json_t* ap = json_array(), *apf = json_array(), *pcvm = json_array(), *cr = json_array();
    for (int i = 0; i < 4; i++) {
        json_array_append_new(ap,   json_boolean(autoPlay[i]));
        json_array_append_new(apf,  json_boolean(autoPlayFull[i]));
        json_array_append_new(pcvm, json_integer(playCVMode[i]));
        json_array_append_new(cr,   json_boolean(continuousRecord[i]));
    }
    json_object_set_new(root, "autoPlay",         ap);
    json_object_set_new(root, "autoPlayFull",     apf);
    json_object_set_new(root, "playCVMode",       pcvm);
    json_object_set_new(root, "continuousRecord", cr);

    // Audio buffers
    json_t* bufs = json_array();
    for (int i = 0; i < 4; i++) {
        json_t* ch = json_object();
        json_object_set_new(ch, "hasContent",     json_boolean(hasContent[i]));
        json_object_set_new(ch, "recordedLength", json_integer(recordedLength[i]));
        json_object_set_new(ch, "playState",      json_boolean(playState[i]));
        json_object_set_new(ch, "playReversed",   json_boolean(playReversed[i]));
        json_object_set_new(ch, "readPos",        json_real(readPos[i]));
        if (hasContent[i] && recordedLength[i] > 0) {
            int len = recordedLength[i];
            json_object_set_new(ch, "bufL", json_string(b64Encode(bufL[i].data(), len * sizeof(float)).c_str()));
            json_object_set_new(ch, "bufR", json_string(b64Encode(bufR[i].data(), len * sizeof(float)).c_str()));
        }
        json_array_append_new(bufs, ch);
    }
    json_object_set_new(root, "buffers", bufs);

    return root;
}

void dataFromJson(json_t* root) override {
    // Settings
    json_t* bd = json_object_get(root, "bufferDuration");
    if (bd) { bufferDuration = (float)json_real_value(bd); if (currentSampleRate > 0.f) resizeBuffers(currentSampleRate); }
    json_t* sc = json_object_get(root, "showCrosshairs");
    if (sc) showCrosshairs = json_boolean_value(sc);
    json_t* ps = json_object_get(root, "persist");
    if (ps) persist = json_boolean_value(ps);
    json_t* bss = json_object_get(root, "bgScrollSpeed");
    if (bss) bgScrollSpeed = (int)json_integer_value(bss);
    json_t* bsr = json_object_get(root, "bgScrollRight");
    if (bsr) bgScrollRight = json_boolean_value(bsr);
    json_t* napf = json_object_get(root, "noiseAuxPreFader");
    if (napf) noiseAuxPreFader = json_boolean_value(napf);

    // Per-channel toggles
    json_t* ap = json_object_get(root, "autoPlay");
    if (ap)   for (int i = 0; i < 4; i++) { json_t* v = json_array_get(ap,   i); if (v) autoPlay[i]    = json_boolean_value(v); }
    json_t* apf = json_object_get(root, "autoPlayFull");
    if (apf)  for (int i = 0; i < 4; i++) { json_t* v = json_array_get(apf,  i); if (v) autoPlayFull[i] = json_boolean_value(v); }
    json_t* pcvm = json_object_get(root, "playCVMode");
    if (pcvm) for (int i = 0; i < 4; i++) { json_t* v = json_array_get(pcvm, i); if (v) playCVMode[i]   = (int)json_integer_value(v); }
    json_t* crj = json_object_get(root, "continuousRecord");
    if (crj)  for (int i = 0; i < 4; i++) { json_t* v = json_array_get(crj,  i); if (v) continuousRecord[i] = json_boolean_value(v); }

    // Audio buffers
    json_t* bufs = json_object_get(root, "buffers");
    if (bufs) {
        for (int i = 0; i < 4; i++) {
            json_t* ch = json_array_get(bufs, i);
            if (!ch) continue;
            json_t* v;
            bool hc  = false; v = json_object_get(ch, "hasContent");     if (v) hc = json_boolean_value(v);
            int  len = 0;     v = json_object_get(ch, "recordedLength"); if (v) len = (int)json_integer_value(v);
            bool ps2 = false; v = json_object_get(ch, "playState");      if (v) ps2 = json_boolean_value(v);
            bool pr  = false; v = json_object_get(ch, "playReversed");   if (v) pr  = json_boolean_value(v);
            float rp = 0.f;  v = json_object_get(ch, "readPos");        if (v) rp  = (float)json_real_value(v);

            json_t* jbl = json_object_get(ch, "bufL");
            json_t* jbr = json_object_get(ch, "bufR");

            if (!hc || len <= 0 || !jbl || !jbr) continue;

            auto decL = b64Decode(json_string_value(jbl), strlen(json_string_value(jbl)));
            auto decR = b64Decode(json_string_value(jbr), strlen(json_string_value(jbr)));
            int floatBytes = len * (int)sizeof(float);

            if ((int)decL.size() < floatBytes || (int)decR.size() < floatBytes) continue;

            std::vector<float> fL(len), fR(len);
            memcpy(fL.data(), decL.data(), floatBytes);
            memcpy(fR.data(), decR.data(), floatBytes);

            // Always defer: onSampleRateChange fires after dataFromJson and would
            // wipe a direct copy via resizeBuffers. Apply in onSampleRateChange instead.
            pendingBuf[i].L            = std::move(fL);
            pendingBuf[i].R            = std::move(fR);
            pendingBuf[i].len          = len;
            pendingBuf[i].hasContent   = hc;
            pendingBuf[i].playState    = ps2;
            pendingBuf[i].playReversed = pr;
            pendingBuf[i].readPos      = rp;
            pendingBuf[i].pending      = true;
        }
    }
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
bool autoPlay[4]          = {true, true, true, true};
bool autoPlayFull[4]      = {true, true, true, true};
bool continuousRecord[4]  = {false, false, false, false};

// XY Pad display settings (UI state, reset on Init)
bool showCrosshairs = false;
bool persist        = true;

// Background scroll speed: 0=Off, 1=Slow, 2=Medium, 3=Fast
int  bgScrollSpeed = 2;
bool bgScrollRight = true;
int playCVMode[4] = {0, 0, 0, 0}; // 0=Play/Stop, 1=Retrigger, 2=Forward/Reverse

std::atomic<bool> xyDragging{false};
float xyFinalX = 0.5f;
float xyFinalY = 0.5f;

// Warble LFO phases (per channel, advance while playing)
float wowPhase[4]     = {};
float flutterPhase[4] = {};

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

// Deferred buffer data loaded from JSON — applied once sample rate is known
struct PendingBuf {
    std::vector<float> L, R;
    int len = 0;
    bool hasContent = false;
    bool playState = false;
    bool playReversed = false;
    float readPos = 0.f;
    bool pending = false;
} pendingBuf[4];

// OGG media loops
struct OggLoop {
    std::vector<float> L, R;
    int numSamples = 0;
    int nativeSampleRate = 44100;
    bool loaded = false;
} oggLoops[8];

float    oggPlayPos    = 0.f;
int      currentOggIdx = -1;
bool     noiseAuxPreFader = true;

const char* const oggFilenames[8] = {
    "res/audio/MicPre.ogg",
    "res/audio/ReelToReel.ogg",
    "res/audio/Cassette.ogg",
    "res/audio/VHS.ogg",
    "res/audio/VinylClean.ogg",
    "res/audio/VinylDirty.ogg",
    "res/audio/Film8mm.ogg",
    "res/audio/Film16mm.ogg",
};

void loadOggFiles() {
    for (int i = 0; i < 8; i++) {
        std::string path = asset::plugin(pluginInstance, oggFilenames[i]);
        int ch = 0, sr = 0;
        short* data = nullptr;
        int n = stb_vorbis_decode_filename(path.c_str(), &ch, &sr, &data);
        if (n <= 0 || !data) continue;
        oggLoops[i].L.resize(n);
        oggLoops[i].R.resize(n);
        oggLoops[i].nativeSampleRate = sr;
        oggLoops[i].numSamples = n;
        for (int s = 0; s < n; s++) {
            oggLoops[i].L[s] = data[s * ch + 0] / 32768.f;
            oggLoops[i].R[s] = (ch >= 2) ? data[s * ch + 1] / 32768.f : oggLoops[i].L[s];
        }
        free(data);
        oggLoops[i].loaded = true;
    }
}

void processBypass(const ProcessArgs& args) override {
    float inL = inputs[AUDIOLEFTIN_INPUT].getVoltage();
    float inR = inputs[AUDIORIGHTIN_INPUT].isConnected() ? inputs[AUDIORIGHTIN_INPUT].getVoltage() : inL;
    outputs[AUDIOLEFTOUT_OUTPUT].setVoltage(inL);
    outputs[AUDIORIGHTOUT_OUTPUT].setVoltage(inR);
}

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

        if (cvRising) {
            if (playCVMode[i] == 1) {
                // Retrigger: reset playhead to start (or end if reversed), ensure playing
                playState[i] = true;
                readPos[i] = playReversed[i] ? (float)(std::max(1, recordedLength[i]) - 1) : 0.f;
            } else if (playCVMode[i] == 2) {
                // Forward/Reverse toggle
                playReversed[i] = !playReversed[i];
            } else {
                // Play/Stop toggle (default)
                playState[i] = !playState[i];
                if (playState[i])
                    readPos[i] = playReversed[i] ? (float)(std::max(1, recordedLength[i]) - 1) : 0.f;
            }
        }
        if (btnRising) {
            playState[i] = !playState[i];
            if (playState[i])
                readPos[i] = playReversed[i] ? (float)(std::max(1, recordedLength[i]) - 1) : 0.f;
        }

        lights[PLAY1_LIGHT + i].setBrightnessSmooth((playState[i] && !playReversed[i]) ? 1.f : 0.f, args.sampleTime);
        lights[PLAY1_BLUE_LIGHT + i].setBrightnessSmooth((playState[i] && playReversed[i]) ? 1.f : 0.f, args.sampleTime);
    }

    // --- BEZEL SPINNING (pauses audio readPos when held) ---
    for (int i = 0; i < 4; i++) {
        if (playState[i] && !bezelDragging[i].load()) {
            float pitchVal = clamp(params[SPEED1_PARAM + i].getValue() + clamp(inputs[SPEED1CVIN_INPUT + i].getVoltage(), -5.f, 5.f) / 10.f, 0.025f, 1.f);
            float spinSpeed = 0.02f + pitchVal * 0.8f;
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

    // Warble (wow + flutter)
    float warble     = clamp(params[WARBLE_PARAM].getValue() + clamp(inputs[WARBLECVIN_INPUT].getVoltage(), -5.f, 5.f) / 10.f, 0.f, 1.f);
    float warbleFade = clamp(warble / 0.05f, 0.f, 1.f); // fade in over first 5% of knob
    float wowRate     = 0.5f + warble * 1.5f;  // 0.5–2 Hz
    float flutterRate = 5.f  + warble * 10.f;  // 5–15 Hz
    float wowDepth     = warbleFade * warble * warble * 0.042f;
    float flutterDepth = warbleFade * warble * warble * 0.011f;

    // Per-channel: read loop buffer (with pitch), source-crossfade with input for output + recording.
    // source left (t=0) = hear/record input; source right (t=1) = hear/record loop (sound on sound).
    // bilinear vol[] always sums to 1.0, so unity gain is preserved across XY positions.
    float outL = 0.f, outR = 0.f;
    float chanMixL[4] = {}, chanMixR[4] = {};

    // Global loop window params
    float sizeParam  = clamp(params[SIZE_PARAM].getValue() + clamp(inputs[SIZECVIN_INPUT].getVoltage(), -5.f, 5.f) / 10.f, 0.f, 1.f);
    float posParam   = clamp(params[POSITION_PARAM].getValue() + clamp(inputs[POSITIONCVIN_INPUT].getVoltage(), -5.f, 5.f) / 10.f, 0.f, 1.f);
    float xfadeParam = std::max(0.01f, clamp(params[XFADE_PARAM].getValue() + clamp(inputs[XFADECVIN_INPUT].getVoltage(), -5.f, 5.f) / 10.f, 0.f, 1.f));

    for (int i = 0; i < 4; i++) {
        float sampL = 0.f, sampR = 0.f;

        if (playState[i] && hasContent[i] && bufferSize > 0 && !bezelDragging[i].load()) {
            int len = recordedLength[i];
            if (len >= 2) {
                // Compute loop window (strictly within recorded content)
                int minLoopSize = std::min(100, len);
                int loopSize    = clamp((int)(sizeParam * sizeParam * sizeParam * (float)len), minLoopSize, len);
                int maxStart    = len - loopSize; // always >= 0 since loopSize <= len
                int loopStart   = clamp((int)(posParam * (float)maxStart), 0, maxStart);
                int loopEnd     = loopStart + loopSize; // always <= len, exact

                // Clamp readPos into window (handles init and window changes mid-playback)
                if (readPos[i] < (float)loopStart) readPos[i] = (float)loopStart;
                if (readPos[i] >= (float)loopEnd)  readPos[i] = (float)(loopEnd - 1);

                int rp0 = clamp((int)readPos[i], loopStart, loopEnd - 1);
                int rp1 = clamp(rp0 + 1, loopStart, loopEnd - 1);
                float frac = readPos[i] - (float)rp0;
                sampL = bufL[i][rp0] + (bufL[i][rp1] - bufL[i][rp0]) * frac;
                sampR = bufR[i][rp0] + (bufR[i][rp1] - bufR[i][rp0]) * frac;

                // XFade envelope: linear fade zones at loop edges, shrinking toward edges as xfade decreases.
                // xfade=0: no fade (gain=1 everywhere); xfade=1: fade spans full half-loop each side (triangle).
                if (xfadeParam > 0.f) {
                    float posInLoop = readPos[i] - (float)loopStart;
                    float fadeLen   = xfadeParam * (float)(loopSize / 2);
                    float gain = 1.f;
                    if (posInLoop < fadeLen)
                        gain = posInLoop / fadeLen;
                    else if (posInLoop > (float)loopSize - fadeLen)
                        gain = ((float)loopSize - posInLoop) / fadeLen;
                    sampL *= clamp(gain, 0.f, 1.f);
                    sampR *= clamp(gain, 0.f, 1.f);
                }

                float speed = clamp(params[SPEED1_PARAM + i].getValue() + clamp(inputs[SPEED1CVIN_INPUT + i].getVoltage(), -5.f, 5.f) / 10.f, 0.025f, 1.f) * 2.f;

                // Wow and flutter: advance per-channel LFO phases and modulate speed
                wowPhase[i]     = std::fmod(wowPhase[i]     + wowRate     * args.sampleTime, 1.f);
                flutterPhase[i] = std::fmod(flutterPhase[i] + flutterRate * args.sampleTime, 1.f);
                float warbleMod = std::sin(2.f * float(M_PI) * wowPhase[i])     * wowDepth
                                + std::sin(2.f * float(M_PI) * flutterPhase[i]) * flutterDepth;
                speed *= (1.f + warbleMod);

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

        float srcParam = clamp(params[SOURCE1_PARAM + i].getValue() + clamp(inputs[SOURCE1CVIN_INPUT + i].getVoltage(), -5.f, 5.f) / 10.f, -1.f, 1.f);
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

    // === MEDIA (OGG LOOPS) — mixed in after XY mixer ===
    int mediaIdx = clamp((int)std::round(params[MEDIA_PARAM].getValue()
        + clamp(inputs[MEDIACVIN_INPUT].getVoltage(), -5.f, 5.f) * 7.f / 10.f), 0, 7);

    if (mediaIdx != currentOggIdx) {
        currentOggIdx = mediaIdx;
        oggPlayPos = 0.f;
    }

    float mediaL = 0.f, mediaR = 0.f;
    OggLoop& loop = oggLoops[mediaIdx];
    if (loop.loaded && loop.numSamples > 1) {
        float advance = (float)loop.nativeSampleRate / args.sampleRate;
        int i0 = (int)oggPlayPos;
        int i1 = (i0 + 1) % loop.numSamples;
        float frac = oggPlayPos - (float)i0;
        mediaL = (loop.L[i0] + (loop.L[i1] - loop.L[i0]) * frac) * 5.f;
        mediaR = (loop.R[i0] + (loop.R[i1] - loop.R[i0]) * frac) * 5.f;
        oggPlayPos += advance;
        if (oggPlayPos >= (float)loop.numSamples) oggPlayPos -= (float)loop.numSamples;
    }

    // Pre-fader send: raw media before amount knob
    if (noiseAuxPreFader)
        outputs[SEND_OUTPUT].setVoltage((mediaL + mediaR) * 0.5f);

    // Return replaces media (both L and R) before amount knob
    if (inputs[RETURN_INPUT].isConnected()) {
        float ret = inputs[RETURN_INPUT].getVoltage();
        mediaL = ret;
        mediaR = ret;
    }

    float amount = clamp(params[AMOUNT_PARAM].getValue()
        + clamp(inputs[AMOUNTCVIN_INPUT].getVoltage(), -5.f, 5.f) / 10.f, 0.f, 1.f);
    float noiseMixL = mediaL * amount;
    float noiseMixR = mediaR * amount;

    // Post-fader send: after amount knob
    if (!noiseAuxPreFader)
        outputs[SEND_OUTPUT].setVoltage((noiseMixL + noiseMixR) * 0.5f);

    outL = clamp(outL + noiseMixL, -5.f, 5.f);
    outR = clamp(outR + noiseMixR, -5.f, 5.f);

    outputs[AUDIOLEFTOUT_OUTPUT].setVoltage(outL);
    outputs[AUDIORIGHTOUT_OUTPUT].setVoltage(outR);

    // Recording: blend input with the sample already at the write position (sound-on-sound).
    // Warble is NOT involved — chanMixL uses a warble-modulated readPos and must not be
    // written back into the buffer, or the warble effect would accumulate across overdubs.
    for (int i = 0; i < 4; i++) {
        if (!recordState[i] || bufferSize == 0) continue;

        float srcParam = clamp(params[SOURCE1_PARAM + i].getValue() + clamp(inputs[SOURCE1CVIN_INPUT + i].getVoltage(), -5.f, 5.f) / 10.f, -1.f, 1.f);
        float t = (srcParam + 1.f) * 0.5f; // 0 = record input only, 1 = layer over existing buffer
        bufL[i][writePos[i]] = inL * (1.f - t) + bufL[i][writePos[i]] * t;
        bufR[i][writePos[i]] = inR * (1.f - t) + bufR[i][writePos[i]] * t;

        writePos[i]++;
        if (recordedLength[i] < bufferSize) recordedLength[i]++;
        hasContent[i] = true;

        if (writePos[i] >= bufferSize) {
            writePos[i] = 0;
            if (continuousRecord[i]) {
                // Keep recording — wrap and overwrite from the start
            } else {
                recordState[i] = false;
                if (autoPlayFull[i] && !playState[i]) {
                    playState[i] = true;
                    readPos[i] = 0.f;
                }
            }
        }
    }

    // XY Pad — param + CV offset (CV bypassed while dragging), then slewed
    float targetX, targetY;
    if (!xyDragging.load()) {
        float cvX = inputs[XCVIN_INPUT].isConnected() ? clamp(inputs[XCVIN_INPUT].getVoltage(), -5.f, 5.f) / 10.f : 0.f;
        float cvY = inputs[YCVIN_INPUT].isConnected() ? clamp(inputs[YCVIN_INPUT].getVoltage(), -5.f, 5.f) / 10.f : 0.f;
        targetX = clamp(params[XPOS_PARAM].getValue() + cvX, 0.f, 1.f);
        targetY = clamp(params[YPOS_PARAM].getValue() + cvY, 0.f, 1.f);
    } else {
        targetX = params[XPOS_PARAM].getValue();
        targetY = params[YPOS_PARAM].getValue();
    }

    float slewParam = clamp(params[SLEW_PARAM].getValue() + (inputs[SLEWCVIN_INPUT].isConnected() ? clamp(inputs[SLEWCVIN_INPUT].getVoltage(), -5.f, 5.f) / 10.f : 0.f), 0.02f, 1.f);
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

	void onButton(const ButtonEvent& e) override {
		if (e.button == GLFW_MOUSE_BUTTON_RIGHT && e.action == GLFW_PRESS) {
			if (tehom) tehom->eraseBuffer(channel);
			e.consume(this);
			return;
		}
		LEDBezel::onButton(e);
	}

	void onEnter(const EnterEvent& e) override { isHovered = true; }

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
		if (!tehom) return;

		bool flashing = tehom->eraseFlash[channel] > 0.f;
		std::string desiredText = flashing ? "Erased" :
		                          (isHovered ? (tehom->recordState[channel] ? "Recording..." : "Record") : "");

		if (desiredText.empty()) {
			if (tooltip) {
				APP->scene->removeChild(tooltip);
				delete tooltip;
				tooltip = nullptr;
			}
		} else {
			if (!tooltip) {
				tooltip = new ui::Tooltip;
				APP->scene->addChild(tooltip);
			}
			tooltip->text = desiredText;
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
    double lastClickTime = -1.0;

    struct TrailPoint { Vec pos; float alpha; };
    std::vector<TrailPoint> trail;
    Vec lastTrailPos = {-1.f, -1.f};  // sentinel: uninitialized

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
            // Double-click snaps cursor to center
            double now = glfwGetTime();
            if (now - lastClickTime < 0.3) {
                lastClickTime = -1.0;
                dragPos.x = box.size.x * 0.5f;
                dragPos.y = box.size.y * 0.5f;
                updateFromPos();
                e.consume(this);
                return;
            }
            lastClickTime = now;

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

        // Persist trail — comet tail: each point starts at cursor radius and shrinks as it fades
        if (module->persist) {
            Vec cur(px, py);
            if (lastTrailPos.x < 0.f) lastTrailPos = cur;
            // Interpolate between last and current position at 2px steps
            Vec delta = cur.minus(lastTrailPos);
            float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
            int steps = std::max(1, (int)(dist / 2.f));
            for (int s = 0; s <= steps; s++) {
                float t = (float)s / (float)steps;
                Vec p = lastTrailPos.plus(delta.mult(t));
                trail.push_back({p, 1.f});
            }
            lastTrailPos = cur;
            if (trail.size() > 600)
                trail.erase(trail.begin(), trail.begin() + (int)(trail.size() - 600));
            for (auto& tp : trail) {
                float r = radius * tp.alpha;
                if (r > 0.f) {
                    nvgBeginPath(args.vg);
                    nvgCircle(args.vg, tp.pos.x, tp.pos.y, r);
                    nvgFillColor(args.vg, nvgRGBAf(1.f, 1.f, 1.f, tp.alpha * 0.7f));
                    nvgFill(args.vg);
                }
                tp.alpha -= 0.04f;
            }
            trail.erase(
                std::remove_if(trail.begin(), trail.end(), [](const TrailPoint& tp) { return tp.alpha <= 0.f; }),
                trail.end()
            );
        } else if (!trail.empty()) {
            trail.clear();
            lastTrailPos = {-1.f, -1.f};
        }

        // Crosshairs
        if (module->showCrosshairs) {
            nvgBeginPath(args.vg);
            nvgMoveTo(args.vg, px, 0);
            nvgLineTo(args.vg, px, box.size.y);
            nvgMoveTo(args.vg, 0, py);
            nvgLineTo(args.vg, box.size.x, py);
            nvgStrokeColor(args.vg, nvgRGB(60, 60, 60));
            nvgStroke(args.vg);
        }

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
struct TehomScrollingBG : Widget {
    Tehom* module = nullptr;
    std::shared_ptr<window::Svg> svg;
    float scrollX = 0.f;
    float scaledW = 0.f;

    static constexpr float speeds[] = {0.f, 0.15f, 0.5f, 1.4f}; // Off/Slow/Medium/Fast

    void step() override {
        Widget::step();
        if (scaledW <= 0.f) return;
        int spd = module ? clamp(module->bgScrollSpeed, 0, 3) : 2;
        bool goRight = module ? module->bgScrollRight : true;
        float delta = goRight ? speeds[spd] : -speeds[spd];
        scrollX += delta;
        if (scrollX >= scaledW) scrollX -= scaledW;
        if (scrollX < 0.f)      scrollX += scaledW;
    }

    void draw(const DrawArgs& args) override {
        if (!svg)
            svg = window::Svg::load(asset::plugin(pluginInstance, "res/panels/TehomBGSilver.svg"));
        if (!svg || !svg->handle) return;

        float svgW = svg->handle->width;
        float svgH = svg->handle->height;
        if (svgW <= 0.f || svgH <= 0.f) return;

        float scale = box.size.y / svgH;
        scaledW = svgW * scale;

        nvgSave(args.vg);
        nvgScissor(args.vg, 0, 0, box.size.x, box.size.y);
        // Scroll right: anchor first copy at scrollX - scaledW, second at scrollX
        nvgTranslate(args.vg, scrollX - scaledW, 0);
        nvgScale(args.vg, scale, scale);
        window::svgDraw(args.vg, svg->handle);
        nvgTranslate(args.vg, svgW, 0);
        window::svgDraw(args.vg, svg->handle);
        nvgRestore(args.vg);
    }
};
constexpr float TehomScrollingBG::speeds[];
struct TehomWidget : ModuleWidget {
	TehomWidget(Tehom* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/panels/Tehom.svg")));

		// Scrolling background (drawn below panel, shows through transparent areas)
		auto* bg = new TehomScrollingBG;
		bg->module   = module;
		bg->box.size = box.size;
		addChildBottom(bg);
       
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
		addParam(createParamCentered<TehomSmallKnob>(mm2px(Vec(60.972, 36.448)), module, Tehom::WARBLE_PARAM));
		addParam(createParamCentered<TehomSmallKnob>(mm2px(Vec(71.215, 36.448)), module, Tehom::MEDIA_PARAM));
		addParam(createParamCentered<TehomSmallKnob>(mm2px(Vec(81.457, 36.448)), module, Tehom::AMOUNT_PARAM));
		addParam(createParamCentered<TehomSmallKnob>(mm2px(Vec(60.972, 114.136)), module, Tehom::SLEW_PARAM));
		addParam(createParamCentered<TehomSmallKnob>(mm2px(Vec(71.215, 114.136)), module, Tehom::SIZE_PARAM));
		addParam(createParamCentered<TehomSmallKnob>(mm2px(Vec(81.457, 114.136)), module, Tehom::POSITION_PARAM));
		addParam(createParamCentered<TehomSmallKnob>(mm2px(Vec(91.8, 114.136)), module, Tehom::XFADE_PARAM));

		// Big Knobs
		addParam(createParamCentered<TehomKnob>(mm2px(Vec(14.738, 39.243)), module, Tehom::SOURCE1_PARAM));
		addParam(createParamCentered<TehomKnob>(mm2px(Vec(113.152, 39.243)), module, Tehom::SOURCE2_PARAM));
		addParam(createParamCentered<TehomKnob>(mm2px(Vec(14.738, 101.014)), module, Tehom::SOURCE3_PARAM));
		addParam(createParamCentered<TehomKnob>(mm2px(Vec(113.152, 101.014)), module, Tehom::SOURCE4_PARAM));
		addParam(createParamCentered<TehomKnob>(mm2px(Vec(38.86, 39.243)), module, Tehom::SPEED1_PARAM));
		addParam(createParamCentered<TehomKnob>(mm2px(Vec(137.275, 39.243)), module, Tehom::SPEED2_PARAM));
		addParam(createParamCentered<TehomKnob>(mm2px(Vec(38.86, 101.014)), module, Tehom::SPEED3_PARAM));
		addParam(createParamCentered<TehomKnob>(mm2px(Vec(137.275, 101.014)), module, Tehom::SPEED4_PARAM));
	
		// Audio inputs
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(60.972, 11.851)), module, Tehom::AUDIOLEFTIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(71.215, 11.851)), module, Tehom::AUDIORIGHTIN_INPUT));

		// Inputs 
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(60.972, 26.591)), module, Tehom::WARBLECVIN_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(71.215, 26.591)), module, Tehom::MEDIACVIN_INPUT));
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

        // GUI section
        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("GUI"));
        menu->addChild(createSubmenuItem("XY Pad", "", [=](Menu* subMenu) {
            subMenu->addChild(createCheckMenuItem("Draw Crosshairs", "",
                [=]() { return tehom->showCrosshairs; },
                [=]() { tehom->showCrosshairs = !tehom->showCrosshairs; }
            ));
            subMenu->addChild(createCheckMenuItem("Cursor Trails", "",
                [=]() { return tehom->persist; },
                [=]() { tehom->persist = !tehom->persist; }
            ));
        }));
        menu->addChild(createSubmenuItem("Background Scroll", "", [=](Menu* subMenu) {
            subMenu->addChild(createMenuLabel("Speed"));
            const char* names[] = {"Off", "Slow", "Medium", "Fast"};
            for (int s = 0; s < 4; s++) {
                subMenu->addChild(createCheckMenuItem(names[s], "",
                    [=]() { return tehom->bgScrollSpeed == s; },
                    [=]() { tehom->bgScrollSpeed = s; }
                ));
            }
            subMenu->addChild(new MenuSeparator);
            subMenu->addChild(createMenuLabel("Direction"));
            subMenu->addChild(createCheckMenuItem("Right", "",
                [=]() { return tehom->bgScrollRight; },
                [=]() { tehom->bgScrollRight = true; }
            ));
            subMenu->addChild(createCheckMenuItem("Left", "",
                [=]() { return !tehom->bgScrollRight; },
                [=]() { tehom->bgScrollRight = false; }
            ));
        }));

        // Global section
        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("Global"));
        menu->addChild(createSubmenuItem("Buffer Size", "", [=](Menu* subMenu) {
            const float durations[] = {1.f, 2.f, 5.f, 10.f, 20.f, 30.f, 60.f};
            const char* labels[]    = {"1 second", "2 seconds", "5 seconds", "10 seconds", "20 seconds", "30 seconds", "1 minute"};
            for (int j = 0; j < 7; j++) {
                float dur = durations[j];
                subMenu->addChild(createCheckMenuItem(
                    labels[j], "",
                    [=]() { return tehom->bufferDuration == dur; },
                    [=]() { tehom->bufferDuration = dur; tehom->resizeBuffers(tehom->currentSampleRate); }
                ));
            }
        }));
        menu->addChild(createSubmenuItem("Noise Aux Send", "", [=](Menu* subMenu) {
            subMenu->addChild(createCheckMenuItem("Pre-Fader", "",
                [=]() { return tehom->noiseAuxPreFader; },
                [=]() { tehom->noiseAuxPreFader = true; }
            ));
            subMenu->addChild(createCheckMenuItem("Post-Fader", "",
                [=]() { return !tehom->noiseAuxPreFader; },
                [=]() { tehom->noiseAuxPreFader = false; }
            ));
        }));

        // Channels section
        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("Channels"));
        const std::vector<std::string> modeNames = {"Play/Stop", "Retrigger", "Forward/Reverse"};
        for (int i = 0; i < 4; i++) {
            menu->addChild(createSubmenuItem("Channel " + std::to_string(i + 1), "", [=](Menu* subMenu) {
                subMenu->addChild(createMenuLabel("Recording"));
                subMenu->addChild(createCheckMenuItem(
                    "Continuous Record", "",
                    [=]() { return tehom->continuousRecord[i]; },
                    [=]() { tehom->continuousRecord[i] = !tehom->continuousRecord[i]; }
                ));
                subMenu->addChild(new MenuSeparator);
                subMenu->addChild(createMenuLabel("Auto-Play"));
                subMenu->addChild(createCheckMenuItem(
                    "Auto-Play when recording complete", "",
                    [=]() { return tehom->autoPlay[i]; },
                    [=]() { tehom->autoPlay[i] = !tehom->autoPlay[i]; }
                ));
                subMenu->addChild(createCheckMenuItem(
                    "Auto-Play when buffer full", "",
                    [=]() { return tehom->autoPlayFull[i]; },
                    [=]() { tehom->autoPlayFull[i] = !tehom->autoPlayFull[i]; }
                ));
                subMenu->addChild(new MenuSeparator);
                subMenu->addChild(createMenuLabel("Play CV Mode"));
                for (int m = 0; m < 3; m++) {
                    subMenu->addChild(createCheckMenuItem(
                        modeNames[m], "",
                        [=]() { return tehom->playCVMode[i] == m; },
                        [=]() { tehom->playCVMode[i] = m; }
                    ));
                }
            }));
        }
    }
};
Model* modelTehom = createModel<Tehom, TehomWidget>("Tehom");