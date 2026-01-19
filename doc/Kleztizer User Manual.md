[![Video Tutorial](https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/KleztizerYoutubeThumb.jpg)](https://youtu.be/PGaFpK83QaM)

# Kleztizer VCV Rack User Manual 

"I apologize in advance for waterboarding you with text for the next hour or so of your day. I have a very deep personal connection to this subject matter and am very passionate about it. If you don't give a shit and just want to figure out how to use this module, scroll down until you see the word **Overview.** If you do care about why someone would make a device as strange as this for free and then spend several hours on a Sunday putting together this documentation for you, then stick around for a long-winded, very self-indulgent and reflective essay.

So why **Klezmer?**

My grandparents were Holocaust survivors. I spent most of my life not knowing the full details of their story, as they seldom spoke about it and divulged very little detail to their children. They did, however, leave my family a lot of breadcrumbs — photographs, written notes, etc. About 5 years ago I became the family historian and started piecing their story together in an obsessive manner. I have this huge digital document that looks like the type of thing from an FBI office tying together a criminal conspiracy. I dug up a lot of incredible information about their story, and even found a missing family member that no one knew about. Learning what life was like in Europe for Jews before the war started has been cathartic for me. Being a Jew, especially one with such a close connection to the Holocaust, our stories are always drowned in misery and depression. It was nice to learn about the good that existed, despite all of the overwhelming bad (I'm talking about centuries leading up to the war, not just the war itself). I spent a lot of time learning about the history of where my grandparents grew up: my grandfather from what was then Poland (now Belarus) and my grandmother from Bessarabia (now Moldova). Learning about the history of the Ashkenazim and the centuries of culture we developed brought some really amazing things to light for me. In this process I came across Klezmer music. Bessarabia was a hotbed of development for the genre, and there's even some tunes named after the place (and places within that place).

Through a bizarre series of events, I became an amateur Klezmer musician. This period of intense family research overlapped with the spastic acquisition of an upright bass. I saw a cheap used double bass for a really good deal at a local music store and pounced on it. I had only played double bass a little in high school and was not particularly good at it, but something (*let's face it, I was high all the time*) compelled me to impulsively purchase the bass. I then just started playing bass whenever I was free. The apartment I was living in at the time, unbeknownst to me, was right behind a Jewish community center that was operating out of an old car showroom with no signage. I had lived in that apartment for 5 years and walked past it every day and had no clue what it was — it wasn't particularly open to the public then. One day the double doors in the front were open for some type of event they were hosting, and there was live Klezmer music being played. I was so confused about what was going on and just wandered into this open party wondering how the hell this place was here without me knowing (I literally lived **BEHIND** IT!). I went up to Eric, the guy who seemed to be in charge, and sheepishly asked him, "Can you teach me how to play Klezmer music? I've been trying to find resources online and have no idea what the hell I'm doing." He looked at me and really casually just said, "Yeah sure, just bring your instrument here or whatever." So I did! Fast forward a few years later, I'm playing Klezmer gigs all the time and set up a monthly residency at a local bar. I still suck at the bass, and I still have no idea what I'm doing. What's different now is that I have a community (many of which are alte kockers) to shpiel and kvetch with.

So how does **Kleztizer** play into all of this?

Through playing Klezmer I realized that there's some potential overlap in spheres of influence in my life. There are a lot of issues I have with making electronic music in a modular environment. Modular music is nonlinear by nature. You can try as hard as you want to lock it down to a grid and get it to do exactly what you want at a specific time, but that's fighting what the thing inherently is. It's voltage, nothing more, nothing less. By it's very nature it isn't music. You are constantly fighting an uphill battle trying to make something usable with it, and when you finally do, that moment can be fleeting and instantly fall apart. It's very difficult to craft voltage into sections, phrases, or songs. People try to make it do those things, but to my ear, it never quite works. There's something about it that's just inherently dorky and wrong - something awkward, unexpected, and unruly. If you want make songs easily, use MIDI or use a DAW (better yet, throw all of this shit out and play a real instrument). Modular is something else. **MODAL MUSIC**, by its very nature, has a similar kind of nonlinearity to it. Playing modal music is very freeform — sure, you can follow "rules" and let your ear guide you to what's "correct," but if you start going down this rabbit hole you'll be flush with examples of people who completely shatter those rules using modal theory. There's so much doing "wrong" that winds up being "right."

Understanding modal music and music theory is a very dense subject, especially for the uninitiated. My goal when making this module was to eschew the need to understand anything about music theory and get whoever is using this module to generate *something* musical, even if the result could be perceived as "incorrect" by the trained ear. When using **Kleztizer**, I want you to feel like an ignorant child pushing buttons and turning knobs until you like what you hear, even if you don't necessarily understand what the hell is going on. If you do in fact know what's going on and you do understand music theory, then this device hopefully is a very useful palette for exploring these concepts. I admit, this module creates more problems than it solves, but I'm pretty sure I'm the only moron in existence to try and translate Klezmer music theory to a non-linear electronic musical instrument. If you're reading this and you did in fact beat me to the chase, please send me an email. I would really like to have a beer with you. "

**-Ross Fish January 2026**


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

Going in depth on the music theory of Klezmer music is best left to musicologists. This subject is extraordinarily dense, as the musical tradition is over 500 years old. Liturgical Jewish music is even older (since the year 70 CE in terms of own western aknowledgment and accounting). If I were to begin explaining this to you, I would probably do a terrible job. That being said, here are a few excellent resources on the subject:

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
