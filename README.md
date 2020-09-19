# arduino-tic-tac-toe
Tic Tac Toe in Arduino with a TFT ILI9163C.

## Requirements
You need 2 libraries:
* [sumotoy's ILI9136C library](https://github.com/sumotoy/TFT_ILI9163C)
* [sumotoy's Modified Adafruit library](https://github.com/sumotoy/Adafruit_GFX)
You will also need some hardware:
* TFT ILI9163C
* Joystick module
* Arduino board
* Jumper wires

## Circuit diagram
Circuit diagram for this project goes like:
### TFT ILI9163C
VCC to +5V.

GND to GND.

DI to MOSI pin.

SCK to SCK pin.

CS to SS pin.

RST to 3v3.

RS(DC or A0) to DC or RS pin.

LED to 3V3.
### Joystick

VCC to +5V.

GND to GND.

VRx to A1.

VRy to A2.

SW to pin 7.

## Contribution
All contributions are welcome. If you would like to change something, contact me on [discord](https://discord.gg/HAGQFjr).
