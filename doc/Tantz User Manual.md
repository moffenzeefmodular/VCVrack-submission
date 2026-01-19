[![Video Tutorial](https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TantzYoutubeThumb.jpg)](https://youtu.be/oB4lEuMjE34)

---

# Tantz! VCV Rack User Manual 

"**Tantz!** is the second module in my Klezmer series and is a companion module for **Kleztizer.** For an essay on my relationship with Klezmer music [check out the Kleztizer user manual here](https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/Kleztizer%20User%20Manual.md). I worked on this module with my friend and fellow Klezmer musician David Rosman who helped me draft the sequences inside this module. We took some serious liberties coming up with all of these patterns, as there really isn't much of a reference to use other than our own ears. Unlike heavily researched percussion heavy genres like afro-cuban music, we don't have a huge database of transcriptions to work from. Using this module very rarely produces accurate Klezmer patterns if under heavy modulation. The patterns were designed like presets on an organ drum machine where you're supposed to "set it and forget it." That being said, I don't actually want you to use it in this manner at all, that's why there's CV for everything, Make weird IDM with it that has nothing to do with Klezmer music. 

  **-Ross Fish January 2026**


# History of Percussion in Klezmer Music

This is my stab at a brief history of percussion and its relationship to Klezmer music. Please take it with a massive wooden spoonful of salt, as I am not an ethnomusicologist; this is just information I picked up through my own research and anecdotal points I've gathered from the living elder Klezmorim in my life.

For the majority of its evolution, percussion was not actually a core component of the music until much later in the development of the genre. In early Klezmer music, strings and wind instruments dominated the small ensembles. The interplay between all of the individual parts is where the dominant rhythmic structure is inherited, and percussion is a sort of afterthought to reinforce these patterns as time went on. An instrument known as the cimbalom (a type of hammered dulcimer) started making its way into the music by the 17th century and became the driving rhythmic instrument of the genre. Around the 19th century, as Jewish musicians started joining military bands, they adopted brass instruments and the percussion used for these marching ensembles.

The pogroms in Russia in the early 1900s caused many Klezmorim to flee Europe and come to America. In the early 1910s, Klezmer began its second life in America. The earliest acoustic recordings of Klezmer music still don't seem to have much (or any at all) percussion on them, and this could be a combination of factors. My assumption is that the technology of the era made percussion more of a problem than an asset, overpowering the other instruments in volume and making the melody less intelligible.

The Yiddish theater was a dominant force in American Jewish music. Jazz, opera, vaudeville, and other contemporary music of the era started influencing and morphing the genre. Popluar Jewish singers like The Barry Sisters made their recording debut in the 1930s. The old-world ways of traditional Klezmer music were beginning to fade and morph into something entirely new.

After the Holocaust, the genre almost disappeared completely. Many Klezmorim died in Europe. Unfortunately, the original traditional players who were already in America were not particularly interested in keeping the genre alive, and there wasn't much of a market left for it, at least in its raw, unadulterated folk form.

The type of drumming people associate with modern Klezmer music started developing in earnest in the 1950s, as post-war survivors came over and started assimilating into American culture. Advances in recording technology allowed percussion to work alongside the rest of the ensemble without drowning out the sound of the melody players. "Big band" types of ensembles that resembled the popular jazz and swing music of the day started doing modern interpretations of traditional Yiddish folk standards and Yiddish theater repertoire. All of the schmaltz and cheese of the 1950s and 60s records became most people's relationship to the genre—a weird and distant cousin of what was once a very rich folk tradition. It's hard to even call Jewish music of this era Klezmer, as it's more closely related to jazz than anything else.

The 1970s Klezmer revival, spearheaded by Lev Liberman on the West Coast, sought to reconnect with the roots of the genre. Lev spent a lot of time digging up old 78s and meeting some of the old players in person, doing his best musical detective work to put the pieces back together as best as he could. What we live with now is our best interpretation of what we thought the genre was, putting together bits and pieces spread out across huge timelines. There are ethnomusicologists who have dedicated their entire lives to this pursuit, and who probably think most of this blurb is completely incorrect. They are probably correct in that assumption. I'm doing the best I can.

--- 

# Klezmer Rhythm References

As mentioned earlier, drumming proper is not necessarily the dominant rhythmic force of these rhythms. Here are some recorded examples of some of the grooves you will hear in Klezmer music. 

* **Bulgar** 
* **Araber**
* **Terkisher**
* **Hora**
* **Zhok**
* **In Zibn** 

---
# Overview

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TantzScreenshot.png" width="500">

**Tantz!** is a preset drum sequencer inspired by organ drum machines. There are 6 tracks that have 8 pattern variations per track. Changing the **Style** shifts the global bank for all tracks to a completely new set of patterns and time signature. Global swing is provided mainly for the patterns that are in 3/4 to mimick the sound of the cimbalom. 

Although the knobs are labeled Kick, Snare, Hat etc., these are just modern words I used as anchors so you could wrap your head around the function of each track. The patterns associated with each channel don't necessarily need to be triggered with those drums, in fact, they would be more effective triggering different musical notes rather than percussion at all. 

---

# CV Ranges 

### All CV inputs respond to -5 V…+5 V. Every knob is treated as an offset: with the knob at 50%, a -5 V signal will turn it all the way left, and a +5 V signal will turn it all the way right. All logic inputs (labeled CLK IN) ignore pulse width and trigger on a rising-edge signal. All logic outputs are 0-5v. 

---

# Global

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TantzGlobal.png" width="250">

This section controls the functionality of the entire **Tantz!** module.

* **RUN:** A global Start/Stop for **Tantz!** with an associated bipolar CV input. Sequence will not run unless an external clock is present at **CLK. IN**
* **STYLE:** Select the global rhythmic style for **Tantz!** (in order): Bulgar #1, Bulgar #2, Araber, Terkisher, Hora/Zhok, In Zibn. Think of this knob as a global "bank" for the patterns on each channel.
* **SWING:** Global swing amount with an associated bipolar CV input. 
* **PW:** Global pulse width for all gate outputs.

---

# Individual Tracks 

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TantzTracks.png" width="400">

The individual tracks of Tantz! are responsible for selecting eight sub-variations of patterns within each bank of rhythms. The banks of rhytmic patterns are controlled by the **Style** parameter. 

* **KNOB:** Pattern select 1-8 for one of the preset rhythms as determined by **Style.**
* **SWITCH:** Mute on/off for the associated channel.

---

# I/O 

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TantzIO.png" width="600">

This section is responsible for clocking I/O, voltage control, and gate outputs. 

* **RESET:** A gate **output** that fires one step before step 1 of the sequence. This reset is meant to go to other sequencers down the line to keep them in sync with **Tantz!.**
* **CLK IN:** A rising edge trigger input that is required for Tantz! to run. 
* **CV INPUTS:** Bipolar CV inputs for pattern selection per channel.
* **GATE OUTPUTS:** Associated gate outputs for each channel. 

---