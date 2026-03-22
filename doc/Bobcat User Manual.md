<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/BobcatScreenshot.png" width="300"></p>

---

# Bobcat VCV Rack User Manual

*"Not a snare drum. Not a tom. Something that lives in between and doesn't apologize for it."*

**— Ross Fish, 2026**

---

# Overview

**Bobcat** is a two-voice noise percussion synthesizer. Each voice generates bandpass-filtered white noise with a sample-and-hold clocking stage that controls the character of the noise texture (Hiss), shaped by an exponential decay envelope (Tail). Both voices sum to a single output. Bobcat is designed for snare, hi-hat, clap, and industrial percussion sounds.

The **SNARL** switch extends the decay range of both voices simultaneously from a short percussive mode into a long drone/swell mode.

---

# CV Ranges

* **BANG! inputs:** Gate, threshold 0.5V (rising edge triggers envelope).
* **TAIL CV inputs:** ±5V. Summed with the Tail knob.
* **HISS CV inputs:** ±5V. Summed with the Hiss knob.
* **Output:** ±5V audio.

---

# Controls

### TAIL 1 / TAIL 2
Controls the decay time of each voice's envelope. Counterclockwise is fast, clockwise is slow. In Short mode (SNARL off): 5ms–100ms. In Long mode (SNARL on): 5ms–2000ms.

### HISS 1 / HISS 2
Controls two things simultaneously: the clock speed of the sample-and-hold noise source (80Hz–8kHz) and the cutoff frequency of the bandpass filter (500Hz–10kHz). Counterclockwise produces low, gritty rumble. Clockwise produces bright, airy hiss.

### SNARL Switch
- **Short (left):** Both voices decay in 5ms–100ms range. Good for snappy percussion.
- **Long (right):** Both voices decay in 5ms–2000ms range. Good for swells, body, and sustained noise textures.

---

# I/O

### Inputs

* **BANG! 1:** Gate input for voice 1. A rising edge triggers voice 1's decay envelope.

* **TAIL 1 CV:** Offsets the Tail 1 knob. ±5V.

* **HISS 1 CV:** Offsets the Hiss 1 knob. ±5V.

* **BANG! 2:** Gate input for voice 2. A rising edge triggers voice 2's decay envelope.

* **TAIL 2 CV:** Offsets the Tail 2 knob. ±5V.

* **HISS 2 CV:** Offsets the Hiss 2 knob. ±5V.

### Outputs

* **BOBCAT:** Summed audio output of both voices. Clamped to ±5V. The LED illuminates proportionally to the combined envelope level.

---

# Signal Chain (per voice)

1. White noise generator
2. Sample-and-hold clocked at the rate set by the Hiss knob (80Hz–8kHz)
3. Biquad bandpass filter centered at the frequency set by the Hiss knob (500Hz–10kHz)
4. Exponential decay envelope triggered by BANG! input
5. Both voices summed and averaged

---

# Tips

- For a snare: Voice 1 low Hiss + medium Tail, Voice 2 high Hiss + short Tail. Trigger both from the same gate.
- For hi-hats: Use only Voice 1 with high Hiss and short Tail. A second instance of Bobcat works well for open/closed hat pairs.
- For body and texture: Flip SNARL to Long and increase Tail for evolving noise swells.
- Hiss CV from a sequencer creates pitched noise percussion — each step snaps the noise color to a different frequency band.

---
