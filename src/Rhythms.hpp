#pragma once
#include <string>
#include <algorithm>

struct RhythmData {
    static const int NUM_STYLES = 8;
    static const int NUM_DRUMS = 6;
    static const int NUM_PATTERNS = 8;
    static const int MAX_STEPS = 16;

    // rhythms[style][drum][pattern][step]
    bool rhythms[NUM_STYLES][NUM_DRUMS][NUM_PATTERNS][MAX_STEPS] = {};
    int sequenceLengths[NUM_STYLES] = {16, 16, 16, 16, 16, 16, 16, 16};

    RhythmData() {
        initRhythms();
    }

    // Load pattern from a string of '0' and '1'
    void loadRhythmFromString(int style, int drum, int pattern, const std::string& patternStr) {
        int len = std::min((int)patternStr.size(), MAX_STEPS);
        for (int i = 0; i < len; i++)
            rhythms[style][drum][pattern][i] = (patternStr[i] == '1');
    }

	/*
	========================
	Tantz Drum Sequencer Mapping
	========================

	--- Styles (STYLE_PARAM) ---
	0 = Bulgar
	1 = Sher
	2 = Khosidl
	3 = Skotshne
	4 = Hora
	5 = Zhok
	6 = Araber
	7 = Terkisher
	8 = In Zibn

	--- Drums ---
	0 = Kick
	1 = Snare
	2 = Hi-Hat Closed
	3 = Hi-Hat Open
	4 = Percussion 1
	5 = Percussion 2

	--- Patterns per Drum ---
	0 = Pattern 1
	1 = Pattern 2
	2 = Pattern 3
	3 = Pattern 4
	4 = Pattern 5
	5 = Pattern 6
	6 = Pattern 7
	7 = Pattern 8

	--- Pattern String ---
	"1" = gate ON
	"0" = gate OFF
	Length = up to MAX_STEPS (16 by default)
	Example: "1000100010001000" → hits on steps 1, 5, 9, 13

	========================
	Usage:
	loadRhythmFromString(style, drum, pattern, "1010001010001000");
	========================
    */

    void initRhythms() {
        // ============================
        // Example: Style 0 (Bulgar)
        // ============================
        // Kick
        loadRhythmFromString(0, 0, 0, "1000100010001000");
        loadRhythmFromString(0, 0, 1, "1001001001001001");
        loadRhythmFromString(0, 0, 2, "1000001010000101");
        loadRhythmFromString(0, 0, 3, "1001001010010010");
        loadRhythmFromString(0, 0, 4, "1100100010001000");
        loadRhythmFromString(0, 0, 5, "1000100010101000");
        loadRhythmFromString(0, 0, 6, "1010101010101010");
        loadRhythmFromString(0, 0, 7, "1001000010100100");

        // Snare
        loadRhythmFromString(0, 1, 0, "0000100000001000");
        loadRhythmFromString(0, 1, 1, "0000100100001000");
        loadRhythmFromString(0, 1, 2, "0000000010000000");
        loadRhythmFromString(0, 1, 3, "0000100000001000");
        loadRhythmFromString(0, 1, 4, "0000001000100000");
        loadRhythmFromString(0, 1, 5, "0000100001000000");
        loadRhythmFromString(0, 1, 6, "0000000010100000");
        loadRhythmFromString(0, 1, 7, "0000100000100000");

        // Hi-Hat Closed
        loadRhythmFromString(0, 2, 0, "1010101010101010");
        loadRhythmFromString(0, 2, 1, "1111111100000000");
        loadRhythmFromString(0, 2, 2, "1010110010101100");
        loadRhythmFromString(0, 2, 3, "1111000011110000");
        loadRhythmFromString(0, 2, 4, "1100110011001100");
        loadRhythmFromString(0, 2, 5, "1111111111111111");
        loadRhythmFromString(0, 2, 6, "1000100010001000");
        loadRhythmFromString(0, 2, 7, "1010101010001010");

        // Hi-Hat Open
        loadRhythmFromString(0, 3, 0, "0000010000000100");
        loadRhythmFromString(0, 3, 1, "0000001000000010");
        loadRhythmFromString(0, 3, 2, "0000000000001000");
        loadRhythmFromString(0, 3, 3, "0000100000010000");
        loadRhythmFromString(0, 3, 4, "0001000100010001");
        loadRhythmFromString(0, 3, 5, "0000100010001000");
        loadRhythmFromString(0, 3, 6, "0000010001000100");
        loadRhythmFromString(0, 3, 7, "0000100100010010");

        // Perc1
        loadRhythmFromString(0, 4, 0, "1000100010001000");
        loadRhythmFromString(0, 4, 1, "0100010001000100");
        loadRhythmFromString(0, 4, 2, "0010001000100010");
        loadRhythmFromString(0, 4, 3, "0001000100010001");
        loadRhythmFromString(0, 4, 4, "1010101010101010");
        loadRhythmFromString(0, 4, 5, "1100110011001100");
        loadRhythmFromString(0, 4, 6, "1111000011110000");
        loadRhythmFromString(0, 4, 7, "0000111100001111");

        // Perc2
        loadRhythmFromString(0, 5, 0, "0001000100010001");
        loadRhythmFromString(0, 5, 1, "0010001000100010");
        loadRhythmFromString(0, 5, 2, "0100010001000100");
        loadRhythmFromString(0, 5, 3, "1000100010001000");
        loadRhythmFromString(0, 5, 4, "1111000011110000");
        loadRhythmFromString(0, 5, 5, "1100110011001100");
        loadRhythmFromString(0, 5, 6, "1010101010101010");
        loadRhythmFromString(0, 5, 7, "0000111100001111");
    }
};
