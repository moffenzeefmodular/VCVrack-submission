<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TheRunnerScreenshot.png" width="300"></p>

---

# The Runner VCV Rack User Manual

*"A drone that doesn't sit still. A machine that breathes."*

**— Ross Fish, 2026**

---

# Overview

**The Runner** is a five-voice harmonic drone synthesizer. This VCV Rack version is a Eurorack fantasy of the original hardware synth, created in collaboration with **Boy Harsher** as part of the promotional campaign for their 2023 film of the same name.

The module generates a stack of pulse wave oscillators tuned to harmonically related intervals above and below the PITCH frequency. The ANIMATE control sets the depth of a random drift (drunk walk) LFO that continuously modulates the pulse width of all voices and the filter cutoff simultaneously. A state-variable lowpass filter with resonance shapes the harmonic content, and an optional chorus effect adds width and movement. All major parameters have dedicated CV inputs.

> **Note:** The hardware version of this module differs from the VCV Rack version. The hardware used MIDI for pitch input; the VCV Rack version uses a 1V/Oct CV input.

> **Note:** The hardware version of this module differs from the VCV Rack version.

---

# CV Ranges

* **PITCH (1V/Oct):** 0–8V. Each volt raises the pitch by one octave.
* **VOLUME CV / GAIN CV / ANIMATE CV / HARMONICS CV / CUTOFF CV / RESONANCE CV:** ±10V (normalized to a 0–1 control range internally).
* **CHORUS CV:** >2.5V enables chorus, overriding the switch.
* **Output:** ±10V audio.

---

# Controls

### PITCH Knob
Sets the base frequency of the fundamental oscillator. Range is 13.75Hz (A0) to 440Hz (A4), spanning approximately two octaves of usable drone territory. The four harmonic voices are always locked to the fundamental: octave below, octave above, a twelfth (P12), and two octaves above.

### NOTESHZ Switch
- **Off:** Pitch is continuous, tracking the knob and CV smoothly.
- **On:** Pitch is quantized to the nearest semitone.

### ANIMATE Knob
Controls the depth of the random drift LFO. At zero there is no drift — pulse width and filter cutoff are static. As Animate increases, the LFO wanders more aggressively, breathing life into the drone.

### RANGE Switch
- **Low:** Slower animate drift speed.
- **High:** Faster animate drift speed.

### HARMONICS Knob
Crossfades additional harmonic voices into the mix. At fully counterclockwise, only the fundamental is audible. Turning clockwise progressively adds the octave below, then the octave above, then the P12 (octave + fifth), then the double octave.

### CUTOFF Knob
Sets the lowpass filter cutoff frequency. Range is 20Hz to 5kHz. The Animate LFO also modulates the cutoff slightly regardless of the Animate depth setting.

### RESONANCE Knob
Sets the filter resonance. Range is 0–90%. Does not self-oscillate.

### GAIN Knob
Pre-filter input gain. Range is 1x to 20x. Increasing gain drives the oscillator mix harder into the filter for overdrive and harmonic saturation.

### VOLUME Knob
Output level. Full counterclockwise is silence; full clockwise is maximum output.

### CHORUS Switch
- **Off:** No chorus effect.
- **On:** Enables a subtle chorus/flanger effect (7.5ms base delay, 0.4Hz LFO, 50% wet).

---

# I/O

### Inputs

* **PITCH (1V/Oct):** 1V/Oct CV input. 0–8V range. Adds to the Pitch knob.

* **VOLUME CV:** Offsets the Volume knob. ±10V.

* **GAIN CV:** Offsets the Gain knob. ±10V.

* **ANIMATE CV:** Offsets the Animate knob. ±10V.

* **CHORUS CV:** >2.5V enables chorus, overriding the switch position.

* **HARMONICS CV:** Offsets the Harmonics knob. ±10V.

* **CUTOFF CV:** Offsets the Cutoff knob. ±10V.

* **RESONANCE CV:** Offsets the Resonance knob. ±10V.

### Outputs

* **OUT:** Five-voice harmonic drone, filtered and shaped. Clamped to ±10V.

### LEDs

* **Animate LED:** Reflects the current amplitude of the Animate LFO drift.

---

# Signal Chain

1. Five pulse wave oscillators: fundamental, octave below, octave above, P12, double octave
2. HARMONICS crossfade blends voices into the mix
3. GAIN amplification (1x–20x)
4. State-variable lowpass filter (Cutoff + Resonance), modulated by Animate LFO
5. Optional chorus effect (7.5ms delay, 0.4Hz LFO)
6. High-pass filter at 20Hz (DC removal)
7. VOLUME output level

---

# Tips

- ANIMATE fully clockwise + RANGE High: aggressively drifting, unstable drone that never repeats.
- HARMONICS at halfway: a fuller, chorused quality without overwhelming the fundamental.
- High GAIN + high RESONANCE: self-distorting filter sweeps for industrial character.
- Slowly automate PITCH CV for barely-there microtonal drift in the fundamental.
- Patch CUTOFF CV to a slow envelope for a bloom effect on each section.

---
