[![Video Tutorial](https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/DitDitYoutubeThumb.jpg)](https://youtu.be/rMIwqCQthho)

---

<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/DitDitScreenshot.png" width="300"></p>

---

# Dit Dit VCV Rack User Manual

*"I've had a soft spot for Morse code since I was a kid. There's something deeply compelling about reducing all of human language to just two symbols — a short pulse and a long one — and the idea that you could send anything across a wire with only that. Dit Dit is the module I always wanted: a way to clock text into gates so your synthesizer can say something, even if nobody knows what it's saying. You don't need to know a single thing about Morse code to use it. Type something, patch a clock in, and let it rip. It's a silly module with a serious pedigree, and I think that's about right."*

**— Ross Fish, 2026**

---

# What is "Dit Dit"?

**Dit** and **Dah** are the phonetic names radio operators use for the two symbols of Morse code — the short pulse (dit, or dot) and the long pulse (dah, or dash). Saying the sounds out loud while keying is an old technique for internalizing the rhythm: a letter like **R** ( · — · ) becomes *dit-dah-dit*, rolling off the tongue in the same rhythm it travels down the wire. At the end of a transmission, operators would often sign off with two quick dits — *dit dit* — a casual, informal goodbye that became a shorthand among ham radio operators worldwide. It means something like "see you later" in a language of bleeps.

The module is named after that sign-off.

---

# Overview

**Dit Dit** is a clock-driven Morse code text-to-gate sequencer. Type words into it, clock it, and Morse code gate patterns come out of it. Dit Dit's intended use is for creating evolving polyrhythmic patterns. Get creative with your letter and word combinations and see what you discover.

Type any text into the display, patch in a clock, flip the switch to Start, and Dit Dit converts each character into its Morse code sequence, firing gate outputs for each element as the clock advances. The **DIT** output fires on dots, the **DAH** output fires on dashes, and the **MORSE** output fires on both. The tempo of the transmission is entirely determined by the incoming clock — faster clock, faster code. There is no internal oscillator and no audio output. It is purely a gate sequencer that speaks in letters.

> **Note for hardware users:** The VCV Rack version replaces the hardware's PS/2 keyboard input with a double-click text editor built into the display, and replaces the F1–F5 and Ctrl+S/O keyboard shortcuts with context menu settings and save/load slots. All timing behavior, gate logic, and CV I/O are otherwise identical to the hardware module.

---

# CV Ranges

### All logic inputs (BANG!, RESET, START/STOP CV) trigger on a rising-edge signal. The threshold is 1 V. All logic outputs are 0 V / 5 V gates or triggers depending on the Morse Output mode setting.

---

# Display

The display at the top of the module is an OLED-style text area with two modes: **Text Input Mode** (when the module is stopped) and **Animation Mode** (when running).

### Text Input Mode (START/STOP Switch Down)

When the module is stopped, the display shows the text currently loaded in the buffer. If the buffer is empty, it shows a dim placeholder: **DOUBLE-CLICK TO TYPE**.

**Double-clicking** the display opens the text editor. A blinking cursor appears and you can type directly into the buffer. Dit Dit accepts the letters **A–Z**, digits **0–9**, and the punctuation characters **. , ? ! / ' ; : - @ ( ) = + & "** — any character that has a valid ITU Morse code representation. The buffer holds up to **84 characters**. All input is stored and displayed in uppercase.

While editing:
- **Enter:** Closes the editor.
- **Escape:** Closes the editor and clears the buffer entirely.
- **Arrow keys:** Move the cursor left, right, up, and down through the text.
- **Home / End:** Jump to the start or end of the buffer.
- **Backspace / Delete:** Delete the character before or after the cursor.
- **Space:** Inserts a word space (7-tick gap in playback).
- **Tab:** Inserts four spaces.

> **Hardware difference:** The hardware module uses a PS/2 keyboard for text entry and supports additional key commands including **Ctrl+S** (save to slot), **Ctrl+O** (load from slot), **INS** (insert mode), **F6** (save settings), **F7** (factory reset), and an **F12** Easter egg. In the VCV version, save/load slots and settings are all accessed through the right-click context menu. Module initialization (right-click > Initialize) is the equivalent of the hardware F7 factory reset.

### Animation Mode (START/STOP Switch Up)

When the module is running, the display shows the character currently being transmitted — large, centered — with its Morse pattern (dots and underscores) shown below it, building progressively as each element fires. Two status indicators appear in the lower area:

* **END** (in orange): The message has reached the end of the buffer and Loop is disabled.
* **NO CLOCK** (in orange): No clock signal has arrived at BANG! in the last seven seconds.

---

# Controls

### START / STOP Switch

A two-position toggle that puts the module into Animation Mode (up) or Text Input Mode (down). Flipping to Start immediately begins transmitting from the top of the buffer and resets the playback position. Flipping to Stop kills all outputs and returns to Text Input Mode. Dit Dit will only emit gate sequences when the Start switch is up **and** the BANG! input is receiving a clock signal.

---

# I/O

### Inputs

* **BANG!:** The clock input. Each rising edge advances the Morse state machine by one dit-unit. The speed of the transmission tracks the incoming clock in real time — there is no fixed internal timing. Valid clock periods are approximately 5 ms to 5 seconds per tick.

* **RESET:** A rising-edge trigger input that snaps the sequence back to the first character without stopping the module. All current outputs are cut immediately.

* **START/STOP:** A gate input that is logically OR'd with the physical Start/Stop switch. A rising edge starts the module; a falling edge stops it.

### Outputs

* **DIT:** Gate output for every time a dot (short element) is transmitted. The LED above the jack illuminates while the output is high.

* **DAH:** Gate output for every time a dash (long element) is transmitted. The LED above the jack illuminates while the output is high.

* **MORSE:** The full Morse code output — a gate fires on every element, dots and dashes alike. This is useful for clocking or envelopes when you don't need to distinguish between the two element types.

---

# How Morse Code Works

* Morse code consists of **dits** (dots) and **dahs** (dashes). A dit is a single unit of measurement from which all of the timing of the transmission is derived.

* One **dit** is a short pulse.

* One **dah** is a pulse held for the length of **three dits**.

* Each character of the alphabet has a corresponding combination of dits and dahs.

* Morse code operators place a **three-dit silence** between each character.

* Morse code operators place a **seven-dit silence** between each word.

* Normally Morse code is internalized by the operator as a sing-song *dah-dit-dah-dah* that fluctuates in timing as a musician would. Morse code is not necessarily a robotic, metronomic signal. To make Morse code musically useful, Dit Dit fixes the timing to a rigid grid that follows the rules of Morse code precisely without fluctuations.

* In Dit Dit, **one dit equals one clock pulse**. Each space inserted with the spacebar indicates the end of a word and inserts a seven-dit silence in the sequence.

* In its default configuration, Dit Dit can be used to send actual Morse code messages if the output drives an audible transmission source. Slow the clock down — it's difficult to decode a pattern that is moving too quickly.

* To get the classic Morse code sound: patch the **MORSE** output into the CV input of a VCA, patch a sine wave oscillator into the audio input of the VCA, and tune the oscillator to somewhere in the range of **400–1000 Hz**.

---

# Timing

Dit Dit implements ITU standard Morse code timing, driven entirely by the incoming clock:

| Element | Duration |
|---|---|
| Dit (dot) | 1 clock tick |
| Dah (dash) | 3 clock ticks (or 2 with Dah:Dit Ratio 2:1) |
| Inter-element gap | 1 clock tick |
| Letter gap | 3 clock ticks |
| Word gap | 7 clock ticks |

In **gate mode** (default), outputs are held high for 50% of the element's clock window, with a minimum of 10 ms. In **trigger mode**, all outputs fire as fixed 10 ms pulses regardless of element length or clock speed.

> **Hardware difference:** The hardware module's trigger mode uses a 25% pulse width. The VCV version uses a fixed 10 ms pulse.

---

# Context Menu Options

### Settings

* **Loop:** When enabled, the buffer loops back to the beginning after the last character and transmits continuously. When disabled, the module stops at the end of the message and the display shows **END**. Default is on. *(Hardware equivalent: F4)*

* **Letter Spacing:** When enabled, a standard 3-tick inter-character gap is inserted between letters. When disabled, the gap is reduced to 1 tick. Default is on. *(Hardware equivalent: F2)*

* **Word Spacing:** When enabled, a standard 7-tick gap is inserted at each space character. When disabled, the gap is reduced to 2 ticks. Default is on. *(Hardware equivalent: F3)*

* **Dah:Dit Ratio 2:1:** By default, a dah is three times the duration of a dit (the ITU standard). When enabled, the dah is shortened to twice the duration of a dit. This produces a faster, more compressed transmission. Default is off. *(Hardware equivalent: F1)*

* **Morse Output: Trigger:** By default, all three outputs fire as gates held high for 50% of the element's clock window. When enabled, all outputs fire as fixed 10 ms triggers instead. Default is off. *(Hardware equivalent: F5)*

---

### Text

* **Load random quote:** Replaces the current buffer with a randomly selected phrase from a built-in list of time-related idioms and sayings.

* **Clear text:** Empties the text buffer and resets the playback position.

* **Delete all memory slots:** Clears all eight saved text slots at once. *(Hardware equivalent: F7 factory reset, which also clears F1–F5 states)*

---

### Save to Slot / Load from Slot

Dit Dit has **eight memory slots** for saving and recalling text. Each slot stores the full buffer contents and persists with the patch.

* **Save to Slot 1–8:** Saves the current buffer into the selected slot. Slots that already contain text show a short preview of their contents in the menu. *(Hardware equivalent: Ctrl+S)*
* **Load from Slot 1–8:** Replaces the current buffer with the contents of the selected slot and resets playback to the beginning. Empty slots cannot be loaded. *(Hardware equivalent: Ctrl+O)*

**Keyboard shortcut:** While hovering over the module and not editing text, pressing **1–8** on the keyboard (or numpad) instantly loads the corresponding slot, if that slot contains text. *(This shortcut is shared with the hardware module's Animation Mode number keys.)*

---

# Morse Code Reference

*Courtesy of electronics-notes.com*

### Letters

| Letter | Code | | Letter | Code |
|---|---|---|---|---|
| A | · − | | N | − · |
| B | − · · · | | O | − − − |
| C | − · − · | | P | · − − · |
| D | − · · | | Q | − − · − |
| E | · | | R | · − · |
| F | · · − · | | S | · · · |
| G | − − · | | T | − |
| H | · · · · | | U | · · − |
| I | · · | | V | · · · − |
| J | · − − − | | W | · − − |
| K | − · − | | X | − · · − |
| L | · − · · | | Y | − · − − |
| M | − − | | Z | − − · · |

### Numbers

| Number | Code | | Number | Code |
|---|---|---|---|---|
| 0 | − − − − − | | 5 | · · · · · |
| 1 | · − − − − | | 6 | − · · · · |
| 2 | · · − − − | | 7 | − − · · · |
| 3 | · · · − − | | 8 | − − − · · |
| 4 | · · · · − | | 9 | − − − − · |

### Punctuation

| Character | Code |
|---|---|
| Period . | · − · − · − |
| Comma , | − − · · − − |
| Question mark ? | · · − − · · |
| Exclamation mark ! | − · − · − − |
| Slash / | − · · − · |
| Apostrophe ' | · − − − − · |
| Semicolon ; | − · − · − · |
| Colon : | − − − · · · |
| Dash − | − · · · · − |
| At sign @ | · − − · − · |
| Left bracket ( | − · − − · |
| Right bracket ) | − · − − · − |
| Equals sign = | − · · · − |
| Addition sign + | · − · − · |
| Ampersand & | · − · · · |
| Quotation mark " | · − · · − · |

---
