[![Video Tutorial](https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/MuskratYoutubeThumb.jpg)](https://youtu.be/h4xqRPyOV_s)

---

<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/MuskratScreenshot.png" height="638"></p>

---

# Muskrat VCV Rack User Manual

---

# Overview

**Muskrat** is a lo-fi wavetable drum module with four synthesis algorithms selectable via the SELECT knob. BANG! triggers a decay envelope, and TAIL with the three-position RANGE switch control the decay time. The three performance knobs — SCRATCH, DIG, and CHEW — have different roles depending on the active algorithm, and the RATSWITCH toggle modifies behavior within each mode.

> **Note:** The synthesis algorithm selector (Muskrat / FM / PD / Granular) was originally offered as part of the **Rat Nest** expander hardware module. In the VCV Rack version, all four modes are fused directly into the Muskrat module.

> **Note:** The hardware version of this module differs from the VCV Rack version.

---

# CV Ranges

* **BANG! input:** Gate, threshold 0.5V (rising edge triggers envelope).
* **SCRATCH CV / DIG CV / CHEW CV:** ±5V, summed with the corresponding knobs.
* **RATSWITCH Gate:** Gate, threshold 0.5V. High signal enables Ratswitch mode.
* **Output:** ±5V audio.

---

# Controls

### TAIL Knob
Controls the decay time within the range set by the RANGE switch. Counterclockwise is shortest; clockwise is longest.

### RANGE Switch
Three-position switch setting the decay time range:
- **Fast:** 5ms – 30ms
- **Medium:** 30ms – 200ms
- **Slow:** 200ms – 4000ms

### SCRATCH Knob
Selects the wavetable in Muskrat mode (1 of 24 lo-fi wavetables) and the grain source in Granular mode (1 of 16 grain wavetables). Has no direct effect in FM or PD modes.

### DIG Knob
In Muskrat mode: controls wavetable playback length and wavefold distortion threshold.
In FM mode: modulator pitch.
In PD mode: octave multiplier (1–8x).
In Granular mode: grain window size.

### CHEW Knob
Controls the base frequency/pitch across all four algorithms.

### SELECT Knob
Selects the synthesis algorithm:
- **1 — Muskrat:** Lo-fi wavetable playback with wavefolding distortion.
- **2 — FM:** Frequency modulation synthesis using two triangle oscillators.
- **3 — PD:** Phase distortion synthesis using sawtooth/triangle waveforms.
- **4 — Granular:** Reverse/forward grain playback from lo-fi grain wavetables.

### RATSWITCH
- **Off:** Standard forward behavior across all algorithms.
- **On:** In Granular mode, reverses grain playback direction and adds noise. In FM and PD modes, modifies the modulation frequency relationship.

---

# I/O

### Inputs

* **BANG!:** Gate input. A rising edge above 0.5V triggers the decay envelope.

* **SCRATCH CV:** Offsets the Scratch knob. ±5V.

* **DIG CV:** Offsets the Dig knob. ±5V.

* **CHEW CV:** Offsets the Chew knob. ±5V.

* **RATSWITCH Gate:** Enables Ratswitch mode while the gate is high. Overrides the RATSWITCH switch.

### Outputs

* **OUT:** Audio output of the selected algorithm, shaped by the decay envelope. Clamped to ±5V. The LED reflects the envelope level.

---
