<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/ParticlesScreenshot.png" height="638"></p>

---

*"In a war torn world ravaged by hatred, environmental meltdown on the horizon, and democracy as we know it crumbling in front of our very eyes, I have made an oath: I will continue to make the most useless possible software imaginable." -Ross Fish 2026*

---

# Particles VCV Rack User Manual

---

# Overview

**Particles** is a full-screen visual effects module. When switched on it fills the entire VCV Rack window with falling animated particles — snowflakes, raindrops, embers, meteors, cats and dogs, fast food, and more. Sixteen particle types are available. Gravity, direction, size, and particle density are all CV-controllable. 

Particles produces no audio. It is purely visual.

> **Performance note:** Running Particles with a high engine thread count (Engine → Threads) may cause audio glitches in your patch. For best performance, set Threads to **1** in Engine settings when using this module.

---

# Controls

### TYPE Knob
Selects the particle shape. The knob sweeps through all 16 types in the following order:

| Position | Type |
|---|---|
| 1 | Snowflake |
| 2 | Raindrop |
| 3 | Acid Rain |
| 4 | Leaf |
| 5 | Petal |
| 6 | Ash |
| 7 | Ember |
| 8 | Star |
| 9 | Meteor |
| 10 | Bubble |
| 11 | Confetti |
| 12 | Polka Dots |
| 13 | Heart |
| 14 | Men |
| 15 | Cats and Dogs |
| 16 | Fast Food |

Raindrop, Acid Rain, and Meteor are direction-locked — they always face the direction they are travelling. All other types rotate freely as they fall.

### SIZE Knob
Particle size. Particles vary slightly around this base value. Default is 25%.

### GRAVITY Knob
Sets the downward fall speed of the particles. At minimum the particles barely drift; at maximum they streak rapidly down the screen. Default is 15%.

### DIRECTION Knob
Sets the horizontal drift direction and speed. At 12 o'clock particles fall straight down. Turn left to drift left, right to drift right. Default is 60%.

### DENSITY Knob
Controls the number of particles on screen at once, from a sparse handful at minimum to a lot at maximum. Default is 50%.

### ON/OFF Button (LED)
Latching push button that toggles the animation on and off. The ON/OFF button is excluded from the Randomize function.

---

# I/O

### Inputs

* **TYPE CV:** ±5V. Summed with the Type knob to offset particle type selection.

* **SIZE CV:** ±5V. Summed with the Size knob to offset particle size.

* **GRAVITY CV:** ±5V. Summed with the Gravity knob to offset fall speed.

* **DIRECTION CV:** ±5V. Summed with the Direction knob to offset horizontal drift.

* **DENSITY CV:** ±5V. Summed with the Density knob to offset particle count.

* **ON/OFF CV:** Gate input, threshold 1V (rising edge). Each rising edge toggles the effect on or off, the same as pressing the ON/OFF button.

---

# Context Menu

Right-click the module panel to access additional options.

### Particle Opacity
A slider from 10% to 100% that sets the transparency of the particle layer. Useful for blending the effect subtly into a performance or presentation setup. The mouse tracker ring is always drawn at full opacity regardless of this setting. Default is 100%.

### Mouse Tracker
Toggles the green crosshair ring that follows the mouse cursor. When enabled, a bright green circle and center dot are drawn over the particle layer at the current mouse position, making the cursor easy to find during performances. On by default.
