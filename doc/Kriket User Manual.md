<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/KriketScreenshot.png" height="638"></p>

---

# Kriket VCV Rack User Manual

*"The cricket doesn't know music theory. It knows one frequency and it commits to it completely."*

**— Ross Fish, 2026**

---

# Overview

**Kriket** is a four-voice pitched noise percussion synthesizer. Each voice generates a bandpass-filtered square wave in the range of 1kHz–5kHz, triggered by a gate input. All four voices sum to a single output. The module is designed for high-frequency percussive tones — clicks, snaps, chirps, and digital artifacts. A shared CV input shifts all four voices simultaneously in 1V/octave.

---

# CV Ranges

* **PITCH knobs:** Each knob sweeps from 1kHz to 5kHz.
* **CV input:** 0–5V. Scales all four frequencies upward by 1V/octave. At 0V the pitches are set entirely by the knobs. Clamped to the 1kHz–5kHz range.
* **BANG inputs:** Gate inputs. Threshold is 0.5V.
* **Output:** ±5V audio.

---

# Controls

### PITCH 1 / 2 / 3 / 4
Sets the center frequency of each voice's bandpass filter and the frequency of its square wave oscillator. Range is 1kHz (fully counterclockwise) to 5kHz (fully clockwise). Default is 1kHz.

---

# I/O

### Inputs

* **BANG! 1–4:** Gate inputs for each voice. While the gate is high, that voice's oscillator contributes to the output. The oscillator runs continuously — the gate acts as an on/off switch rather than a trigger.

* **CV:** A shared pitch CV input. Applies a 1V/octave upward shift to all four voices simultaneously. Range 0–5V. Useful for melodic control over the entire instrument at once.

### Outputs

* **OUT:** Summed audio output of all active voices. Clamped to ±5V.

---

# Signal Chain (per voice)

1. Square wave oscillator at the frequency set by Pitch knob + CV offset
2. High-pass filter at 100 Hz (removes DC and sub content)
3. Low-pass filter with a cutoff set to the highest active voice frequency (reduces aliasing)
4. Output is gated on/off by the BANG! input
5. All four voices summed and scaled

---

# Tips

- Tune all four voices to the same frequency for a denser, phase-cancellation-heavy tone.
- Use different pitches on each voice and trigger them with rhythmic patterns for polyrhythmic high-frequency percussion.
- Patch a slow LFO into the CV input to sweep all voices together.
- At very high frequencies (near 5kHz) the voices produce sharp clicking sounds useful as hi-hat or transient replacements.

---
