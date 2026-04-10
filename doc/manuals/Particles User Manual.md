<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/ParticlesScreenshot.png" height="638"></p>

---

# Particles VCV Rack User Manual

---

# Overview

**Particles** is a full-screen visual effects module. When switched on it fills the entire VCV Rack window with falling animated particles — snowflakes, raindrops, embers, meteors, cats and dogs, fast food, and more. Sixteen particle types are available, each with hand-drawn shapes rendered live using NanoVG. Gravity, direction, size, and particle density are all CV-controllable. A built-in mouse tracker draws a bright green crosshair ring around the cursor so it stays visible through the effect.

Particles produces no audio. It is purely visual.

> **Performance note:** Running Particles with a high engine thread count (Engine → Threads) may cause audio glitches in your patch. For best performance, set Threads to **1** in Engine settings when using this module.

---

# CV Ranges

* **TYPE CV:** 0–10V. Summed with the Type knob to select the particle type. Full 0–10V range covers all 16 types.
* **SIZE CV:** 0–10V. Summed with the Size knob to offset particle size.
* **GRAVITY CV:** 0–10V. Summed with the Gravity knob to offset fall speed.
* **DIRECTION CV:** 0–10V. Summed with the Direction knob to offset horizontal drift.
* **DENSITY CV:** 0–10V. Summed with the Density knob to offset particle count.
* **ON/OFF CV:** Gate input, threshold 1V. Each rising edge toggles the particle effect on or off.

---

# Controls

### TYPE Knob
Selects the particle shape. The knob sweeps through all 16 types in the following order:

| Position | Type |
|---|---|
| 0 | Snowflake |
| 1 | Raindrop |
| 2 | Acid Rain |
| 3 | Leaf |
| 4 | Petal |
| 5 | Ash |
| 6 | Ember |
| 7 | Star |
| 8 | Meteor |
| 9 | Bubble |
| 10 | Confetti |
| 11 | Polka Dots |
| 12 | Heart |
| 13 | Men |
| 14 | Cats and Dogs |
| 15 | Fast Food |

Raindrop, Acid Rain, and Meteor are direction-locked — they always face the direction they are travelling. All other types rotate freely as they fall.

### SIZE Knob
Sets the base radius of each particle. Particles vary slightly around this base value. Default is 25%.

### GRAVITY Knob
Sets the downward fall speed of the particles. At minimum the particles barely drift; at maximum they streak rapidly down the screen. Default is 15%.

### DIRECTION Knob
Sets the horizontal drift direction and speed. At 12 o'clock particles fall straight down. Turn left to drift left, right to drift right. Default is 60%.

### DENSITY Knob
Controls the number of particles on screen at once, from a sparse handful at minimum to twice the default count at maximum. Default is 50%.

### ON/OFF Button (LED)
Latching push button that toggles the particle effect on and off. The LED illuminates when the effect is active. Particles fade in and out smoothly when toggled. The ON/OFF button is excluded from the Randomize function.

---

# I/O

### Inputs

* **TYPE CV:** Voltage-controlled particle type selection. 0–10V range summed with the Type knob.

* **SIZE CV:** Offsets particle size. 0–10V summed with the Size knob.

* **GRAVITY CV:** Offsets downward fall speed. 0–10V summed with the Gravity knob.

* **DIRECTION CV:** Offsets horizontal drift. 0–10V summed with the Direction knob.

* **DENSITY CV:** Offsets particle count. 0–10V summed with the Density knob.

* **ON/OFF CV:** Gate input. Each rising edge (threshold 1V) toggles the effect on or off, the same as pressing the ON/OFF button.

---

# Context Menu

Right-click the module panel to access additional options.

### Particle Opacity
A slider from 10% to 100% that sets the transparency of the particle layer. Useful for blending the effect subtly into a performance or presentation setup. The mouse tracker ring is always drawn at full opacity regardless of this setting. Default is 100%.

### Mouse Tracker
Toggles the green crosshair ring that follows the mouse cursor. When enabled, a bright green circle and center dot are drawn over the particle layer at the current mouse position, making the cursor easy to find during performances. On by default.

---

# Notes

* Particles renders over the entire VCV Rack window, including all other modules. It sits behind the menu bar and browser.
* The effect fades in and out smoothly over approximately one third of a second when toggled.
* All settings — including Particle Opacity and Mouse Tracker — are saved and restored with the patch.
* The ON/OFF button is excluded from Randomize. All other knobs randomize normally.

---
