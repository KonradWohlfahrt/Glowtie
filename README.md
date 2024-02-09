# Glowtie
This repository contains the project files for the Glowtie, a bowtie with individually addressable LEDs.
It was originally designed by [Stephen Hawes](https://github.com/sphawes/glowtie).
I've used the modified project of the Glowtie from [Raxathor](https://github.com/raxathor/GlowTie) to create my own version of it. Both the pcb and stls were not designed by me. However, the code is!

Since the build process involves a lot of smd soldering, I don't recommend this as a beginner project. 
When building my own, the lack of documentation, especially where to place the components, was a real struggle. In the following, I'm going to explain everything you need to know, to build your own Glowtie.
Let's get started!


***
# Materials
Those are all the components you'll need for the Glowtie:
- 1x custom pcb
- 1x ESP12F module
- 1x AMS1117-3.3V / TC2117-3.3V (take a look at `#LDO-Alternative`)
- 1x TP4056A
- 1x DW01-A
- 1x FS8205A
- 1x LiPo Battery (max size: 20 x 30 x 7mm)
- 1x SMD Power Slide Switch
- 1x Micro USB
- 13x WS2812b LEDs (Neopixel)
- 5x 10 kOhm 0805
- 3x 1 kOhm 0805
- 1x 100 Ohm 0805
- 1x 1.2 kOhm 0805
- 1x Red LED 0805
- 1x Green LED 0805
- 2x 0.1uF 0805
- 2x 10uF 0805
- 1x 1uF 0805
- 1x 100uF 1206


***
# Schematics & Soldering
These are the schematics from [Raxathor's instructable](https://www.instructables.com/GlowTie/).
The circuit on the pcb looks like the following:
![Schematic1](https://github.com/Donut-Studio/Glowtie/blob/main/images/Schematic1.png)
![Schematic2](https://github.com/Donut-Studio/Glowtie/blob/main/images/Schematic2.png)

The pcb I've used had no indication on where the components should be placed.
By tracing the paths and looking at the schematic, I was able to get everything in the right spot.
Look at these pictures if you are not sure where to place your components:

![Soldering Front](https://github.com/Donut-Studio/Glowtie/blob/main/images/SolderingFront.png)
![Soldering Back](https://github.com/Donut-Studio/Glowtie/blob/main/images/SolderingBack.png)

Make sure to program your chip in advance or, add some programming cables to the back.
Now your pcb should look something like this:
![Done](https://github.com/Donut-Studio/Glowtie/blob/main/images/PCBDone.png)


***
# Programming
In the `src/Glowtie/` folder you'll find my version of the source code.
Open the `Glowtie.ino` file and change the ssid and password to your preferences.
This will be the access point you can connect to.
If you don't want the chip to save the selected effect and color to the eeprom, 
remove the `#include <EEPROM.h>` and the beginning and the marked code snippets in lines 77 and 200.
Install the `Adafruit Neopixel` library and the `ESP8266` board manager.
Connect the Glowtie with a FTDI Programmer (3.3V logic level) like this and upload the code:
- TX (Glowtie) => RX (FTDI)
- RX (Glowtie) => TX (FTDI)
- VCC (Glowtie) => VCC (FTDI)
- GND (Glowtie) => GND (FTDI)
- Flash (Glowtie) => GND (FTDI)
> If you did not connect flash to gnd, the chip will no be in programming-mode.


***
# LDO-Alternative
After building a glowtie myself, I discovered another flaw in the pcb design from Raxathor:
The current LDO (AMS1117) has a dropout voltage of 1V. Meaning, that the difference between the input and output voltage should be at least 1V. 
The voltage levels of most LiPo batteries with one cell look like this:
- 4.2V (fully charged)
- 3.7V (normal)
- 3.5V (low, discharging even lower may reduce longevity)

In this case the difference is about 0.9V with a fully charged battery and even lower for a used one.
Thus, the LDO will drag down the voltage towards ground and reduce the output voltage.
With a battery voltage of 4V the ESP (and LEDs) would not work reliable anymore. Not to mention the "low battery" mode which will stop all animations and the webserver eventhough the battery could still supply power.

Stephen Hawes used an `MCP1826S` with only 250mV of dropout. However, the pinout of this LDO is not suitable for Raxathor's pcb. An alternative would be the `TC2117` (I've tested this LDO and it works perfectly!).
I'd love to come back to this and design my own pcb, but that's a project for the future.


***
# The end
Great, now you have yourself your own Glowtie!
There is an enclosure which you can print in a 3d-printer.
Remember to check out Stephen Hawes and Raxathor, as this repository is based on theirs.

> Make sure that your Glowtie is turned off when you charge the battery!