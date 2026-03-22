[![Video Tutorial](https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/BadIdea1800YoutubeThumb.jpg)](https://youtu.be/rJQTEyduKEI)

---

<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/BadIdea1800Screenshot.png" height="638"></p>

---

# Bad Idea #1800-Call-Yer-Mum VCV Rack User Manual

---

# Overview

**Bad Idea #1800-Call-Yer-Mum** is a DTMF tone generator arranged like a telephone keypad. In Original Firmware mode, holding a gate input high plays the dual-tone DTMF frequency pair for that key (the authentic modem and phone handshake tones). In Alt Firmware mode, a rising edge on any input triggers a one-shot sample from a selectable drum kit. All 12 voices sum to a single audio output.

> **Note:** The hardware version of this module differs from the VCV Rack version.

---

# CV Ranges

* **Gate inputs (1–9, *, 0, #):** Threshold 0.5V. In Original Firmware: sustained gate = tone plays. In Alt Firmware: rising edge = sample trigger.
* **Output:** ±5V audio.

---

# Controls

There are no knobs or switches on this module. All control is through the gate inputs and the context menu firmware selector.

### Context Menu — Firmware
- **Original Firmware:** Each gate input activates the standard DTMF tone pair for that key. All active keys mix to the output simultaneously.
- **Alt Firmware:** A submenu of drum kits. Each kit maps one sample per key. Rising edges trigger one-shot sample playback.

---

# I/O

### Inputs

* **1, 2, 3 / 4, 5, 6 / 7, 8, 9 / *, 0, #:** Gate inputs arranged in standard telephone keypad order. Each input has an LED above it that illuminates while the gate is active or a sample is playing.

### Outputs

* **OUT:** Summed audio output of all active voices. Clamped to ±5V.

---

# How DTMF Works

DTMF (Dual-Tone Multi-Frequency) encodes each keypad symbol as the simultaneous sum of two sine waves — one from a row frequency and one from a column frequency. The standard telephone grid uses four row frequencies (697Hz, 770Hz, 852Hz, 941Hz) and three column frequencies (1209Hz, 1336Hz, 1477Hz). Bad Idea #1800 implements all twelve standard combinations.

---
