#pragma once
#include <string>
#include <algorithm>

struct RhythmData {
    static const int NUM_STYLES = 5;
    static const int NUM_DRUMS = 6;
    static const int NUM_PATTERNS = 8;
    static const int MAX_STEPS = 16;

    // rhythms[style][drum][pattern][step]

    //Styles order: Bulgar/Fraylach, Araber, Terkisher, Zhok/Hora, In Zibn
    bool rhythms[NUM_STYLES][NUM_DRUMS][NUM_PATTERNS][MAX_STEPS] = {};
    int sequenceLengths[NUM_STYLES] = {16, 16, 16, 16, 16}; // Sequence step length 

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
	1 = Araber
    2 = Terkisher
    3 = Hora
    4 = 7/8

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

        // Stle 0: Bulgar/Freylach
        // Kick
        loadRhythmFromString(0, 0, 0, "1000000000001000");
        loadRhythmFromString(0, 0, 1, "1000001000001000");
        loadRhythmFromString(0, 0, 2, "1000100010001000");
        loadRhythmFromString(0, 0, 3, "1000101000001000");
        loadRhythmFromString(0, 0, 4, "1000001000001010");
        loadRhythmFromString(0, 0, 5, "1000101000101000");
        loadRhythmFromString(0, 0, 6, "1000001000101010");
        loadRhythmFromString(0, 0, 7, "1000101000101010");
        // Snare
        loadRhythmFromString(0, 1, 0, "0000100000000000");
        loadRhythmFromString(0, 1, 1, "0000100000100000");
        loadRhythmFromString(0, 1, 2, "0000100000100010");
        loadRhythmFromString(0, 1, 3, "0010100000100010");
        loadRhythmFromString(0, 1, 4, "0011100000100010");
        loadRhythmFromString(0, 1, 5, "0011101000100010");
        loadRhythmFromString(0, 1, 6, "0011101010100010");
        loadRhythmFromString(0, 1, 7, "1011101010101010");
        // Hi-Hat Closed
        loadRhythmFromString(0, 2, 0, "1000000010000000");
        loadRhythmFromString(0, 2, 1, "1000100010001000");
        loadRhythmFromString(0, 2, 2, "1000100010001010");
        loadRhythmFromString(0, 2, 3, "1000101010001010");
        loadRhythmFromString(0, 2, 4, "1000101010101010");
        loadRhythmFromString(0, 2, 5, "1000101010101110");
        loadRhythmFromString(0, 2, 6, "1011101010101110");
        loadRhythmFromString(0, 2, 7, "1011101010101111");
        // Hi-Hat Open
        loadRhythmFromString(0, 3, 0, "0000100000001000");
        loadRhythmFromString(0, 3, 1, "0000100010001000");
        loadRhythmFromString(0, 3, 2, "1000001000001000");
        loadRhythmFromString(0, 3, 3, "1000001000101000");
        loadRhythmFromString(0, 3, 4, "1000001000101010");
        loadRhythmFromString(0, 3, 5, "1000100010001000");
        loadRhythmFromString(0, 3, 6, "0010001000100010");
        loadRhythmFromString(0, 3, 7, "0010001000101010");
        // Perc1
        loadRhythmFromString(0, 4, 0, "0000000000000010");
        loadRhythmFromString(0, 4, 1, "0000000000100010");
        loadRhythmFromString(0, 4, 2, "1000000000100010");
        loadRhythmFromString(0, 4, 3, "1000001000100010");
        loadRhythmFromString(0, 4, 4, "0000101000100010");
        loadRhythmFromString(0, 4, 5, "0010101101101010");
        loadRhythmFromString(0, 4, 6, "0011001110101000");
        loadRhythmFromString(0, 4, 7, "0011011011011000");
        // Perc2
        loadRhythmFromString(0, 5, 0, "0000110000001100");
        loadRhythmFromString(0, 5, 1, "0000110011001100");
        loadRhythmFromString(0, 5, 2, "1010001010001001");
        loadRhythmFromString(0, 5, 3, "1000110011001101");
        loadRhythmFromString(0, 5, 4, "0011001001101011");
        loadRhythmFromString(0, 5, 5, "1010101010001101");
        loadRhythmFromString(0, 5, 6, "1011101011101111");
        loadRhythmFromString(0, 5, 7, "1011101110111001");

        //Style 1: Araber
        //Kick
        loadRhythmFromString(1, 0, 0, "1000001000001000");
        loadRhythmFromString(1, 0, 1, "1000001010000000");
        loadRhythmFromString(1, 0, 2, "1000001010001000");
        loadRhythmFromString(1, 0, 3, "1000001010001010");
        loadRhythmFromString(1, 0, 4, "1000001010101000");
        loadRhythmFromString(1, 0, 5, "1010001010001000");
        loadRhythmFromString(1, 0, 6, "1010001010001010");
        loadRhythmFromString(1, 0, 7, "1010001010001011");
        //Snare
        loadRhythmFromString(1, 1, 0, "0000000000001000");
        loadRhythmFromString(1, 1, 1, "0000000010000000");
        loadRhythmFromString(1, 1, 2, "0000000000001010");
        loadRhythmFromString(1, 1, 3, "0000100000100000");
        loadRhythmFromString(1, 1, 4, "0000100000000010");
        loadRhythmFromString(1, 1, 5, "0010000000001000");
        loadRhythmFromString(1, 1, 6, "0010000000001011");
        loadRhythmFromString(1, 1, 7, "0010000010001100");
        //Closed Hat
        loadRhythmFromString(1, 2, 0, "0010001000101000");
        loadRhythmFromString(1, 2, 1, "1000110010001100");
        loadRhythmFromString(1, 2, 2, "1110001010001011");
        loadRhythmFromString(1, 2, 3, "1100110011001100");
        loadRhythmFromString(1, 2, 4, "1101101110101000");
        loadRhythmFromString(1, 2, 5, "1011101010101011");
        loadRhythmFromString(1, 2, 6, "1110111011101110");
        loadRhythmFromString(1, 2, 7, "1110111011101111");
        //Open Hat
        loadRhythmFromString(1, 3, 0, "0000100000100000");
        loadRhythmFromString(1, 3, 1, "0000100000001000");
        loadRhythmFromString(1, 3, 2, "0000100000001010");
        loadRhythmFromString(1, 3, 3, "1000000010000000");
        loadRhythmFromString(1, 3, 4, "1000000010001000");
        loadRhythmFromString(1, 3, 5, "1000001010000000");
        loadRhythmFromString(1, 3, 6, "1000001010000010");
        loadRhythmFromString(1, 3, 7, "1000001010000011");
        //Perc 1
        loadRhythmFromString(1, 4, 0, "0000000000001000");
        loadRhythmFromString(1, 4, 1, "0000001000000010");
        loadRhythmFromString(1, 4, 2, "0000100000000010");
        loadRhythmFromString(1, 4, 3, "0000100000100010");
        loadRhythmFromString(1, 4, 4, "0010001000100000");
        loadRhythmFromString(1, 4, 5, "0010001000101000");
        loadRhythmFromString(1, 4, 6, "0010001000100011");
        loadRhythmFromString(1, 4, 7, "0010001000100111");
        //Perc 2 
        loadRhythmFromString(1, 5, 0, "0000000000000010");
        loadRhythmFromString(1, 5, 1, "0000000100000010");
        loadRhythmFromString(1, 5, 2, "0010000000000010");
        loadRhythmFromString(1, 5, 3, "0000101000000000");
        loadRhythmFromString(1, 5, 4, "0010001000000000");
        loadRhythmFromString(1, 5, 5, "0010001000000010");
        loadRhythmFromString(1, 5, 6, "0010001000000011");
        loadRhythmFromString(1, 5, 7, "0010001000000111");
        
        //Stle 2: Terkisher
        //Style 3: Hora/Zhok
        //Style 4: In Zibn
    }
};
