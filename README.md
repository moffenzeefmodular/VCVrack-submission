### Moffenzeef VCV Rack Plugins
By Ross Fish Feb 2025 
[Moffenzeef Website](https://moffenzeefmodular.com)

## v2.0.0
- Initial Release 

## v2.0.1
- Removed static variables from all modules
- Fixed Count module. Count had variables declared in the wrong spot in the code, causing multiple instances to communicate with one another by mistake.

## v2.0.2 
- Removed DC offset from GMO
- Fixed Kriket output scaling and clamped to 10vpp
- Updated all jack labels to show their data type (CV, Gate, Audio)
- Improved LED behavior for all modules

## v2.0.3 
- Fixed LED outputs on Count: start output LED was non functioning
- Fixed LED outputs on Count: start/stop switch turns off all LEDs

## v2.0.4
- Dark panels integrated for all modules

## v2.1.0 
- Added 2hp blank panel
- Added 14hp blank panel
- Added Swarm module 
- Added [INTENSIFIES] module

## v2.2.0 
- Added The Runner

## v2.3.0 
- Added Stargazer
- Added Kleztizer
- Added Tantz!
- Themed jacks and screws updated for all modules
- All wavetables and sequences moved to res folder
- Renamed widgets to start with letters instead of numbers

## v2.4.0 
- Added Quad Deviant
- Changed positive LED indicators on Deviant to green
- Removed deprecated use of setBrightnessSmooth from BadIdea1800 

## v2.5.0
- Stargazer: Added external audio input to oscillator section.
- Stargazer: Added ability to type note value directly onto pitch knob to avoid need for external tuner 
- Stargazer: Added a new LFO shape "Smooth Random" which is a sort of drunken walk sine wave LFO that randomly changes it's frequency within the given range.
- Kleztizer: Repaired voicing and inversion knobs. Inversion was unresponsive in "Closed" mode. "Open" mode was not implemented correctly
- [INTENSIFIES]: Added stereo output.
- Removed static variables from Tantz so that multiple instances don’t conflict with one another
- Repaired Tantz! Sequences (there was all sorts of goofy stuff happening)
- Added user manuals for Stargazer, Quad Deviant, Tantz!, and Kleztizer
- Added themed screws and themed jacks to Quad Deviant

## Tehom Beta: v2.6.0-rc1
- Tehom beta release 

## Tehom Beta: v2.6.0-rc2
**Audio files:**
- Cut file sizes down to 1 minute loops to speed up plugin loading time 

**Art updates:**
- Changed Tehom title to english

**UI updates:**
- Added fish icon
- Double click on XY pad to reset to center
- Add "Record" on hover for record button

**Context menu and feature updates:**
- **Added scrubbing feature** - LED bezels now behave like a tape reel / vinyl record and let you scrub
- **Added Continous Record mode** 
- **Added Record Source: Main Output** - Buffer records main output instead of internal SOS loop
- **Added XY Pad Pans Audio** - Audio pans in the stereo field based on X axis.
- **Added Wander mode to cursor**
- Rename "Persist" to "Cursor Trailing"
- Re-organized menus and removed sub menus to clean up organization
- Added cursor select - fish / circle 🐟 

**User manual updated with all relevant changes** 