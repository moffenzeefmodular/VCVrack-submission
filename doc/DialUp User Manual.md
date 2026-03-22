<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/DialUpScreenshot.png" height="638"></p>

---

# Dial-Up VCV Rack User Manual

*"The most important sound of an entire generation of internet users, reduced to a single knob."*

**— Ross Fish, 2026**

---

# Overview

**Dial-Up** is a bytebeat percussion synthesizer that generates sounds reminiscent of early digital noise, modem handshakes, and lo-fi computer audio. A bytebeat algorithm produces a 1-bit waveform that is filtered and shaped by a decay envelope triggered by a gate input. The Modem knob controls the algorithm's time constant, sweeping through a wide range of tones from low clunks to high-pitched screeches. A Tail knob and Range switch control how long the sound decays after each trigger.

---

# CV Ranges

* **BANG! input:** Gate, threshold 0.5V (rising edge triggers envelope).
* **MODEM CV:** ±5V. Summed with the Modem knob.
* **Output:** ±5V audio.

---

# Controls

### TAIL Knob
Controls the decay time of the envelope. The actual time range depends on the RANGE switch setting. Counterclockwise is the shortest decay within the selected range; clockwise is the longest.

### RANGE Switch
A three-position switch that sets the decay time range:
- **Fast:** 5ms – 30ms (sharp transient clicks and snaps)
- **Medium:** 30ms – 200ms (standard percussion)
- **Slow:** 200ms – 4000ms (long sweeps and sustained tones)

### MODEM Knob
Controls the character of the bytebeat algorithm — effectively the "note" or pitch of the generated sound. Sweeps through 256 discrete tonal variations. Small movements produce subtle changes; large sweeps cross dramatically different sonic territory. A small amount of random jitter is applied to the control value to keep the sound from sitting static between triggers.

---

# I/O

### Inputs

* **BANG!:** Gate input. A rising edge above 0.5V triggers the decay envelope and starts the sound. The bytebeat oscillator runs continuously; the envelope controls whether you hear it.

* **MODEM CV:** Offsets the Modem knob. ±5V.

### Outputs

* **OUT:** The processed audio output. The bytebeat signal passes through a 10kHz low-pass filter and a 100Hz high-pass filter before being shaped by the decay envelope. Clamped to ±5V. The LED above the output jack reflects the current envelope level.

---

# How Bytebeat Works

Bytebeat is a class of audio algorithm from the early 2010s that generates sound by running simple integer math on a continuously incrementing time variable. The result is a 1-bit waveform rich in harmonics and aliasing artifacts. Dial-Up uses a single bytebeat formula with the Modem knob selecting which "note" in the algorithm's output spectrum is active. The time variable advances at a rate proportional to the Modem setting, which determines both the pitch and the timbral character of the output simultaneously.

---

# Tips

- Short Range + high Tail: punchy digital transients with a click-y tail.
- Slow Range + high Modem CV: sweeping sci-fi tones and grinding noise bursts.
- Patch a sequencer CV into MODEM CV to step through different tonal characters on each hit.
- Multiple instances of Dial-Up at different Modem settings, triggered in sequence, create a lo-fi drum machine character.

---
