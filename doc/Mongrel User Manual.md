[![Video Tutorial](https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/MongrelYoutubeThumb.jpg)](https://youtu.be/zZV6yRjzHpU)

---

<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/MongrelScreenshot.png" height="638"></p>

---

# Mongrel VCV Rack User Manual

---

# Overview

**Mongrel** is a ring modulation drum synthesizer. Two low-frequency oscillators — YIP and YAP — run simultaneously in the 20–300Hz range. GROWL blends between the raw sum of both oscillators and their ring-modulated product. TAIL sets the decay time of an exponential envelope triggered by BANG!. The SNARL switch toggles both oscillators between triangle and square waveforms; a SNARL CV gate input overrides the switch.

> **Note:** The hardware version of this module differs from the VCV Rack version.

---

# CV Ranges

* **BANG! input:** Gate, threshold 0.5V (rising edge triggers envelope).
* **TAIL / YIP / GROWL / YAP CV:** ±5V, summed with the corresponding knobs.
* **SNARL CV:** Gate, threshold 0.5V. High signal enables square wave mode.
* **Output:** ±5V audio.

---

# Controls

### TAIL Knob
Controls the decay time of the envelope. Range is 5ms (fully counterclockwise) to 300ms (fully clockwise).

### YIP Knob
Sets the frequency of oscillator 1. Range is 20Hz to 300Hz.

### YAP Knob
Sets the frequency of oscillator 2. Range is 20Hz to 300Hz.

### GROWL Knob
Blends between the dry sum of YIP and YAP (fully counterclockwise) and the ring-modulated product of the two oscillators (fully clockwise). Intermediate positions mix both signals.

### SNARL Switch
- **Triangle (left):** Both oscillators output triangle waves.
- **Square (right):** Both oscillators output square waves.

---

# I/O

### Inputs

* **BANG!:** Gate input. A rising edge above 0.5V triggers the decay envelope and starts the sound.

* **TAIL CV:** Offsets the Tail knob. ±5V.

* **YIP CV:** Offsets the Yip frequency knob. ±5V.

* **GROWL CV:** Offsets the Growl knob. ±5V.

* **YAP CV:** Offsets the Yap frequency knob. ±5V.

* **SNARL CV:** Gate input. While high, switches both oscillators to square wave mode regardless of the SNARL switch position.

### Outputs

* **OUT:** Ring mod drum output. Envelope-shaped mix of YIP, YAP, and their ring-modulated product. Clamped to ±5V. The LED reflects the envelope level.

---

# Signal Chain

1. YIP oscillator at YIP frequency (triangle or square)
2. YAP oscillator at YAP frequency (triangle or square)
3. Ring modulator: YIP × YAP
4. GROWL blend: dry sum (YIP + YAP) ↔ ring mod
5. Exponential decay envelope triggered by BANG!
6. Output clamped to ±5V

---
