<a href="https://youtu.be/oMb-bblWSxk" target="_blank"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/GmoYoutubeThumb.jpg" alt="Video Tutorial"></a>

---

<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/GmoScreenshot.png" height="638"></p>

---

# GMO VCV Rack User Manual

---

# Overview

**GMO** is a lo-fi wavetable rompler. The original firmware contains eight recordings of metal clangs stored as short wavetables. BANG! triggers one-shot playback; the LOOP switch enables continuous looping. SPECIMEN selects the wavetable, SPEED sets the playback rate, and HEAD and TAIL define the start and end points within the wavetable. Alternate sample banks covering drum machines, percussion, and artist series content are available via the context menu.

> **Note:** The hardware version of this module differs from the VCV Rack version.

---

# CV Ranges

* **BANG! input:** Gate, threshold 0.5V. Rising edge triggers one-shot playback.
* **LOOP Gate input:** Gate, threshold 0.5V. High signal enables looping mode.
* **SPECIMEN CV / SPEED CV / HEAD CV / TAIL CV:** ±5V, summed with the corresponding knobs.
* **Output:** ±5V audio.

---

# Controls

### SPECIMEN Knob
Selects which wavetable is played. In the original firmware, range is 1–8. In alternate banks, range adjusts to the number of wavetables in that bank. Snaps to integer values.

### SPEED Knob
Controls playback speed and therefore pitch. Counterclockwise is slower; clockwise is faster. Speed range depends on the selected bank (the original firmware spans approximately 0.05x to 3x).

### LOOP Switch
- **Off:** One-shot mode. BANG! triggers a single pass from HEAD to TAIL.
- **On:** Loop mode. The wavetable plays continuously between HEAD and TAIL.

### HEAD Knob
Sets the start point of playback within the wavetable. Counterclockwise is the beginning; clockwise moves the start point toward the end.

### TAIL Knob
Sets the end point of playback. Must be positioned after HEAD — a minimum gap is enforced between them.

---

# I/O

### Inputs

* **BANG!:** Gate input. Rising edge begins one-shot playback from the HEAD point. Has no effect in Loop mode (looping runs continuously).

* **SPECIMEN CV:** Offsets the Specimen knob. ±5V.

* **SPEED CV:** Offsets the Speed knob. ±5V.

* **LOOP Gate:** Enables loop mode while the gate is high. Overrides the LOOP switch.

* **HEAD CV:** Offsets the Head knob. ±5V.

* **TAIL CV:** Offsets the Tail knob. ±5V.

### Outputs

* **OUT:** Wavetable audio output. Passes through a 200Hz high-pass filter. Clamped to ±5V. The LED reflects the current output level.

---

# Context Menu — Sample Bank

Right-click to access the Sample Bank menu.

- **Original Firmware:** Eight lo-fi metal clang wavetables.
- **Drum Machines / Percussion / Artist Series / Other:** Alternate wavetable banks. Each bank maps its wavetables to the SPECIMEN knob range.

---
