<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/MitoScreenshot.png" height="638"></p>

---

# Mito VCV Rack User Manual

*"Six voices, one clock. Every division is its own little world."*

**— Ross Fish, 2026**

---

# Overview

**Mito** is a six-channel clock divider with swing and pulse width control. Patch a clock into BANG! and each of the six channels outputs a gate at a division of that incoming clock — from every pulse (÷1) to every sixteenth pulse (÷16). Each channel has its own Division knob, mute switch, and CV input for voltage-controlled division. Swing offsets every other beat, and Width sets how long the gate stays high within each division window. A RESET input snaps all counters back to zero.

---

# CV Ranges

* **BANG! input:** Gate, threshold 0.5V (rising edge).
* **RESET input:** Gate, threshold 0.5V (rising edge).
* **Division CV inputs:** ±5V. Summed with the division knob as an offset.
* **SWING CV:** ±5V. Summed with the Swing knob.
* **WIDTH CV:** ±5V. Summed with the Width knob.
* All gate outputs are 0V / 5V.

---

# Controls

### DIVISION Knobs (1–6)
Sets the clock division for each channel. Range is ÷1 (fires every clock pulse) to ÷16 (fires every sixteenth clock pulse). Knobs snap to integer values.

### MUTE Switches (1–6)
- **Up (Unmute):** Channel is active.
- **Down (Mute):** Channel output is silenced. LED goes dark.

### SWING Knob
Adds a swing offset (0–100ms) to alternating beats. At zero, timing is straight. As swing increases, every other division fires slightly late, creating a pushed groove feel. The amount of swing is in milliseconds relative to the incoming clock period.

### WIDTH Knob
Sets the pulse width of all output gates as a fraction of the clock period. Range is approximately 5%–25% of the clock duration.

---

# I/O

### Inputs

* **BANG!:** The master clock input. Each rising edge advances the internal counter. The tempo of the output gates is entirely determined by the incoming clock.

* **RESET:** A rising edge resets the master counter to zero, re-syncing all six channels to the downbeat.

* **CH1–6 CV:** Per-channel voltage control over division amount. ±5V is summed with the division knob as an offset.

* **SWING CV:** Offsets the swing amount. ±5V summed with the Swing knob.

* **WIDTH CV:** Offsets the pulse width. ±5V summed with the Width knob.

### Outputs

* **CH1–6:** Gate outputs at the division set by each channel's knob and CV. 0V / 5V. Each has an LED that illuminates while the gate is high.

---

# Tips

- Set multiple channels to the same division for synchronized gates that can be individually muted.
- Use prime-number divisions (e.g., 3, 5, 7, 11) across channels to create polyrhythmic patterns that take a long time to repeat.
- Patch the Width CV to a slow LFO to gradually breathe the gate length in and out across all channels.
- Use RESET from a bar-length trigger to keep long polyrhythmic patterns phase-locked to your song structure.

---
