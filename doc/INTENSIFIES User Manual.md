<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/IntensifiesScreenshot.png" height="638"></p>

---

# INTENSIFIES VCV Rack User Manual

---

# Overview

**INTENSIFIES** is an amplitude-modulated sample rate reducer with a built-in square wave synthesizer. This VCV Rack version is a Eurorack fantasy of the original INTENSIFIES effects pedal. Audio enters via stereo L/R inputs and is processed through a sample-and-hold stage clocked by the Carrier oscillator. An optional Modulator oscillator can amplitude-modulate the sample-and-hold clock rate, producing choppy, rhythmic sample rate reduction effects. The Gain stage adds overdrive. A separate Synth output generates a square wave from the combined Carrier and Modulator frequencies, usable as an independent noise or drone voice.

> **Note:** The hardware version of this module differs from the VCV Rack version (hardware is an effects pedal).

---

# CV Ranges

* **AUDIO IN L / R:** Standard audio input, ±5V. Right input normalizes to Left when unpatched.
* **CARRIER CV / MODULATOR CV / GAIN CV / FX VOLUME CV / SYNTH VOLUME CV:** ±10V (normalized to a 0–1 control range internally).
* **ENGAGE CV / BYPASS CV:** >+1V = force on; <-1V = force off; between ±1V = follow the switch.
* **FX L / R outputs:** ±5V audio.
* **SYNTH output:** ±5V audio.

---

# Controls

### CARRIER Knob
Controls the sample-and-hold clock frequency — the rate at which input samples are captured and held. Higher values = less sample rate reduction. Lower values = more aggressive lo-fi effect.

### CARRIER RANGE Switch
Sets the base frequency range for the Carrier:
- **Low:** Base 1Hz (very slow sample rate reduction / audio choppping)
- **Medium:** Base 10Hz
- **High:** Base 100Hz (subtle to moderate lo-fi)

### MODULATOR Knob
Controls the frequency of the amplitude modulator oscillator. When Engage is active, the sample-and-hold is only active during the high half of the modulator cycle, creating rhythmic gaps in the output.

### MODULATOR RANGE Switch
Sets the frequency range of the modulator oscillator:
- **Low:** 0.01Hz – 5Hz (slow rhythmic gating)
- **Medium:** 0.01Hz – 50Hz (rhythmic to audio-rate choppping)
- **High:** 0.01Hz – 500Hz (audio-rate amplitude modulation)

### MODULATOR ENGAGE Switch
- **Off:** Modulator is inactive. Sample-and-hold runs continuously at the Carrier rate.
- **On:** Modulator is active. Sample-and-hold runs only during the high half of the modulator cycle.

### GAIN Knob
Sets the amount of pre-output amplification/distortion applied to the FX output. Range is 1x to 20x (or 1x to 200x with GAIN RANGE set to High).

### GAIN RANGE Switch
- **20x:** Standard gain range.
- **200x:** Extended gain range for severe overdrive and clipping.

### FX VOLUME Knob
Sets the output level of the FX L/R outputs after the Gain stage.

### SYNTH VOLUME Knob
Sets the output level of the Synth output.

### BYPASS Switch
- **Off:** FX processing is active.
- **On:** Audio passes directly from the input to the FX output with no processing.

---

# I/O

### Inputs

* **AUDIO IN L:** Left audio input. ±5V.

* **AUDIO IN R:** Right audio input. ±5V. Normalizes to Audio In L when unpatched.

* **CARRIER CV:** Voltage control over the Carrier frequency. ±10V.

* **MODULATOR CV:** Voltage control over the Modulator frequency. ±10V.

* **GAIN CV:** Voltage control over the Gain amount. ±10V.

* **FX VOLUME CV:** Voltage control over the FX output level. ±10V.

* **SYNTH VOLUME CV:** Voltage control over the Synth output level. ±10V.

* **ENGAGE CV:** Voltage control over Modulator Engage. >+1V = on; <-1V = off.

* **BYPASS CV:** Voltage control over FX Bypass. >+1V = bypassed; <-1V = processing active.

### Outputs

* **FX L / FX R:** Stereo processed audio. Sample rate reduced, amplitude modulated, and amplified signal. Clamped to ±5V. The LED above the outputs reflects the current FX output level.

* **SYNTH OUT:** Square wave output derived from the logical OR of the Carrier and Modulator oscillator states. Independent of the audio input. Clamped to ±5V.

### LEDs

* **Main Out LED:** FX output level.
* **Gain LED:** Post-gain signal level.
* **Modulator LED:** Modulator oscillator state (illuminates when modulator is in its high phase).

---
