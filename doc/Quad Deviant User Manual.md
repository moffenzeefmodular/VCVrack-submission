[![Video Tutorial](https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/QuadDeviantYoutubeThumb.jpg)](https://youtu.be/G7r7HutJrxc)

# Quad Deviant VCV Rack User Manual 

### "You'll notice on the front panel of this module it says MORDAX x MOFFENZEEF. This title is a tribute to my friend Brandon Fessler, who owns Mordax. He had a lot of input on the design of this module. Brandon was at my house for Thanksgiving, and we were talking about Deviant on my couch (he's a fan). He told me I needed to put more of them together into one module and make a mega Moffenzeef sequencer, so this implementation was the result of that conversation. Revisiting Deviant was an opportunity for me to make it even more streamlined by removing the Smooth knob (which originally was an afterthought to cure an ailment created by poor engineering — you'll have to ask me about that in person). As Deviant was originally a hardware device and part of System Moffenzeef, it never existed in a void. Deviant was always meant to be paired with MITO as the core of System Moffenzeef's sequencer. The reason Deviant is a two-channel device is that I want one channel to control all of the divisions on your MITO (your rhythmic sequences), and the other channel to control all of the timbre-related parameters in your patch. This method means you only have two modulation signals to focus on for the whole system. The Deviant in this application becomes a macro controller for the entire groove. Now, with a quad Deviant, this philosophy can extend even further. Perhaps the third Deviant is specifically for volume, and the fourth is meant for FX modulation. The CV Bus was Brandon's suggestion, and I kind of went nuts with it, adding any combination of voltages I could think of. I'm not sure what you would use any of the outputs for, but now you have a bunch of interrelated, rhythmically random outputs to work with. As this design has extended beyond my original goals and philosophy, I don't have instructions as to what to do with this section of the circuit, as I'm still trying to figure out its use cases. This is where my work ends and yours begins — let me know what you discover!" — Ross Fish, January 2026
---

# Overview

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/QuadDeviantScreenshot.png" width="750">

**Quad Deviant** is a four-channel stepped random voltage generator with Max/Min control per channel. Each channel’s random voltage can be dialed into a window defined by the **Top** and **Bottom** parameters, while the **Range** switch determines the overall spread of this window. CV inputs are provided for both aspects of the window: **Top** and **Bottom**.

The four outputs are combined into an assignable **CV Bus** at the far right of the module. Individual channels can be toggled on or off to determine whether they contribute to this bus.

**Deviant** condenses the function of several utility modules into a single, easy-to-use interface, so the user doesn’t have to worry about scaling all the different voltages manually. A myriad of combinations awaits you, offering unpredictable yet controllable randomness.

---

# CV And Audio Ranges

### All CV inputs respond to -5v...+5v. Every knob is treated as an offset - with the knob at 50%, a -5v signal will turn it all the way left, and a +5v signal will turn it all the way right. 

--- 

# Individual Channels 

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/QuadDeviantChannel.png" width="100">

---

# CV Bus 

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/QuadDeviantCVBus.png" width="100">

--- 

# Normalization Schema 

Several parts of the circuit are "normalized" together. This means that with no patch cable present the connection is hardwired. When you patch a signal into the associated jack it will override this pre-existing normalization. The normalized signals are as follows: 

* The **CLK IN** of each channel recieves it's trigger from the **CLK IN** of the previous channel and then continues it's normalization down to the next channel. Example 1: If a trigger is plugged into **CLK IN** on channel 1, all 4 Deviants will trigger. If you plug a new trigger into channel 2, it will trigger channels 2, 3, and 4. Example 2: If a trigger is plugged into **CLK IN** on channel 1 and a seperate trigger is pluugged into channel **CLK IN** on channel 3, channel 1 and 2 will share the same trigger and channels 3 and 4 will share the same trigger. 

