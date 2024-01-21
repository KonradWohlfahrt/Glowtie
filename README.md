# Glowtie
This repository contains the project files for the Glowtie, a bowtie with individually addressable LEDs.
It was originally designed by [Stephen Hawes](https://github.com/sphawes/glowtie).
I've used the modified project of the Glowtie from [Raxathor](https://github.com/raxathor/GlowTie) to create my own version of it. Since the build process involves a lot of smd soldering, I don't recommend this as a beginner project. 
When building my own, the lack of documentation, especially where to place the components, was a real struggle. In the following, I'm going to explain everything you need to know, to build your own Glowtie.
Let's get started!

***
# Materials
Those are all the components you'll need for the Glowtie:
- 1x custom pcb
- 1x ESP12F module
- 1x AMS1117-3.3V
- 1x TP4056A
- 1x DW01-A
- 1x FS8205A
- 1x LiPo Battery
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
I've downloaded the schematic from [Raxathor instructable](https://www.instructables.com/GlowTie/).
The circuit on the pcb looks like the following:
![Schematic1](https://github.com/Donut-Studio/Glowtie/blob/images/Schematic1.png)
![Schematic2](https://github.com/Donut-Studio/Glowtie/blob/images/Schematic2.png)

The pcb I've used had no indication on where the components should be placed.
By tracing the paths and looking at the schematic, I was able to get everything in the right spot.
Look at these pictures if you are not sure where to place your components:

![Soldering Front](https://github.com/Donut-Studio/Glowtie/blob/images/SolderingFront.png)
![Soldering Back](https://github.com/Donut-Studio/Glowtie/blob/images/SolderingBack.png)

Make sure to program your chip in advance or, add some programming cables to the back.
Now your pcb should look something like this:
![Done](https://github.com/Donut-Studio/Glowtie/blob/images/PCBFinished.png)

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
# The end
Great, now you have yourself your own Glowtie!
There is an enclosure which you can print in a 3d-printer.
Remember to check out Stephen Hawes and Raxathor, as this repository is based on theirs.

> Make sure that your Glowtie is turned off when you charge the battery!