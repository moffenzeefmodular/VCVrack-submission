<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/SwarmScreenshot.png" width="300"></p>

---

# Swarm VCV Rack User Manual

*"Randomness without structure is noise. Structure without randomness is a metronome. Swarm lives in the middle."*

**— Ross Fish, 2026**

---

# Overview

**Swarm** is a self-clocked gate generator with four semi-random pattern outputs, a master clock output, and a square wave VCO. Each of the four channels runs an independent probabilistic sequencer that continuously mutates its own gate pattern at a low density. The four channels run at different time ratios relative to the master clock — they are always slightly out of phase with one another, which is the point. Flip a Loop switch to freeze a channel's current pattern and stop it from mutating.

The VCO output tracks the master clock speed and switches between a low audio range and a high audio range with the Range switch, making Swarm useful as a noise/texture source in addition to a rhythmic gate generator.

---

# CV Ranges

* **TIME CV:** ±5V. Summed with the Coarse and Fine knob values to set the master clock period.
* **WIDTH CV:** ±5V. Offsets the pulse width value.
* All gate outputs are 0V / 5V.
* VCO output is ±5V square wave.

---

# Controls

### COARSE Knob
Sets the base speed of the master clock. This is the primary tempo control. CV input and Fine knob are summed with this value.

### FINE Knob
Trims the clock speed up or down from the Coarse setting.

### WIDTH Knob
Sets the pulse width of the master clock and all four channel outputs.

### RANGE Switch
- **Slow:** Master clock period 150–500ms. VCO runs 40–200 Hz.
- **Fast:** Master clock period 15–150ms. VCO runs 1kHz–15kHz.

### LOOP 1–4 Switches
When a Loop switch is on, that channel's gate pattern stops mutating and loops its current sequence. The sequence length is also randomized at the moment you flip the switch on. Flip it off to resume mutation. Use this to capture and hold a pattern you like.

---

# I/O

### Inputs

* **TIME CV:** Offsets the master clock speed. ±5V.

* **WIDTH CV:** Offsets the pulse width. ±5V.

### Outputs

* **VCO:** Square wave oscillator tracking the master clock speed. In Slow range: 40–200 Hz. In Fast range: 1kHz–15kHz. Always active regardless of gate activity.

* **CH1 / CH2 / CH3 / CH4:** Probabilistic gate outputs. Each channel runs at a different fixed time ratio relative to the master clock (1.3×, 3.7×, 5.5×, 7.9×). Gate patterns mutate continuously at ~20% probability per step unless frozen with the Loop switch.

* **CLOCK:** The master clock gate output. Square wave at the period set by the Coarse, Fine, and Time CV controls. Pulse width set by the Width knob and CV.

---

# How the Pattern Generation Works

Each channel maintains a 64-step gate buffer. On every step, there is a ~20% chance that one randomly selected step in the current sequence will be flipped. This means the pattern is always slowly drifting — occasionally stepping into new territory, occasionally stepping back. At any given moment the density of active gates is statistically around 20%.

When a Loop switch is engaged, mutation stops and the sequence length is locked to a new random value between 1 and 64. This creates an opportunity to capture momentary patterns that would otherwise drift away.

---
