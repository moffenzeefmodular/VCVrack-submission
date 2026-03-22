<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/CountScreenshot.png" width="300"></p>

---

# Count VCV Rack User Manual

*"A clock is an argument about what matters. Every division is a vote."*

**— Ross Fish, 2026**

---

# Overview

**Count** is a self-contained clock generator with adjustable speed, pulse width, and four divided outputs. A single master oscillator drives everything. The divided outputs fire at even multiples of the master clock period, configurable via the Top/Bottom and Left/Right switches. A one-shot Start pulse fires each time the clock is switched on, useful for resetting downstream sequencers.

---

# CV Ranges

* **TIME CV input:** ±5V. Adds to the coarse frequency setting. Positive voltage speeds up the clock; negative voltage slows it down.
* **WIDTH CV input:** ±5V. Adds to the pulse width knob value.
* All outputs are 0V / 5V gates.

---

# Controls

### COARSE
Sets the base clock speed. The knob uses a logarithmic-style response so the useful range spans from slow rhythmic pulses to fast audio-rate clocking.

### FINE
Trims the clock speed up or down by up to ±5ms from the coarse setting. Use this for subtle tempo adjustments without disturbing the coarse setting.

### WIDTH
Sets the pulse width of all outputs. At 50% the gate is on for exactly half of each clock period. CV input is summed with the knob position.

### RANGE Switch
Selects between two speed ranges:
- **Slow:** ~500ms to ~3000ms period (sub-Hz tempos)
- **Fast:** ~25ms to ~500ms period (rhythmic to near-audio)

### START/STOP Switch
Starts and stops the clock. When switched to Stop, all outputs go low and the internal counters reset. When switched to Start, a one-shot 100ms pulse fires on the START output and the clock begins running from the first beat.

### UPBEAT / DOWNBEAT Switch
- **Downbeat:** Divided outputs fire on beat 1 of each division cycle.
- **Upbeat:** Divided outputs are delayed by one master clock pulse, shifting the accent off the downbeat.

### TOP / BOTTOM Switch
Selects the division grid:
- **Bottom:** Divisions at ×4, ×8, ×12, ×16 of the master clock
- **Top:** Divisions at ×6, ×10, ×14, ×18 of the master clock

### LEFT / RIGHT Switch
Multiplies all division values by 4:
- **Left:** Standard division values (×4/8/12/16 or ×6/10/14/18)
- **Right:** Quadrupled division values (×16/32/48/64 or ×24/40/56/72)

---

# I/O

### Inputs

* **TIME CV:** Offsets the clock frequency. ±5V range summed with the Coarse knob.

* **WIDTH CV:** Offsets the pulse width. ±5V range summed with the Width knob.

### Outputs

* **START:** Fires a single 100ms gate pulse each time the clock is switched from Stop to Start. Useful for resetting sequencers or triggering one-shot events at patch startup.

* **MAIN:** The master clock gate output. Pulse width is controlled by the Width knob and CV.

* **DIVIDED 1–4:** Clock gates at the subdivisions determined by the Top/Bottom and Left/Right switches. Each output has an LED above it that illuminates while the gate is high.

---

# Division Table

| Setting | DIV 1 | DIV 2 | DIV 3 | DIV 4 |
|---|---|---|---|---|
| Bottom / Left | ÷4 | ÷8 | ÷12 | ÷16 |
| Top / Left | ÷6 | ÷10 | ÷14 | ÷18 |
| Bottom / Right | ÷16 | ÷32 | ÷48 | ÷64 |
| Top / Right | ÷24 | ÷40 | ÷56 | ÷72 |

---
