<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/MoffenmixScreenshot.png" height="638"></p>

---

# Moffenmix VCV Rack User Manual

*"Every mixer is an opinion about loudness."*

**— Ross Fish, 2026**

---

# Overview

**Moffenmix** is a four-channel mixer with per-channel gain staging, variable gain range, mute switches, and a Link input that passes through at unity gain. Each channel runs through a soft-clipping distortion stage before volume attenuation, so cranking the gain into high ranges intentionally colors the signal. A single summed output is clamped to ±5V.

---

# CV Ranges

All inputs are audio-rate signals. Input and output voltage range is ±5V. There are no CV inputs on Moffenmix — all control is manual.

---

# Controls

Each of the four channels has the same set of controls:

### GAIN Knob (per channel)
Sets the pre-distortion gain for the channel. At maximum it applies the full gain multiplier determined by the Range switch. Default is 100% (unity within the selected range).

### RANGE Switch (per channel)
A three-position switch that sets the gain multiplier applied to the Gain knob:
- **1×:** Gain knob sweeps 0 to 1× (unity gain maximum)
- **5×:** Gain knob sweeps 0 to 5× gain
- **100×:** Gain knob sweeps 0 to 100× gain — use this range to overdrive signals and push hard into the soft clipper

### VOLUME Knob (per channel)
Attenuates the channel signal *after* the gain and distortion stage. This is the final level trim before summing. Default is 50%.

### MUTE Switch (per channel)
- **Up (Unmute):** Channel is active and contributes to the mix.
- **Down (Mute):** Channel output is silenced. The LED goes dark.

### LEDs
Each channel has an LED between the Range switch and Volume knob. Brightness reflects the post-distortion, pre-volume signal level.

---

# I/O

### Inputs

* **CH1 / CH2 / CH3 / CH4:** The four audio inputs. Each is independently processed through its channel's gain, range, and distortion settings.

* **LINK:** A fifth input that passes through at unity gain with no gain staging or distortion applied. Useful for chaining from another mixer or adding a pre-processed submix.

### Outputs

* **MIX:** The summed output of all four channels plus the Link input. Hard-clamped to ±5V.

---

# Signal Flow

For each channel:
1. Input signal is multiplied by Gain × Range multiplier
2. Soft clipping is applied (gradual saturation above ±5V)
3. Volume knob attenuates the result
4. All four channels plus Link are summed
5. Output is hard-clamped to ±5V

---
