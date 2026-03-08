<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TehomScreenshot.png" width="750">

---

# Tehom VCV Rack User Manual

*"Tehom was made in collaboration with my new friend Omri Cohen. I've been a fan of Omri's work for a long time — he's one of the most thoughtful and musical people working in the modular space, and building something together felt like a natural fit. We both wanted to make a sound-on-sound looper, but not the kind where you press record and get a clean, perfectly looped phrase back. We wanted something dirtier and weirder than that. The philosophy behind Tehom is degradation, machine noise, and the beauty that comes from things falling apart in interesting ways. Rather than being a normal looper or sampler, this is about experimenting with the relationships between odd-sized loops, the strangeness that emerges when four of them are running at different speeds and sizes, and the texture that machine noise adds when you layer it underneath. If you're looking for a looper that plays back exactly what you recorded without mangling it, this isn't it. Tehom is an instrument for exploration and experimentation."*

**— Ross Fish, 2026**

---

# Overview

**Tehom** is a four-channel sound-on-sound looper with a built-in XY mixer, machine noise engine, warble, and a suite of global loop window controls. Each of the four channels records and plays back audio independently, with its own playback speed, source blend, and playback direction. The four channels feed into a bilinear XY mixer that pans and balances between them spatially. A library of eight machine noise loops can be mixed into the final output, routed through an external effects chain via a send-return path, and blended in at any level. Nothing in Tehom is designed to stay pristine. The more you push it, the more it degrades.

---

# CV Ranges

### All CV inputs respond to -5 V…+5 V. Every knob is treated as an offset: with the knob at center, a -5 V signal will push it all the way left, and a +5 V signal will push it all the way right. All logic inputs (Record CV, Play CV) trigger on a rising-edge signal and ignore pulse width.

---

# Audio I/O

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TehomAudioIOScreenshot.png" width="400">

* **AUDIO L IN / AUDIO R IN:** Stereo audio inputs. If only the left input is patched, the signal is summed to mono and used for both channels internally.

* **AUDIO L OUT / AUDIO R OUT:** Stereo audio outputs. The output is the sum of the four XY-mixed loop channels plus the machine noise mix.

---

# Individual Channels

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TehomChannelScreenshot.png" width="300">

There are four identical channels arranged in the corners of the module. Each channel records and plays back audio into its own independent buffer, with full control over speed, source blend, and playback direction. The channels are labeled 1 (top-left), 2 (top-right), 3 (bottom-left), and 4 (bottom-right).

* **SOURCE:** A blend knob between the audio input and the loop buffer. Turned fully left, the channel hears and records only the live audio input. Turned fully right, the channel hears and records only its own loop — this is sound-on-sound mode, where each record pass layers on top of the previous one. The middle position mixes both. A CV input is provided for bipolar modulation.

* **SPEED:** Controls the playback speed of the loop buffer. At center (12 o'clock), playback runs at 1× speed. Turned fully left, playback slows to a near-stop. Turned fully right, playback runs at 2× speed. A CV input is provided for bipolar modulation. The LED Bezel spinning indicator tracks this value visually.

* **RECORD (red button):** Toggles recording on and off. A **left-click** starts and stops recording. When recording is started on a buffer that has no content, it begins recording from the top. When recording is stopped manually, the channel will auto-play back what was recorded if **Auto-Play when recording complete** is enabled in the context menu. A **right-click** erases the buffer entirely and flashes white to confirm. A CV input is provided — recording is toggled on each rising edge.

* **PLAY (green/blue button):** Toggles playback on and off. A **left-click** starts and stops playback. A **right-click** toggles the playback direction between forward (green light) and reverse (blue light). A CV input is provided — behavior depends on the **Play CV Mode** setting in the context menu (see Context Menu section).

* **LOOP switch:** A two-position toggle that enables or disables looping. When **On**, the playhead wraps around within the loop window continuously. When **Off**, the channel plays through the loop window once and stops.

* **LED Bezel (spinning indicator):** A visual indicator that rotates in proportion to the current playback speed of the channel. It spins forward during forward playback and in reverse during reverse playback. Clicking and holding the bezel **pauses the playhead** for that channel — useful for stuttering effects.

---

# XY Pad

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TehomXYPadScreenshot.png" width="250">

The XY pad blends the four loop channels together — each channel is assigned to a corner of the pad, and the position of the cursor determines the relative contribution of each channel to the stereo output.

* **Channel 1** occupies the top-left corner.
* **Channel 2** occupies the top-right corner.
* **Channel 3** occupies the bottom-left corner.
* **Channel 4** occupies the bottom-right corner.

Moving the cursor toward any corner brings that channel up to full volume while the others fade out. The XY mixing is bilinear, meaning the four volumes always sum to 1.0 — unity gain is preserved regardless of cursor position.

---

# Global

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TehomGlobalScreenshot.png" width="500">

This section sits at the bottom center of the module. The XY CV and Slew controls govern the XY mixer's inputs, outputs, and response time. The Size, Position, and Xfade controls shape the loop playback window and apply simultaneously to all four channels.

## XY CV I/O

* **X CV IN / Y CV IN:** Bipolar CV inputs that offset the XY position. When the cursor is being dragged, CV is bypassed for that axis to prevent jumping.

* **X CV OUT / Y CV OUT:** Outputs the current slewed XY position as a bipolar voltage (−5 V to +5 V). These can be used to modulate other modules in sync with the XY movement.

## XY Slew

* **SLEW:** Controls the response time of the XY pad. At fully left, the XY position responds almost instantly to changes. As the knob is turned right, the position moves more slowly toward its target, creating smooth, gliding transitions between XY positions. A CV input is provided for bipolar modulation.

## Loop Window

* **SIZE:** Determines what fraction of the recorded buffer is used as the active loop window. At fully right, the entire recorded buffer plays back. As the knob is turned left, the loop window shrinks toward a minimum of 100 samples. Applies to all four channels simultaneously. A CV input is provided for bipolar modulation.

* **POSITION:** Moves the start point of the loop window within the recorded buffer. At fully left, the window starts at the beginning of the recording. At fully right, the window is pushed as far toward the end as the current size allows. Using this knob with a small SIZE will allow you to scrub through the audio in the buffer. Applies to all four channels simultaneously. A CV input is provided for bipolar modulation.

* **XFADE:** Applies a linear fade in and fade out at both edges of the loop window to prevent clicks and pops at the loop point. At zero, no fading is applied and the loop repeats with a hard edge. As the knob is turned right, the fade zones grow until at maximum they each span half the loop — creating a triangle-shaped amplitude envelope across the entire loop. Applies to all four channels simultaneously. A CV input is provided for bipolar modulation.

---

# Degrade

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TehomDegradeScreenshot.png" width="500">

This section controls Tehom's degradation and noise features — warble for destabilizing loop playback pitch, a machine noise player, and a send-return path for processing the noise externally.

## Warble

* **WARBLE:** Applies a combined wow and flutter effect to all four playback channels. Wow is a slow, deep pitch oscillation (0.5–2 Hz); flutter is a faster, subtle pitch tremor (5–15 Hz). Both are modulated by the same knob in a non-linear curve — the effect fades in gradually from zero so the bottom portion of the knob is silence, and the upper portion becomes increasingly unstable. A CV input is provided for bipolar modulation.

## Noise

* **MEDIA:** Selects one of eight machine noise loops to blend into the final output. The options in order are: Mic Preamp, Reel To Reel, Cassette, VHS, Vinyl Clean, Vinyl Dirty, 8mm Film, 16mm Film. Each is a looping audio file with its own character and spectral content. A CV input is provided for bipolar modulation.

* **AMOUNT:** Controls the volume of the selected machine noise loop in the final output mix. At fully left, no noise is present. At fully right, the noise loop plays at full level. A CV input is provided for bipolar modulation.

## Noise Send-Return

* **SEND:** A mono output that carries the currently selected machine noise loop signal. The send can be **Pre-Fader** (the default) or **Post-Fader**, selectable from the context menu. Pre-Fader sends the raw noise loop signal before the Amount knob is applied. Post-Fader sends the noise loop after the Amount knob is applied.

* **RETURN:** A mono input. When a cable is patched into this input, it replaces the noise loop signal for both the left and right channels before the Amount knob is applied. This means the return signal's level in the final mix is still controlled by the Amount knob, regardless of pre- or post-fader setting.

---

# Context Menu

<img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/TehomContextMenuScreenshot.png" width="200">

Right-clicking on the module panel opens the context menu. All settings save with the patch and respond to **Init**.

## GUI

* **XY Pad**
  * **Draw Crosshairs:** Overlays horizontal and vertical lines on the XY pad that cross at the current cursor position. Useful for precise placement.
  * **Persist:** When enabled, the XY pad draws a comet trail that follows the cursor and fades over time. When disabled, the pad shows only the cursor dot.

* **Background Scroll**
  * **Off / Slow / Medium / Fast:** Controls the speed of the scrolling background texture behind the module panel.
  * **Direction:** Sets the scroll direction to **Right** or **Left**.

## Global

* **Buffer Size:** Sets the maximum recording duration for all four channels. Options are 1, 2, 5, 10, 20, 30 seconds, and 1 minute. Changing this setting resizes all buffers and clears any recorded content. Default is 2 seconds.

* **Noise Aux Send**
  * **Pre-Fader** *(default)*: The Send output carries the raw noise loop signal before the Amount knob. Changing the Amount knob does not affect the send level.
  * **Post-Fader:** The Send output carries the noise loop after the Amount knob. The send level tracks the Amount setting.

## Channels

Each of the four channels has its own submenu with the following settings:

* **Play CV Mode:** Determines how a rising-edge signal at the Play CV input behaves.
  * **Play/Stop** *(default)*: Each trigger toggles between playing and stopped, resetting the playhead to the start when play begins.
  * **Retrigger:** Each trigger resets the playhead to the start of the loop without toggling stop. Playback continues uninterrupted after the trigger, restarting from the beginning each time.
  * **Forward/Reverse:** Each trigger flips the playback direction between forward and reverse.

* **Auto-Play when recording complete:** When enabled, the channel automatically begins playing back as soon as recording is manually stopped. Default is on.

* **Auto-Play when buffer full:** When enabled, the channel automatically begins playing back when the recording buffer fills to capacity. Default is on.

---
