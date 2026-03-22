<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/SimplifyScreenshot.png" height="638"></p>

---

# Simplify VCV Rack User Manual

---

# Overview

**Simplify** is a clock-gating sequencer with 137 preset 16-step patterns. Each pattern determines which steps of an incoming clock signal are passed to the output and which are silenced. The SELECT knob scrolls through all 137 patterns. BANG! advances the step counter on each rising edge; RESET returns to step 1. A SELECT CV input allows voltage-controlled pattern selection.

The full pattern table is documented in the [hardware user manual](https://www.moffenzeefmodular.com/s/MOF-SIMPLIFY-QUICKSTART-v10-892024.pdf).

> **Note:** The hardware version of this module differs from the VCV Rack version.

---

# CV Ranges

* **BANG! input:** Gate, threshold 0.5V (rising edge advances the step counter).
* **RESET input:** Gate, threshold 0.5V (rising edge resets to step 1).
* **SELECT CV:** ±5V, summed with the SELECT knob as an offset.
* **Output:** 5V gate (5ms pulse width) on active steps; 0V on muted steps.

---

# Controls

### SELECT Knob
Chooses one of 137 preset gate patterns. The knob snaps to integer values. Each pattern is a 16-step sequence of active and muted positions. Counterclockwise is pattern 1; clockwise is pattern 137.

---

# I/O

### Inputs

* **BANG!:** Clock input. Each rising edge above 0.5V advances the internal step counter by one. The pattern loops back to step 1 after step 16.

* **RESET:** Gate input. A rising edge above 0.5V resets the step counter to step 1 immediately.

* **SELECT CV:** ±5V offset to the SELECT knob. Allows voltage-controlled pattern selection.

### Outputs

* **OUT:** Gate output. On active steps, outputs a 5V gate for 5ms following the BANG! input. On muted steps, output stays at 0V. The LED illuminates on active steps.

---

# Tips

- Patch Simplify between a master clock and a drum voice to quickly audition rhythmic variations — just turn SELECT.
- Use RESET from a bar-length trigger to keep the pattern phase-locked to your song structure.
- Automate SELECT CV with a slow LFO or step sequencer for evolving rhythmic variation over time.
- Run multiple Simplify modules from the same clock with different patterns for independent muting layers across voices.

---
