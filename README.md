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
- Added ability to type note value directly onto pitch knob of Stargazer to avoid need for external tuner 