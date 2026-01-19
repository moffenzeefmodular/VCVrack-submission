[![Video Tutorial](https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TantzYoutubeThumb.jpg)](https://youtu.be/oB4lEuMjE34)

---

# Tantz! VCV Rack User Manual 

"Story" **-Ross Fish January 2026**

# Overview

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TantzScreenshot.png" width="500">

Although the knobs are labeled kick snare hat etc, blah blah blah

---

# Klezmer Rhythm References

* **Bulgar** 
* **Araber**
* **Terkisher**
* **Hora**
* **Zhok**
* **In Zibn** 

---

# CV Ranges 

### All CV inputs respond to -5 V…+5 V. Every knob is treated as an offset: with the knob at 50%, a -5 V signal will turn it all the way left, and a +5 V signal will turn it all the way right. All logic inputs (labeled CLK IN) ignore pulse width and trigger on a rising-edge signal. All logic outputs are 0-5v. 

---

# Global

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TantzGlobal.png" width="250">

This section controls the functionality of the entire **Tantz!** module.

* **RUN:** A global Start/Stop for **Tantz!** with an associated bipolar CV input. Sequence will not run unless an external clock is present at **CLK. IN**
* **STYLE:** Select the global rhythmic style for **Tantz!** (in order): Bulgar #1, Bulgar #2, Araber, Terkisher, Hora/Zhok, In Zibn. 
* **SWING:** Global swing amount with an associated bipolar CV input. 
* **PW:** Global pulse width for all gate outputs.

---

# Individual Tracks 

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TantzTracks.png" width="400">

* **KNOB:** Pattern select 1-8 for one of the preset rhythms as determined by **Style.**
* **SWITCH:** Mute on/off for the associated channel.

---

# I/O 

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TantzIO.png" width="600">

* **RESET:** A gate **output** that fires one step before step 1 of the sequence. This reset is meant to go to other sequencers down the line to keep them in sync with **Tantz!.**
* **CLK IN:** A rising edge trigger input that is required for Tantz! to run. 
* **CV INPUTS:** Bipolar CV inputs for pattern selection per channel.
* **GATE OUTPUTS:** Associated gate outputs for each channel. 

---