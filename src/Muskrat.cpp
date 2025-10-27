#include "rack.hpp"
#include "plugin.hpp"
#include "dsp/digital.hpp"  // For utilities like `mix` and other DSP-related functions
#include "../res/wavetables/MuskratWavetables.hpp"
#include <random> 
#include <vector>

using namespace rack;

struct Muskrat : Module {
    enum ParamId {
        TAIL_PARAM,
        RANGE_PARAM,
        SCRATCH_PARAM,
        DIG_PARAM,
        CHEW_PARAM,
        RATSWITCH_PARAM,
        SELECT_PARAM,
        PARAMS_LEN
    };
    enum InputId {
        BANG_INPUT,
        SCRATCH_CV_INPUT,
        DIG_CV_INPUT,
        CHEW_CV_INPUT,
        MUSKRAT_INPUT,
        INPUTS_LEN
    };
    enum OutputId {
        AUDIO_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId {
        LED_LIGHT,
        LIGHTS_LEN
    };

 // Constructor
 Muskrat() {
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	configParam(TAIL_PARAM, 0.f, 1.f, 0.5f, "Decay Time", " %", 0.f, 100.f);
	configSwitch(RANGE_PARAM, 0.f, 2.f, 1.f, "Decay Time Range", {"Fast", "Medium", "Slow"});
	configParam(SCRATCH_PARAM, 0.f, 1.f, 0.5f, "Scratch", " %", 0.f, 100.f);
	configParam(DIG_PARAM, 0.f, 1.f, 0.5f, "Dig", " %", 0.f, 100.f);
	configParam(CHEW_PARAM, 0.f, 1.f, 0.5f, "Chew", " %", 0.f, 100.f);

	configInput(BANG_INPUT, "Bang! Gate");
	configInput(SCRATCH_CV_INPUT, "Scratch CV");
	configInput(DIG_CV_INPUT, "Dig CV");
	configInput(CHEW_CV_INPUT, "Chew CV");
	configSwitch(RATSWITCH_PARAM, 0.f, 1.f, 0.f, "Ratswitch", {"Off", "On"});
	configInput(MUSKRAT_INPUT, "Ratswitch Gate");
	configOutput(AUDIO_OUTPUT, "Muskrat Audio");

  configSwitch(SELECT_PARAM, 1.f, 4.f, 1.f, "Algorithm", {"Muskrat", "FM", "PD", "Granular"}); 
}
  
  //Granular 
  float wavetablesGranular[WAVETABLE_SIZE_GRANULAR];
  float phaseGranular = 0.0f; // phaseGranular accumulator for oscillator
  //OG
  float wavetables[WAVETABLE_SIZE];
  float phase = 0.0f; // Phase accumulator for oscillator
  float frequency = 440.0f; // Default frequency    
  float currentVoltage = 0.0f;
  float amplitude = 40.0f; 
  std::random_device rd;
  std::mt19937 gen{rd()};
  std::uniform_real_distribution<float> dis{-amplitude, amplitude}; // Range -5V to +5V
  // FM
  float phase1 = 0.0f;
  float phase2 = 0.0f;
  float freq1 = 0.0f;
  float freq2 = 0.0f;
  float minFreq = 900.0f;
  float maxFreq = 920.0f;
  float triangleWave(float phaseFM) {
  phaseFM = phaseFM - floor(phaseFM);
  return 2.0f * fabs(2.0f * phaseFM - 1.0f) - 1.0f;
  }
   //PD
   float minFreqPD = 150.0f;
   float maxFreqPD = 275.0f;
   float phase3 = 0.0f;
   float freq3 = 0.0f;
   float triangleWave2(float phaseFM) {
   phaseFM = phaseFM - floor(phaseFM);
   return 2.0f * fabs(2.0f * phaseFM - 1.0f) - 1.0f;
   }
   float sawtoothWave(float phasePD) {
   return 2.0f * (phasePD - floor(phasePD + 0.5f)); 
   }

	// Envelope control variables for decay
  float envelopeValue = 1.0f;  // Envelope output value (for volume control)
  float decayTime = 5.0f;      // Decay time in ms (5ms as requested)
  float decayAlpha = 0.0f;     // Exponential decay coefficient
  bool pulseTriggered = false; // Flag to check if pulse was triggered
  float pulseTime = 0.0f;      // Time tracking for pulse duration
  bool lastBangState = false;  // Previous state of the Bang input

  int ratSelect = 1;

void process (const ProcessArgs &args) override {
  //Scratch param
	float cvInput2 = inputs[SCRATCH_CV_INPUT].getVoltage();  // Read CV input
	float normalizedCV2 = (cvInput2 + 5.0f) / 10.0f; // Map -5V -> 0.0 and 5V -> 1.0
	float knob2Param = params[SCRATCH_PARAM].getValue() + 0.05;  // Original knob value
	float knob2Value = knob2Param + (normalizedCV2 - 0.5f);  // Apply the CV input as an offset
	float controlValue2 = clamp(knob2Value, 0.0f, 1.0f);

  // DIG Param
	float cvInput3 = inputs[DIG_CV_INPUT].getVoltage();  // Read CV input
	float normalizedCV3 = (cvInput3 + 5.0f) / 10.0f; // Map -5V -> 0.0 and 5V -> 1.0
	float knob3Param = params[DIG_PARAM].getValue() + 0.05;  // Original knob value
	float knob3Value = knob3Param + (normalizedCV3 - 0.5f);  // Apply the CV input as an offset
	float controlValue3 = clamp(knob3Value, 0.0f, 1.0f);
	float WAVETABLE_LENGTH = std::min(controlValue3 * 318.0f + 10.0f, 318.0f);
  float foldThreshold = 0.25f + (controlValue3 * 0.25);
	float distortion = foldThreshold * 500; 
	
	//Chew param
	float cvInput = inputs[CHEW_CV_INPUT].getVoltage();  // Read CV input
	float normalizedCV = (cvInput + 5.0f) / 10.0f; // Map -5V -> 0.0 and 5V -> 1.0
	float knob1Param = params[CHEW_PARAM].getValue() + 0.05;  // Original knob value
	float knob1Value = knob1Param + (normalizedCV - 0.5f);  // Apply the CV input as an offset
	float controlValue = clamp(knob1Value, 0.0f, 1.0f);
	frequency = controlValue * args.sampleRate; // Scale the DIG_PARAM for frequency range

    // OG
    const unsigned char* wavetablesData[] = {
        one_wav, two_wav, three_wav, four_wav, five_wav,
        six_wav, seven_wav, eight_wav, nine_wav, ten_wav,
        eleven_wav, twelve_wav, thirteen_wav, fourteen_wav, fifteen_wav,
        sixteen_wav, seventeen_wav, eighteen_wav, nineteen_wav, twenty_wav,
        twentyOne_wav, twentyTwo_wav, twentyThree_wav, twentyFour_wav
      };
    int index = (controlValue2 * 23);
    if (index >= 0 && index < 24) {
        // Normalize the selected wave and assign to wavetables
        for (int j = 0; j < WAVETABLE_SIZE; j++) {
            wavetables[j] = (float)(wavetablesData[index][j] - 238) / 238.0f;
        }
    }
    
		// Granular 
		const unsigned char* wavetablesDataGranular[] = {
			msgOne_wav, msgTwo_wav, msgThree_wav, msgFour_wav, msgFive_wav,
			msgSix_wav, msgSeven_wav, msgEight_wav, msgNine_wav, msgTen_wav,
			msgEleven_wav, msgTwelve_wav, msgThirteen_wav, msgFourteen_wav,
			msgFifteen_wav, msgSixteen_wav
		  };
    int indexGranular = (controlValue2 * 15);
    float phaseGranularIncrement = frequency / args.sampleRate;

// OG
float phaseIncrement = frequency / args.sampleRate;
phase += phaseIncrement;
phase = fmod(phase, WAVETABLE_SIZE);
int phaseIndex = (phase);
float outputSample = wavetables[phaseIndex];
float foldedSample = outputSample;
if (foldedSample > foldThreshold) {
    foldedSample = foldThreshold - (foldedSample - foldThreshold);  // Fold back when exceeding threshold
} else if (foldedSample < -foldThreshold) {
    foldedSample = -foldThreshold - (foldedSample + foldThreshold); // Fold back when below threshold
}

 // FM
 float fmDepth = controlValue2; 
 float pitch1 = controlValue;
 float pitch2 = controlValue3;
 phase2 += freq2 / args.sampleRate;
 if (phase2 >= 1.0f) phase2 -= 1.0f;
 float modulator = triangleWave(phase2) * 10.0f;  // Increased amplitude for more modulation
 freq1 = pitch1 * (minFreq + (maxFreq - minFreq) * pitch1) + modulator * fmDepth * 100.0f; // Increased modulation depth
 phase1 += freq1 / args.sampleRate;
 if (phase1 >= 1.0f) phase1 -= 1.0f;
 float fmOutput = triangleWave(phase1) * 5.0f;  // Output of oscillator 1 (carrier)

//PD
float pdDrive = controlValue2 * 32.f; 
float pitch3 = controlValue; 
int DIG = (int)(controlValue3 * 7.0f) + 1;  
float octaveMultiplier = (float)DIG;  
freq3 = pitch3 * octaveMultiplier * (minFreqPD + (maxFreqPD - minFreqPD) * pitch3);  
phase3 += freq3 / args.sampleRate;
if (phase3 >= 1.0f) phase3 -= 1.0f;
float waveformOutput;
   
// Switches
float chaos = 0.0f; 
bool switchState = params[RATSWITCH_PARAM].getValue() > 0.5f;
bool gateState = inputs[MUSKRAT_INPUT].getVoltage() > 0.5f;
if(switchState || gateState){
  waveformOutput = triangleWave2(phase3) * 5.0f;
  chaos = dis(gen); 
	phaseGranular -= phaseGranularIncrement;
    if (phaseGranular < 0) {
        phaseGranular += WAVETABLE_LENGTH; // Wrap phaseGranular to the end of the wavetable
    }
	freq2 = pitch2 * (1.0f + (20.0f - 1.0f) * pitch1);
}
else{
  chaos = 1; 
  phaseGranular += phaseGranularIncrement;
  phaseGranular = fmod(phaseGranular, WAVETABLE_LENGTH);
  freq2 = pitch2 * (minFreq + (maxFreq - minFreq) * pitch1);
  waveformOutput = sawtoothWave(phase3) * 5.0f;
}	      

//OG
foldedSample = (foldedSample * distortion) + chaos;  // Increased amplification for more pronounced effect
foldedSample += 1.0f;  // Apply offset to ensure the signal stays in a desired range
foldedSample = clamp(foldedSample, -5.0f, 5.0f);
const float cutoffFrequency = 8000.0f; // 10 kHz cutoff
float alpha = 1.0f / (1.0f + (args.sampleRate / (2.0f * M_PI * cutoffFrequency)));
static float previousOutput = 0.0f;
float filteredSample = alpha * foldedSample + (1.0f - alpha) * previousOutput;
previousOutput = filteredSample;

//Granular
if (indexGranular >= 0 && indexGranular < 16) {
	for (int j = 0; j < WAVETABLE_LENGTH; j++) {
		wavetablesGranular[j] = (float)(wavetablesDataGranular[indexGranular][j] - 318) / 318.0f;
	}
}
int phaseGranularIndex = (phaseGranular);
float outputSampleGranular = wavetablesGranular[phaseGranularIndex];

// PD 
float pdOutput = waveformOutput * pdDrive; 
pdOutput = clamp(pdOutput, -5.0f, 5.0f);

//Master decay 
int range = (int)params[RANGE_PARAM].getValue(); // Read the RANGE_PARAM value
switch (range) {
	case 0:
		decayTime = 5.0f + (params[TAIL_PARAM].getValue() * 25.0f); // 5ms to 30ms range
		break;
	case 1:
		decayTime = 30.0f + (params[TAIL_PARAM].getValue() * 170.0f); // 30ms to 200ms range
		break;
	case 2:
		decayTime = 200.0f + (params[TAIL_PARAM].getValue() * 3800.0f); // 200ms to 4000ms range
		break;
}
decayAlpha = exp(-1.0f / (args.sampleRate * (decayTime / 1000.0f))); // Calculate decay coefficient based on decay time
bool bangState = inputs[BANG_INPUT].getVoltage() > 0.5f;
if (bangState && !lastBangState) {
	pulseTriggered = true;
	envelopeValue = 1.0f;  // Start the envelope with full amplitude
	pulseTime = decayTime * (args.sampleRate / 1000.0f);
}
lastBangState = bangState;
if (pulseTriggered) {
	envelopeValue *= decayAlpha;  // Apply exponential decay
	if (envelopeValue < 0.01f) {  // End decay when value is small enough
		envelopeValue = 0.0f;
		pulseTriggered = false;
	}
} else {
	envelopeValue = 0.0f; // Reset envelope when no pulse is triggered
}

// Granular
float granularOutput = (((outputSampleGranular * 5.f) + 2.5f) * 5.0f) * envelopeValue; 
granularOutput = clamp(granularOutput, -5.0f, 5.0f);

/*
float selectPot = params[SELECT_PARAM].getValue();
if(selectPot < 0.43){
ratSelect = 1;
}
if (selectPot > 0.43 && selectPot < 0.54){
ratSelect = 2; 
}
if (selectPot > 0.54 && selectPot < 0.66){
ratSelect = 3; 
}
if (selectPot > 0.66){
ratSelect = 4; 
}
*/

ratSelect = params[SELECT_PARAM].getValue(); 

switch (ratSelect){
case 1: // Muskrat
outputs[AUDIO_OUTPUT].setVoltage(filteredSample * envelopeValue);
break; 
case 2: // F
outputs[AUDIO_OUTPUT].setVoltage(fmOutput * envelopeValue);
break;
case 3: // G
outputs[AUDIO_OUTPUT].setVoltage(pdOutput * envelopeValue);
break; 
case 4: // P
outputs[AUDIO_OUTPUT].setVoltage(granularOutput);
break; 
}

//LED 
float ledBrightness = envelopeValue;  // Map -5V to +5V range to 0-1
lights[LED_LIGHT].setBrightnessSmooth(ledBrightness, args.sampleTime);  // Set LED brightness
}

};

struct MuskratWidget : ModuleWidget {
	MuskratWidget(Muskrat* module) {
		setModule(module);
	  setPanel(createPanel(
		asset::plugin(pluginInstance, "res/panels/Muskrat.svg"),
		asset::plugin(pluginInstance, "res/panels/Muskrat-dark.svg")
		));
        
        addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		    addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<CKSSThree>(mm2px(Vec(24.513, 20.315)), module, Muskrat::RANGE_PARAM));
        addParam(createParamCentered<CKSS>(mm2px(Vec(24.478, 108.396)), module, Muskrat::RATSWITCH_PARAM));

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(38.3, 19.421)), module, Muskrat::TAIL_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(38.3, 41.886)), module, Muskrat::SCRATCH_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(38.3, 64.212)), module, Muskrat::DIG_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(38.3, 86.273)), module, Muskrat::CHEW_PARAM));
		    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(62, 50)), module, Muskrat::SELECT_PARAM));

        addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(10.5, 19.244)), module, Muskrat::BANG_INPUT));
        addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(10.5, 41.872)), module, Muskrat::SCRATCH_CV_INPUT));
        addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(10.5, 64.291)), module, Muskrat::DIG_CV_INPUT));
        addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(10.5, 86.24)), module, Muskrat::CHEW_CV_INPUT));
        addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(10.5, 108.196)), module, Muskrat::MUSKRAT_INPUT));

        addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(38.325, 108.226)), module, Muskrat::AUDIO_OUTPUT));

        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(38.295, 98.285)), module, Muskrat::LED_LIGHT));
	}
};


Model* modelMuskrat = createModel<Muskrat, MuskratWidget>("Muskrat");