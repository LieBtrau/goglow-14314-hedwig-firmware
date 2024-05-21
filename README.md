<img src="./doc/Harry_Potter_Hedwig_GoGlow_14341.png" width="500px"/>

Firmware upgrade for the [GoGlow 14314 Harry Potter Hedwig Lamp](https://www.moosetoys.com/fr/our-toys/goglow-fr/harry-potter-hedwig-veilleuse-et-lampe-torche-goglow-buddy/).

## Introduction
The GoGlow 14314 Harry Potter Hedwig Lamp is a cute little lamp that can be used as a night light.  It has two LEDs.  One is on the bottom and lights when the lamp is picked up.  The other one is inside the owl and lights when the lamp is standing upright.

This is all well and good, but there's no way to turn off the light when you want it to.  You have to wait for 10mins for the light to turn off by itself.  This is not ideal if you want to use it as a night light for a child.

## Original design
The original design is based on an unknown microcontroller.  The laser engraving off the microcontroller has been removed.  The microcontroller has the same pinout as an ATTiny402.

Unfortunately, the unknown microcontroller doesn't respond to UPDI commands in this circuit.  The bottom LED, connected to the UPDI-pin prevents communication.  I didn't bother to investigate further.

## Connections to the MCU
* Pin 1 : VCC
* Pin 3 : normally closed momentary switch to ground.  The switch is closed when the lamp is picked up.
* Pin 6 : UPDI and connected to the bottom LED (cathode of the LED to GND)
* Pin 7 : connected to the top LED (cathode of the LED to GND)
* Pin 8 : GND

### Power consumption at VCC=4.5V
* Bottom LED on : 18.2mA
* Top LED on : 18.13mA
* Both LEDs off : 11.5µA

## Problems
It's annoying that the UPDI/RST pin is connected to the bottom LED, while there are three unused GPIO-pins on the MCU.  We might keep the existing circuit and use 12V UPDI-programming, but it will be much easier to patch the circuit and connect pin 5 to the bottom LED, instead of pin 6.

The original repository can be found at [github](https://github.com/LieBtrau/goglow-14314-hedwig-firmware).