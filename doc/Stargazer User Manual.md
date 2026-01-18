# Stargazer VCV Rack User Manual 

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/StargazerScreenshot.png" width="1000">

[![Video Tutorial](https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/StargazerYoutubeThumb.jpg)](https://youtu.be/sNoFB7NnAwc)

### "This VCV rack module is a hardware clone of a previously released drone synthesizer. The original project from 2016 was designed as portable, an all in one, compact solution for performing a drone set without the need for a specialized power supply or interface. Concepts that I was playing with in Eurorack at the time were packed down into this little 9v powered box with a 1/4" output and used at shows that were (and are) generally poorly attended. Over the lifespan of the product I made five limited edition color ways, all signed by the builders, with only 425 total units produced. Each through-hole unit was soldered by hand in Portland, OR by myself and a rotating cast of clowns. Many of those clowns I still have the luxury of calling my 'friends.' While the VCV module adds a robust new feature set, I made a point to keep the core functionality as close to the original as possible, even utilizing the exact same Wavetable files from the hardware unit. The added features are just a cherry on top."
**-Ross Fish January 2026** 

# Overview

Stargazer is a stereo dual wavetable based drone synth with two multimode filters, sample rate and bit depth reduction, and three LFOs. While a wide variety of sounds are possible with this device, it was designed with the intention of creating slow morphing timbral shifts over time. For this reason, the device is monophonic and has a restricted pitch range dedicated to low frequencies. This is also why the device does not have onboard amplitude envelopes.

The visual design that mimicks a constellation may look haphazard at first, but there is actually a key to help you better understand the underlying signal flow. **SOLID LINES** indicate that parameters are part of the same section or module. **DASHED LINES** indicate a CV routing from source to destination. **DOTTED LINES** indicate an audio routing from source to destination. 

# CV And Audio Ranges

All CV inputs respond to -5v...+5v. Every knob is treated as an offset - with the knob at 50%, a -5v signal will turn it all the way left, and a +5v signal will turn it all the way right. Audio outputs are clamped at -5v and +5v. 

**The LFOs each have their own dedicated outputs. It is recommended to route the LFOs back into Stargazer to explore more complex modulation, especially when intertwining the LFOs together for chaotic results.**

# Normalization Schema 

Several parts of the circuit are "normalized" together. This means that with no patch cable present the connection is hardwired. When you patch a signal into the associated jack it will override this pre-existing normalization. The normalized signals are as follows: 

* **LFO 1:** by default this LFO is mapped to the main oscillator's **WAVE** knob which selects the primary wavetable for the synth. Patching a signal into the **WAVE** CV input will override this connection.
* **LFO 2:** by default this LFO controls the volume stargazer, before the distortion circuit. Patching a signal into the **VOL.** CV input will override this connection.
* **LFO 3:** by default this LFO controls the cutoff frequency of Filter 2. Patching a signal into the **FREQ 2** CV input will override this connection. 
* **FM:** by default this attenuator is controlling the amount **LFO 3** modulates the frequency of **Pitch**. This allows the user to easily get access to vibrato without needing to patch an external signal. Patching a signal into the **FM** CV input will override this connection. 


# Detailed Description 

## Oscillators

## Filters 

## Sample Rate + Bit Depth Reduction 

## Modulation 

## Output Stage 