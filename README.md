# goglow-14314-hedwig-firmware
Firmware upgrade for GoGlow 14314 Harry Potter Hedwig Lamp

## Introduction
The GoGlow 14314 Harry Potter Hedwig Lamp is a cute little lamp that can be used as a night light.  It has two LEDs.  One is on the bottom and lights when the lamp is picked up.  The other one is inside the owl and lights when the lamp is lifted and put down.

This is all well and good, but there's no way to turn off the light when you want it to.  You have to wait for 10mins for the light to turn off by itself.  This is not ideal if you want to use it as a night light for a child.

## Original design
The original design is based on an unknown microcontroller.  The laser engraving off the microcontroller has been removed.  The microcontroller has the same pinout as an ATTiny402.

Unfortunately, the unknown microcontroller doesn't respond to UPDI commands.  

## Connections to the MCU
* Pin 1 : VCC
* Pin 3 : momentary switch to ground.  The switch is closed when the lamp is picked up.
* Pin 6 : UPDI and connected to the bottom LED (cathode of the LED to GND)
* Pin 7 : connected to the top LED (cathode of the LED to GND)
* Pin 8 : GND