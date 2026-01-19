[![Video Tutorial](https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/KleztizerYoutubeThumb.jpg)](https://youtu.be/PGaFpK83QaM)

# Kleztizer VCV Rack User Manual 

### "Understanding modal music and music theory is a very dense subject, especially for the unitiated. My goal when making this module was to eschew the need to understand anything about music theory and get whoever is using this module to generate *something* musical even if the result could be percieved as "incorrect" by the trained ear. When using **Kleztizer**, I want you to feel like an ignorant child pushing buttons and turning knobs until you like what you hear even if you don't necessarily understand what the hell is going on. If you do in fact know what's going on and you do understand music theory, then this device hopefully is a very useful palette for exploring these concepts. I admit, this module creates more problems than it solves, but I'm pretty sure I'm the only moron in existence to try and translate Klezmer music theory to an electronic musical instrument. If you're reading this and you did in fact beat me to the chase, please send me an email. I would really like to have a beer with you. " -Ross Fish January 2026 

---

# The History of Klezmer Music

Here are some excellent resources about the rich history of Klezmer music:

### [Roten, Hervé. “Klezmer Music: from the Past to the Present.” Institut Européen des Musiques Juives](https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/HistoryOfKlezmer.pdf)

### [Punzi Punzi, Ludovica. “On the Confluence of Klezmer and Moldovan Music: A Short Review of Some of the Writings of Dr. Walter Zev Feldman.” Early Music Seattle, 17 Jan. 2023](https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/KlezmerAndMoldavanMusic.pdf)

---


# What is Modal Music? 

Modal music uses specific scales (modes), rather than just major/minor keys, emphasizing a distinct atmosphere by treating all notes within the mode as equally important, often featuring fewer chord changes, less tension-building (like avoiding tritones), and a focus on melodic movement within the mode's "home" area, seen in Gregorian chants, Debussy, and Miles Davis's Kind of Blue. It contrasts with tonal music's hierarchical harmony (tonic, dominant, etc.) by allowing chords to exist more independently, creating a floating, less goal-oriented sound. 

**Key Characteristics:**

* **Scale-Based:** Built from specific modes (Dorian, Phrygian, Lydian, etc.) with unique flavors, not just major/minor. Klezmer music specifically has its own Modes common to the genre. 
* **Equal Notes:** Notes within a mode have less fixed roles; they don't strongly pull to a tonic.
* **Limited Harmony:** Often uses fewer chords, sometimes just one or two, with less emphasis on traditional dominant-tonic resolution (V-I).
* **Atmosphere:** Creates a specific mood, sometimes static or floating, with less dramatic tension and release than tonal music.
* **Tonal Center:** Still has a "home" note (tonic) but doesn't rely on functional harmony to get there. 

---

# Klezmer Music Theory 

Going in depth on the music theory of Klezmer music is best left to musicologists. This subject is extraordinarily dense, as the musical tradition is over 500 years old. Liturgical Jewish music is even older (5786 years by our own accounting). If I were to begin explaining this to you, I probably would do a terrible job. That being said, here are a few excellent resources on the subject:

### [Ditzian, Jason. “Klezmer Scales.” Klezmer.com, 27 Nov. 2023](https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/DitzianKlezmerScales.pdf)

### [Horowitz, Josh. “The Main Klezmer Modes: Ahava Rabboh.” KlezmerShack, 1999](https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/HorowitzTheMainKlezmerModes.pdf)

### [Malin, Yonatan, and Daniel Shanahan. “Modes in Klezmer Music: A Corpus Study Based on Beregovski’s Jewish Instrumental Folk Music.” Music Theory Online, vol. 31, no. 3, 2025](https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/MalinModesinKlezmerMusic.pdf)

---

# Overview

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/KleztizerScreenshot.png" width="500">

**Kleztizer** is a combination module consisting of two quantizers and a chord generator. Both the chord generator and the quantizers follow the "rules" of modal harmony, specifically as they pertain to modes common in Klezmer music. Setting the **Key** and **Mode** dictate the "rules" that the subsequent sections of the module follow. 

---

# CV And Gate Ranges

### All CV inputs respond to -5 V…+5 V. Every knob is treated as an offset: with the knob at 50%, a -5 V signal will turn it all the way left, and a +5 V signal will turn it all the way right. All logic outputs are 0-5v. The button logic inputs are rising edge triggers and ignore pulse width. All Quantized outputs are scaled for 1v/octave pitch tracking.

---

# Key and Mode 

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/KleztizerKeyMode.png" width="500">

The **Key** and **Mode** parameters dictate the root pitch and the associated musical mode to which the chord and lead sections assign their scales.

* **KEY:** Select the root pitch of the musical mode in semitones over a single octave.  
* **MODE:** Select the musical mode (in order): Freygish, Mi Sheberach, Adonai Malakh, Magein Avot, Harmonic Minor.  

---

# Chords 

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/KleztizerChords.png" width="500">

Inspired by the Omnichord and many unusual organs from the 70s and 80s, this button interface selects the chord triad of the associated scale degree in each musical mode. The chords change based on the settings of **Key** and **Mode**. Depending on the musical **Mode** selected, when playing a traditional melody or sending voltages into the **Lead** sections, some chords might sound "correct" while others may not. Since we're in a modular environment and the concept of melody and meter is somewhat flexible, I'll leave that up to you to decide.

* **CHORD SELECT BUTTONS:** Select the chord triad corresponding to the chosen scale degree.  
* **BUTTON TRIGGER INPUTS:** Directly trigger a chord selection with a gate or trigger.  
* **CHORD CV INPUT:** A bipolar CV input for selecting a chord.  
* **VOICING:** This knob shifts the voice allocation for the highest and lowest pitches of the triad: Closed (normal triad), Drop 2 (2nd note is one octave down), Drop 3 (3rd note is one octave down), Open (1st note is one octave down, 3rd note is one octave up).  
* **INVERSION:** This inverts the chord after the **Voicing** has been selected in the following order: Root (no inversion), 1st (lowest note becomes highest), and 2nd (highest note becomes lowest). Although inverting chords after changing the **Voicing** can sometimes yield the same musical results as adjusting the **Voicing** knob itself, this knob acts as a way to modulate around a **Voicing** home base that you like.

---

# Lead Channels 
<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/KleztizerLead.png" width="500">

There are two **Lead** channels in Kleztizer that behave identically. The **Lead** channels are quantizers that adjust their scale and root pitch based on the **Key** and **Mode** parameters. Within those settings, the user can choose where the quantizer scale starts using **Note Offset**. They can modify the overall note range with **CV Amount** and transpose the final result with the **Octave** switch. Each time a new note is generated by the quantizer, a logic signal is generated at the **Trig/Gate** output. This logic output is intended for triggering an amplitude envelope. The idea is that you can send a nonlinear voltage source, such as an LFO, into the quantizer and trigger discrete notes each time the quantizer outputs a new note.

* **NOTE OFFSET:** Sets the starting scale degree of the quantizer. It has an overall range of -2…+2 V, corresponding to -2 to +2 octaves from the designated root pitch.  
* **CV:** A voltage attenuator pre-quantizer that limits the overall note range of the **Lead**.  
* **OCT.:** An octave transpose post-quantizer that shifts the sequence an additional -2 or +2 octaves.  
* **TRIG/GATE:** Changes the behavior of the **TRIG/GATE** logic output. In **Trig** mode, a short 5 ms pulse is triggered each time a new note is generated. In **Gate** mode, a 50% gate is generated for the duration between the previous note and the new note.  
* **TRIG/GATE OUTPUT:** The logic output for the **Lead** sections.  

---

# Quantizer Outputs

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/KleztizerOutputs.png" width="500">

The outputs at the bottom of the module provide the quantized notes from both the **Chord** and **Lead** sections. These outputs connect to 1 V/Oct inputs on oscillators to produce musical pitches.

* **PEDAL:** Outputs a steady note that matches the **Key** knob, intended as a drone for the selected key. It is only affected by the **Key** knob and does not change with any other settings.  
* **BASS:** Outputs the tonic of the current **Chord**, one octave below the other chord outputs. It is unaffected by **Voicing** and **Inversion**.  
* **1/POLY:** Can be used as a polyphonic output for all three chord voices or independently as the first voice in the chord.  
* **2:** The second voice of the selected chord, affected by **Voicing** and **Inversion**.  
* **3:** The third voice of the selected chord, affected by **Voicing** and **Inversion**.  
* **LEAD 1:** The quantized output of **Lead 1**.  
* **LEAD 2:** The quantized output of **Lead 2**.  
