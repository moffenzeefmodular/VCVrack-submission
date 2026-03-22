<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/DeviantScreenshot.png" width="300"></p>

---

# Deviant VCV Rack User Manual

*"It knows the limits. It just doesn't respect them."*

**— Ross Fish, 2026**

---

# Overview

**Deviant** is a dual channel random stepped CV generator with adjustable range and slew. On each rising edge of the BANG! input, a new random voltage is sampled uniformly within the window set by the TOP and BOTTOM knobs. The SMOOTH knob applies a slew limiter to the output, turning abrupt steps into gliding transitions. Each channel is independent, and Channel 2's BANG! normalizes to Channel 1's trigger when unpatched. Dual-color LEDs indicate positive (green) and negative (red) polarity on each output.

> **Note:** The hardware version of this module differs from the VCV Rack version.

---

# CV Ranges

* **BANG! inputs:** Gate, threshold 0.5V (rising edge triggers a new sample).
* **TOP CV / BOTTOM CV inputs:** ±5V, summed with the TOP and BOTTOM knobs.
* **Outputs:** ±5V.

---

# Controls

### TOP 1 / TOP 2
Sets the upper bound of the random voltage window for each channel. Range is -5V to +5V. If TOP is set below BOTTOM, the two are forced equal and only one voltage is possible.

### BOTTOM 1 / BOTTOM 2
Sets the lower bound of the random voltage window for each channel. Range is -5V to +5V.

### SMOOTH 1 / SMOOTH 2
Adds slew limiting to the output. At fully counterclockwise there is no slew — steps are instantaneous. Clockwise increases the slew rate, causing the output to glide toward each new value rather than jumping.

---

# I/O

### Inputs

* **BANG! 1:** Gate input for Channel 1. A rising edge above 0.5V generates a new random voltage.

* **TOP 1 CV:** Offsets the Top 1 knob. ±5V.

* **BOTTOM 1 CV:** Offsets the Bottom 1 knob. ±5V.

* **BANG! 2:** Gate input for Channel 2. Normalizes to BANG! 1 when unpatched.

* **TOP 2 CV:** Offsets the Top 2 knob. ±5V.

* **BOTTOM 2 CV:** Offsets the Bottom 2 knob. ±5V.

### Outputs

* **CH 1:** Random stepped CV for Channel 1. ±5V. Green LED = positive voltage; red LED = negative voltage.

* **CH 2:** Random stepped CV for Channel 2. ±5V. Green LED = positive voltage; red LED = negative voltage.

---

# Tips

- Set TOP and BOTTOM to the same value to use Deviant as a manual CV source that randomizes from a fixed point.
- Narrow the TOP/BOTTOM window and add SMOOTH for gently wandering melodic CV.
- Normalize both BANG! inputs and vary the TOP/BOTTOM settings per channel for correlated but differently-ranged random outputs.
- Patch TOP CV and BOTTOM CV from other voltage sources to dynamically shift the random window over time.

---
