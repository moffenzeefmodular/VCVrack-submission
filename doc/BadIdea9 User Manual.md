<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/BadIdea9Screernshot.png" height="638"></p>

---

# Bad Idea #9 VCV Rack User Manual

---

# Overview

**Bad Idea #9** is a hard-sync oscillator inspired by the original hardware module. A main oscillator and a sync oscillator run simultaneously; each time the sync oscillator completes a cycle it resets the main oscillator's phase, producing the metallic, clangorous timbres characteristic of hard sync. The single CV input acts as a voltage starve control, simultaneously gating the output amplitude and offsetting both oscillator frequencies — the lower the voltage, the more the sound collapses.

> **Note:** The hardware version of this module differs from the VCV Rack version.

---

# CV Ranges

* **PWR input:** 0–5V. Offsets both oscillator frequency controls and scales the output amplitude. At 0V the output is silent. At 5V the module operates at full amplitude with maximum frequency offset.
* **Output:** ±5V audio.

---

# Controls

### RUH Knob
Sets the base frequency of the main oscillator. Range is approximately 120Hz to 1885Hz, running counterclockwise to clockwise. The main oscillator frequency is always at least as high as the sync oscillator frequency.

### ROH Knob
Sets the base frequency of the sync oscillator. Range is approximately 10Hz to 3000Hz. The sync oscillator resets the main oscillator's phase on every cycle, locking the harmonic relationship between them.

---

# I/O

### Inputs

* **PWR:** Voltage starve input. 0–5V. Controls both the output amplitude and frequency offset of both oscillators. Patch a VCA envelope, LFO, or gate here to bring the module to life.

### Outputs

* **OUT:** Hard-sync oscillator audio output. Passes through a 1kHz high-pass filter before output. Amplitude is proportional to the PWR voltage. The LED reflects the current PWR level.

---

# Signal Chain

1. Sync oscillator running at the ROH-set frequency
2. Main oscillator running at the RUH-set frequency, phase-reset by the sync oscillator each cycle
3. Square wave output from the main oscillator
4. 1kHz high-pass filter
5. Amplitude scaled by PWR voltage

---
