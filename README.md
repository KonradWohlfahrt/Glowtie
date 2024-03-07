# Glowtie
This repository contains the project files for the Glowtie, a bowtie with individually addressable LEDs.
It was originally designed by [Stephen Hawes](https://github.com/sphawes/glowtie).
At the beginning, I used [Raxathor's](https://github.com/raxathor/GlowTie) adaptation.
Due to the lack of documentation, especially where to place the smd components, it was a real struggle to build it. Furthermore, I discovered issues with the design and thus settled on creating my own version.
In the following, I'm going to explain how you can build Raxathor's adaptation and of course mine!
Let's get started!


***
# Raxathor's Adaptation
Check out the instructable from [Raxathor](https://www.instructables.com/GlowTie/).

**Materials:**
| Component |  Amount  |
|:----------|---------:|
| custom pcb | 1 |
| lipo battery (max size: 20 x 30 x 7mm) | 1 |
| ESP-12F | 1 |
| TP4056A | 1 |
| DW01-A | 1 |
| FS8205A | 1 |
| TC2117-3.3V ~~AMS1117 3.3V~~ | 1 |
| SMD Power Slide Switch | 1 |
| Micro USB | 1 |
| WS2812b LEDs (Neopixel) | 13 |
| 10k 0805 | 5 |
| 1k 0805 | 3 |
| 100r 0805 | 1 |
| 10k ~~1.2k~~ 0805 (charge progamming resistor, check TP4056A datasheet) | 1 |
| red LED 0805 | 1 |
| green LED 0805 | 1 |
| 0.1uF 0805 | 2 |
| 10uF 0805 | 2 |
| 1uF 0805 | 1 |
| 100uF 1206 | 1 |

**Soldering:**
Solder the components to the indicated boxes on the images.
Make sure to program your chip in advance or add some programming cables to the back.
![Soldering Front](https://github.com/Donut-Studio/Glowtie/blob/main/images/RaxathorSolderingFront.png)
![Soldering Back](https://github.com/Donut-Studio/Glowtie/blob/main/images/RaxathorSolderingBack.png)

**Issues:**
During my tests I discovered a few flaws with the design.

First of all, the AMS1117 LDO has a too high dropout voltage.
This LDO is not suitable for powering the bowtie!
My alternative is the TC2117. Stephen Hawes used an MCP1826S with only 250mV of dropout. 
However, the pinout of this LDO does not fit on Raxathor's pcb.

Furthermore, soldering with a soldering iron was possible but challenging.
Especially with the small pins of the FS8205A Mosfet.
My smd switches (I've tried muliple) wouldn't always work correctly.

There are many other small, annoying design choices I fixed.


***
# My Adaptation
That's why I created my own Glowtie. Those are all the changed I made:
- replaced charging circuit with popular charging board (easier to solder)
- added PH2.0 connector for LiPo battery
- replaced smd with through-hole switch
- added holes for M3 screws to secure the pcb on the 3d print
- new LDO (MIC39100), identical to the pinout of the one used by Stephen Hawes
- voltage divider to measure the voltage of the LiPo battery
- resistor from esp to first led, to prevent damage by high current
- added a button
- better programming pins
- every led has its own capacitor as recommended in the datasheet 

**Materials:**
| Component | Amount | Silkscreen label |
|:----------|:------:|-----------------:|
| custom pcb | 1 | - |
| lipo battery (max size: 20 x 30 x 7mm) | 1 | - |
| ESP-12F | 1 | U4 |
| LiPo charge board | 1 | U3 |
| MIC39100 | 1 | U5 |
| Switch | 1 | U1 |
| button | 1 | U7 |
| Micro USB | 1 | U2 |
| WS2812b LEDs (Neopixel) | 13 | LED1 - LED13 |
| 100nF 0805 | 14 | C1 - C13, C17 |
| 10uF 0805 | 1 | C14 |
| 1uF 0805 | 1 | C15 |
| 100uF 1206 | 1 | C16 |
| 10k 0805 | 6 | R1 - R4, R6, R8 |
| 33k 0805 | 1 | R5 |
| 220r 0805 | 1 | R7 |
| PH2.0 2p | 1 | U6 |
| 90 degree connector 6p | 1 | - |

**Soldering:**
Solder the components as indicated in the table and images.
![Soldering Front](https://github.com/Donut-Studio/Glowtie/blob/main/images/SolderingFront.png)
![Soldering Back](https://github.com/Donut-Studio/Glowtie/blob/main/images/SolderingBack.png)
Also check your datasheet of the battery. The charging board is commonly used for 18650 batteries.
However, the charging current of 1A is not suitable for the batteries used here.
Here's a great [video](https://www.youtube.com/watch?v=6asCEBm4ZAw) showing how you can change the charging resistor (I've used 5.1k 0805).


***
# Programming (for both designs)
In the `src/` folder you'll find my versions of the source code for each pcb design.
Open `Glowtie.ino` and change the ssid and password of the access point to your preferences.
Install the `Adafruit Neopixel` library and the `ESP8266` board manager.
Connect the Glowtie with a FTDI programmer (3.3V logic level) like this and upload the code:
- TX (Glowtie) => RX (FTDI)
- RX (Glowtie) => TX (FTDI)
- VCC (Glowtie) => VCC (FTDI)
- GND (Glowtie) => GND (FTDI)
- Flash (Glowtie) => GND (FTDI)
> If you did not connect flash to gnd, the chip will no be in programming-mode. You may also leave flash disconnected and just hold down the button while plugging in the FTDI programmer.


***
# The end
Great, now you have yourself your own Glowtie!
There is an enclosure which you can print in a 3d-printer.
If you press the button for 1.5 seconds, the current effect will be saved to the eeprom.
A short press will activate(green flash) / deactivate(red flash) the random mode which circles through the effects.
Make sure to turn off the glowtie when loading!
Remember to check out Stephen Hawes and Raxathor, as this repository is based on theirs.

![Glowtie](https://github.com/Donut-Studio/Glowtie/blob/main/images/Glowtie.png)