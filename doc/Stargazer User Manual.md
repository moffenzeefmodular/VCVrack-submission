[![Video Tutorial](https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/StargazerYoutubeThumb.jpg)](https://youtu.be/sNoFB7NnAwc)

---

# Stargazer VCV Rack User Manual 

### "This VCV Rack module is a hardware clone of a previously released drone synthesizer. The original project from 2016 was designed as a portable, all-in-one, compact solution for performing a drone set without the need for a specialized power supply or interface. Concepts that I was playing with in Eurorack at the time were packed down into this little 9v powered box with a 1/4" output and used at shows that were (and are) generally poorly attended. Over the lifespan of the product I made five limited edition colorways, all signed by the builders, with only 425 total units produced. Each through-hole unit was soldered by hand in Portland, OR by myself and a rotating cast of clowns. Many of those clowns I still have the luxury of calling my 'friends.' While the VCV module adds a robust new feature set, I made a point to keep the core functionality as close to the original as possible, even utilizing the exact same wavetable files from the hardware unit. The added features are just a cherry on top."
**-Ross Fish January 2026** 

---

# Overview

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/StargazerScreenshot.png" width="1000">

Stargazer is a stereo dual wavetable based drone synth with two multimode filters, sample rate and bit depth reduction, and three LFOs. While a wide variety of sounds are possible with this device, it was designed with the intention of creating slow morphing timbral shifts over time. For this reason, the device is monophonic and has a restricted pitch range dedicated to low frequencies. This is also why the device does not have onboard amplitude or filter envelopes.

The visual design that mimics a constellation may look haphazard at first, but there is actually a key to help you better understand the underlying signal flow. **SOLID LINES** indicate that parameters are part of the same section or module. **DASHED LINES** indicate a CV routing from source to destination. **DOTTED LINES** indicate an audio routing from source to destination. See Normalization Schema and Block Diagram below for more information.

---

# CV And Audio Ranges

All CV inputs respond to -5v...+5v. Every knob is treated as an offset - with the knob at 50%, a -5v signal will turn it all the way left, and a +5v signal will turn it all the way right. Audio outputs are clamped at -5v and +5v. 

**The LFOs each have their own dedicated outputs. It is recommended to route the LFOs back into Stargazer to explore more complex modulation, especially when intertwining the LFOs together for chaotic results.**

---

# Detailed Description 

## Oscillators

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/StargazerOscillator.png" width="300">

### The oscillator section contains a primary and secondary oscillator that share the same wavetable selection, 1v/Octave pitch input, and FM CV input. The wavetables smoothly interpolate between one another creating a morphing and organic sound. The secondary oscillator can be detuned and mixed in with the primary oscillator to create frequency beating and chorus-like effects. The pitch tracking is restricted to the overall oscillator range of 1hz - 500hz. If you would like the oscillators tuned to a specific chromatic note, you can click on the pitch knob and type in the MIDI note of your preference (example C0, or Eb1). See Normalization Schema and Block Diagram below for more information.

* **Pitch:** Control the pitch of both oscillators from 1hz - 500hz (C-2 to B4 in MIDI). 
* **FM:** Linear FM control over the pitch of both oscillators (restricted to a +/-1v range to get a more vibrato-like effect).
* **Wave:** Smoothly interpolated wavetable selection. There are 88 total wavetables.
* **Mix:** Oscillator 2 volume.
* **Sub:** Transpose Oscillator 2 down one octave (pre-detune). 
* **Detune:** +/-5hz additional control over the pitch of Oscillator 2. Both oscillators share the same pitch, 1v/Octave input, and FM information.

---

## Filters 

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/StargazerFilter.jpg" width="300">

### There are two identical filter sections provided for Stargazer. The difference between the two sections is their location in the signal chain, and their modulation routing. Both filters are multimode and have the ability to be bypassed from the signal chain entirely. See Normalization Schema and Block Diagram below for more information. 

* **Freq:** Cutoff frequency. 80hz - 16khz
* **Mode:** Filter shape. Shapes in the following order: Lowpass, Bandpass, Highpass, Notch, Off (bypass).
* **Res:** Filter resonance.

---

## Sample Rate + Bit Depth Reduction 

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/StargazerBitReduction.png" width="250">

### Control over sample rate and bit depth are provided between both filters. The rationale for the signal routing is that the user would use the first filter to modify the wavetables and then add harmonically rich content back into the signal. The second filter would then subtract this content back out of the signal. This is a weird kind of way of thinking about subtractive synthesis, but yields some aesthetically pleasing results. See Block Diagram below for more information.

* **Alias:** Continuously variable sample rate. 18khz - 20.5hz.
* **Redux:** Stepped control of bit depth. 16 bit - 4 bit. 

---

## Modulation 

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/StargazerLFO.png" width="300">

### There are three identical LFOs inside of Stargazer. The difference between each LFO is their place in the signal chain. See Normalization Schema and Block Diagram below for more information.

* **Wave:** LFO waveshape selection. In order: Sine, Triangle, Ramp Up, Ramp Down, Square, Stepped Random. 
* **Range:** The overall LFO frequency range. 
**Slow:** 0.01hz - 0.1hz, **Medium:** 0.05hz - 10hz, **Fast:** 0.05hz - 50hz.
* **Rate:** LFO frequency. Frequency range is dependent upon the **Range** switch setting.
* **Depth:** LFO amplitude.

---

## Output Stage 

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/StargazerOutput.png" width="300">

### The output stage consists of a VCA, a mono to stereo spatialization section, as well as a distortion. LFO 2 is normalled to Stargazer's VCA volume. Following the VCA, a Spread knob is provided, which controls the panning relationship between Stargazer, and a 20ms delayed copy of the signal. After the spatialization section, a simple gain stage and output volume attenuator are provided. See Normalization Schema and Block Diagram below for more information.

* **Spread:** The panning relationship between Stargazer and its delayed copy. At 0%, the delay is fully bypassed and the signal is mono. As the user gradually turns up this parameter, the delayed signal and dry signal gradually pan apart from one another until they are fully panned left and right at 100%.
* **Gain:** Variable gain from 1x to 100x. The signal is clipped at 10vpp, so this in effect becomes a drive parameter for distortion. 
* **Vol:** The output volume, post distortion. 

---

# Normalization Schema 

Several parts of the circuit are "normalized" together. This means that with no patch cable present the connection is hardwired. When you patch a signal into the associated jack it will override this pre-existing normalization. The normalized signals are as follows: 

* **LFO 1:** by default this LFO is mapped to the main oscillator's **WAVE** knob which selects the primary wavetable for the synth. Patching a signal into the **WAVE** CV input will override this connection.
* **LFO 2:** by default this LFO controls the volume Stargazer, before the distortion circuit. Patching a signal into the **VOL.** CV input will override this connection.
* **LFO 3:** by default this LFO controls the cutoff frequency of Filter 2. Patching a signal into the **FREQ 2** CV input will override this connection. 
* **FM:** by default this attenuator is controlling the amount **LFO 3** modulates the frequency of **Pitch**. This allows the user to easily get access to vibrato without needing to patch an external signal. Patching a signal into the **FM** CV input will override this connection. 

---

# Block Diagram 

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/StargazerBlockDiagram.jpg">
