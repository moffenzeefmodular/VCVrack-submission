#include "plugin.hpp"

struct DialUp : Module {
    // Parameters
    enum ParamId {
        TAIL_PARAM,       // Tail knob for decay time control
        RANGE_PARAM,      // Range switch for decay time
        MODEM_PARAM,
        PARAMS_LEN
    };

    // Inputs
    enum InputId {
        BANG_INPUT,
        MODEM_CV_INPUT,
        INPUTS_LEN
    };

    // Outputs
    enum OutputId {
        DIAL_UP_OUTPUT,
        OUTPUTS_LEN
    };

    // Lights
    enum LightId {
        LED_LIGHT,
        LIGHTS_LEN
    };

    // ---- ATtiny85 @ 8MHz emulation (hardware firmware MOF-DIA-CODE-v0_3) ----
    // Timer0: 8MHz / 8 prescale = 1MHz tick, fast PWM with TOP = OCR0A, so the
    // sample ISR fires every (OCR0A + 1) microseconds. OCR0A is set by the pot
    // (80..20), giving an ISR rate of ~12.3kHz..47.6kHz. Timer1 PWM outputs the
    // 8-bit OCR1A value. Everything below runs on emulated microseconds, so the
    // sound is independent of Rack's sample rate.
    static constexpr float ADC_PERIOD_US = 104.f; // ~13 ADC clocks @ 125kHz: loop() pot read rate

    uint16_t acc[3] = {0, 0, 0};  // 16-bit 'unsigned int' accumulators (AVR)
    uint16_t noteVal = 1;         // Note/Note1/Note2 (all identical in firmware)
    int ocr0a = 50;               // Timer0 TOP -> ISR period
    uint8_t ocr1a = 0;            // Current 8-bit PWM output value
    float isrPhaseUs = 0.f;       // Microseconds since last ISR
    float adcPhaseUs = 0.f;       // Microseconds since last pot read
    float adcNoise = 0.f;         // Slowly varying ADC noise (in LSBs)

    // Emulates loop(): read pot on A3, update Note and OCR0A
    void readPot(float control) {
        // Small ADC noise, as on the real chip (±~1 LSB)
        adcNoise += 0.1f * (random::normal() * 0.7f - adcNoise);
        long potRaw = std::max(0L, std::min(1023L, (long)std::lround(control * 1023.f + adcNoise)));
        // Arduino map() uses long math with truncation
        noteVal = (uint16_t)(potRaw * (32 - 1) / 1023 + 1);
        ocr0a = (int)(80 + potRaw * (20 - 80) / 1015);
    }

    // Runs the emulated chip for durUs microseconds; returns the PWM duty
    // (0..255) averaged over that time (Timer1 PWM is 64MHz/256 = 250kHz, far
    // above audio, so its output is the time-average of OCR1A).
    float emulate(float durUs, float control) {
        float remaining = durUs;
        float integral = 0.f;
        while (remaining > 0.f) {
            float isrPeriodUs = (float)(ocr0a + 1);
            if (isrPhaseUs >= isrPeriodUs) {
                isrPhaseUs -= isrPeriodUs;
                if (isrPhaseUs >= isrPeriodUs)
                    isrPhaseUs = 0.f;
                sampleIsr();
                continue;
            }
            float step = std::min(remaining, isrPeriodUs - isrPhaseUs);
            if (!(step > 0.f))  // guard against float edge cases (never spin)
                break;
            integral += ocr1a * step;
            isrPhaseUs += step;
            remaining -= step;
            adcPhaseUs += step;
            if (adcPhaseUs >= ADC_PERIOD_US) {
                adcPhaseUs -= ADC_PERIOD_US;
                readPot(control);  // OCR0A change takes effect on next period
            }
        }
        return integral / durUs;
    }

    // Anti-aliasing: oversample to ~384kHz and run an 8th-order Butterworth
    // low-pass at a fixed 16kHz before decimating to Rack's rate.
    dsp::BiquadFilter antiAlias[4];
    int osFactor = 1;
    float configuredSampleRate = 0.f;
    float lowPassAlpha = 1.f;

    void configureOversampling(float sr) {
        osFactor = clamp((int)std::ceil(384000.f / sr), 1, 16);
        const float antiAliasHz = 16000.f;
        float fc = std::min(antiAliasHz, 0.45f * sr) / (sr * osFactor);
        static const float q[4] = {0.5098f, 0.6013f, 0.9000f, 2.5629f};
        for (int k = 0; k < 4; k++) {
            antiAlias[k].reset();
            antiAlias[k].setParameters(dsp::BiquadFilter::LOWPASS, fc, q[k], 1.f);
        }
        lowPassAlpha = 1.f - std::exp(-2.f * (float)M_PI * 10000.f / (sr * osFactor));
        configuredSampleRate = sr;
    }

    // Emulates ISR(TIMER0_COMPA_vect)
    void sampleIsr() {
        acc[0] += noteVal;
        acc[1] += noteVal;
        acc[2] += noteVal;
        // OCR1A = (Acc[0] * Acc[1]) ^ Acc[2] >> 8 & 0x80;
        // precedence: (A0*A1) ^ ((A2 >> 8) & 0x80); 16-bit product, 8-bit register
        uint16_t prod = (uint16_t)((uint32_t)acc[0] * (uint32_t)acc[1]);
        ocr1a = (uint8_t)(prod ^ ((acc[2] >> 8) & 0x80));
    }

    // Low-pass/high-pass filter variables
    float previousLowPassOutput = 0.0f; 
    float previousHighPassOutput = 0.0f;
    float previousInput = 0.0f; 

    // Envelope control variables for decay
    float envelopeValue = 1.0f;  // Envelope output value (for volume control)
    float decayTime = 5.0f;      // Decay time in ms (5ms as requested)
    float decayAlpha = 0.0f;     // Exponential decay coefficient
    bool pulseTriggered = false; // Flag to check if pulse was triggered
    float pulseTime = 0.0f;      // Time tracking for pulse duration
    bool lastBangState = false;  // Previous state of the Bang input
    float sampleRate = 44100.0f; // Default sample rate (you can modify this if needed)

    // Constructor for DialUp module
    DialUp() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(TAIL_PARAM, 0.f, 1.f, 0.f, "Decay Time", " %", 0.f, 100.f);
        configSwitch(RANGE_PARAM, 0.f, 2.f, 0.f, "Decay Range", {"Fast", "Medium", "Slow"});
        configParam(MODEM_PARAM, 0.f, 1.f, 0.5f, "Modem", " %", 0.f, 100.f);
        configInput(BANG_INPUT, "Bang Gate");
        configInput(MODEM_CV_INPUT, "Modem CV");
        configOutput(DIAL_UP_OUTPUT, "Dial-Up Audio");
    }

    // Process function (audio generation logic)
    void process(const ProcessArgs &args) override {
        float cvInput = inputs[MODEM_CV_INPUT].getVoltage();  // Read CV input
        float normalizedCV = (cvInput + 5.0f) / 10.0f; // Map -5V -> 0.0 and 5V -> 1.0
        float knob1Value = params[MODEM_PARAM].getValue() + (normalizedCV - 0.5f);  // Apply the CV input as an offset
        float controlValue = clamp(knob1Value, 0.0f, 1.0f);

        // Run the emulated ATtiny in oversampled sub-steps, band-limit the PWM
        // signal with a fixed 16kHz 8th-order low-pass at the oversampled rate,
        // then decimate. Because the emulation runs on emulated microseconds and
        // the anti-alias filter has the same cutoff at every rate, pitch and
        // timbre are the same at every Rack sample rate.
        if (args.sampleRate != configuredSampleRate)
            configureOversampling(args.sampleRate);
        const float subUs = 1e6f / (args.sampleRate * osFactor);
        float pwmAverage = 0.f;
        for (int i = 0; i < osFactor; i++) {
            float y = emulate(subUs, controlValue);
            // One-pole low-pass at 10 kHz (run at the oversampled rate)
            previousLowPassOutput += lowPassAlpha * (y - previousLowPassOutput);
            y = previousLowPassOutput;
            for (int k = 0; k < 4; k++)
                y = antiAlias[k].process(y);
            pwmAverage = y;  // keep last sub-sample (decimation)
        }

        // Map the 8-bit PWM value to -5V..+5V
        float lowPassFilteredOutput = pwmAverage * (10.0f / 255.0f) - 5.0f;

        // Apply high-pass filter for 100 Hz cutoff
        float highPassCutoffFreq = 100.0f;  // 100 Hz cutoff
        float highPassAlpha = args.sampleRate / (args.sampleRate + 2.0f * M_PI * highPassCutoffFreq);  // High-pass filter coefficient

        // High-pass filter equation
        float highPassFilteredOutput = highPassAlpha * (previousHighPassOutput + lowPassFilteredOutput - previousInput);

        // Store the filtered output and input for the next sample
        previousInput = lowPassFilteredOutput;
        previousHighPassOutput = highPassFilteredOutput;

        // Adjust decay time based on RANGE_PARAM
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

        // Calculate decayAlpha based on the decayTime (in ms)
        decayAlpha = exp(-1.0f / (args.sampleRate * (decayTime / 1000.0f))); // Calculate decay coefficient based on decay time

        // Rising edge detection for the BANG_INPUT
        bool bangState = inputs[BANG_INPUT].getVoltage() > 0.5f;
        if (bangState && !lastBangState) {
            // Trigger the envelope when Bang goes from low to high
            pulseTriggered = true;
            envelopeValue = 1.0f;  // Start the envelope with full amplitude
            pulseTime = decayTime * (args.sampleRate / 1000.0f);
        }

        // Update the lastBangState for the next cycle
        lastBangState = bangState;

        // If pulse is active, begin decay envelope
        if (pulseTriggered) {
            envelopeValue *= decayAlpha;  // Apply exponential decay

            if (envelopeValue < 0.01f) {  // End decay when value is small enough
                envelopeValue = 0.0f;
                pulseTriggered = false;
            }
        } else {
            envelopeValue = 0.0f; // Reset envelope when no pulse is triggered
        }

        // Final output is the filtered signal multiplied by the envelope value
        float finalOutput = highPassFilteredOutput * envelopeValue;

        finalOutput = clamp(finalOutput, -5.0f, 5.0f);

        // Output the result to the output port
        outputs[DIAL_UP_OUTPUT].setVoltage(finalOutput);  // Output audio as a control voltage

        // Map final output to LED light (0.0 to 1.0 range for LED)
        float ledBrightness = envelopeValue;  // Map -5V to +5V range to 0-1
        lights[LED_LIGHT].setBrightnessSmooth(ledBrightness, args.sampleTime);  // Set LED brightness
    }
};

struct DialUpWidget : ModuleWidget {
    DialUpWidget(DialUp* module) {
        setModule(module);
	setPanel(createPanel(
		asset::plugin(pluginInstance, "res/panels/DialUp.svg"),
		asset::plugin(pluginInstance, "res/panels/DialUp-dark.svg")
		));

        addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.15, 17.569)), module, DialUp::TAIL_PARAM));
        addParam(createParamCentered<CKSSThreeHorizontal>(mm2px(Vec(10.15, 35.230)), module, DialUp::RANGE_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.15, 52.693)), module, DialUp::MODEM_PARAM));

        addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(10.15, 85.071)), module, DialUp::BANG_INPUT));
        addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(10.15, 99.656)), module, DialUp::MODEM_CV_INPUT));

        addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(10.15, 113.171)), module, DialUp::DIAL_UP_OUTPUT));

        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(10.15, 75.116)), module, DialUp::LED_LIGHT));
    }
};

Model* modelDialUp = createModel<DialUp, DialUpWidget>("DialUp");
