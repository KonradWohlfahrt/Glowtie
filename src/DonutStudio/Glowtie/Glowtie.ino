#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#include "webserver.h"

#define BUTTONPIN 0
#define SAVETIME 1500
#define DOUBLEPRESSINTERVAL 400

#define PIN 12
#define NUMPIXELS 13
#define EFFECTREFRESHTIME 75

#define BATTCHECKTIME 10000
#define BATTREADINGS 10

#define LOWBATTERYVALUE 825
#define SHUTDOWNVALUE 815


const char *ssid = "Glowtie";
const char *password = "pleaseletmein";

const uint32_t red = 0xff0000;
const uint32_t green = 0x00ff00;

const byte brightness[] = { 10, 20, 30, 40 };
const unsigned long randomTimeMin[] = { 120000, 60000, 30000, 10000 };
const unsigned long randomTimeMax[] = { 360000, 120000, 60000, 20000 };


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
ESP8266WebServer server(80);


byte brightnessIndex = 1;


byte redValue = 127;
byte greenValue = 127;
byte blueValue = 127;

bool isRandomMode = false;
bool chooseRandomColor = false;
int speedIndex = 1;
unsigned long randomWait;
unsigned long lastRandomUpdate = 0;

unsigned long lastBatteryCheck = 0;
unsigned long lastEffectUpdate = 0;

unsigned long lastButtonPress = 0;
bool awaitingDoublePress = false;



/* --- UTILITY START --- */

float getVoltage() { return 4.3f / 1023 * analogRead(A0); } // formular: Vbat = (R6 + R5) / (R6 * 1023) * analogRead(A0)
int getBatteryPercent() { return (int)(((float)(analogRead(A0)) - LOWBATTERYVALUE) / (1023 - LOWBATTERYVALUE) * 100); }
bool checkBattery(int minimum)
{
  int avg = 0;
  for (byte b = 0; b < BATTREADINGS; b++)
  {
    avg += analogRead(A0);
    delay(2);
  }
  return (avg / BATTREADINGS) <= minimum;
}

/* --- UTILITY END --- */



#include "LedEffect.h"

#include "Solid.h"

#include "Breathe.h"
#include "Pulse.h"
#include "Bar.h"
#include "Burst.h"

#include "Infinity.h"
#include "Chaser.h"
#include "Circles.h"
#include "Symmetry.h"

#include "Starfield.h"
#include "Rainbow.h"
#include "Filler.h"

const byte effectCount = 19;
LedEffect * effects[] = { new Solid(), new SolidRainbow(),
                          new Breathe(), new Pulse(), new Bar(), new BurstIn(), new BurstOut(), 
                          new Infinity(), new InfinityFill(), new Chaser(), new ChaserFill(), new Circles(), new CirclesFill(), new Symmetry(), new SymmetryFill(),
                          new Starfield(), new Rainbow(), new RainbowFill(), new Filler() };
byte modeIndex = 0;
int effectCounter = 0;

void setup() 
{
  pinMode(A0, INPUT);
  pinMode(BUTTONPIN, INPUT);


  EEPROM.begin(7);
  delay(250);
  modeIndex = constrain(EEPROM.read(0), 0, effectCount - 1);
  redValue = EEPROM.read(1);
  greenValue = EEPROM.read(2);
  blueValue = EEPROM.read(3);

  chooseRandomColor = EEPROM.read(4) == 0;
  speedIndex = constrain(EEPROM.read(5), 0, 3);
  brightnessIndex = constrain(EEPROM.read(6), 0, 3);


  pixels.begin();
  pixels.setBrightness(brightness[brightnessIndex]);

  // ssid, password, default channel, do not hide ssid, one simultaneous connections
  if (WiFi.softAP(ssid, password, 1, false, 1)) 
    scrollAnim(green);
  else
    flashAnim(red);

  server.on("/", handleRoot);
  server.on("/red", handleGetRed);
  server.on("/green", handleGetGreen);
  server.on("/blue", handleGetBlue);
  server.on("/mode", handleGetMode);
  server.on("/vcc", handleGetVCC);
  server.on("/color", handleGetRandomColor);
  server.on("/speed", handleGetSpeed);
  server.on("/brightness", handleGetBrightness);
  server.begin();

  startEffect();
}
void loop() 
{
  server.handleClient();

  if (millis() - lastBatteryCheck >= BATTCHECKTIME) 
  {
    if (checkBattery(LOWBATTERYVALUE))
    {
      WiFi.mode(WIFI_OFF);
      flashAnim(red);
      lowBattery();
      while (true)
      {
        // turn everything off
        if (checkBattery(SHUTDOWNVALUE))
        {
          disableLEDs();
          ESP.deepSleep(0);
        }
        delay(5);
      }
    }

    lastBatteryCheck = millis();
  }


  handleEffects();


  if (digitalRead(BUTTONPIN) == 0)
  {
    bool shortPress = true;
    unsigned long started = millis();
    while (digitalRead(BUTTONPIN) == 0) 
    {
      handleEffects();

      if (shortPress && millis() - started >= SAVETIME)
      {
        shortPress = false;
        flashAnim(green);
        EEPROM.write(0, modeIndex);
        EEPROM.write(1, redValue);
        EEPROM.write(2, greenValue);
        EEPROM.write(3, blueValue);
        EEPROM.commit();
        delay(250);
      }
      
      delay(5);
    }

    if (shortPress)
    {
      if (!awaitingDoublePress)
      {
        awaitingDoublePress = true;
        lastButtonPress = millis();
      }
      else if (millis() - lastButtonPress < DOUBLEPRESSINTERVAL)
      {
        setRandomMode(!isRandomMode);
        awaitingDoublePress = false;
      }
    }
  }
  if (awaitingDoublePress && millis() - lastButtonPress >= DOUBLEPRESSINTERVAL)
  {
    awaitingDoublePress = false;
    showBatteryPercentage();
    startEffect();
  }

  if (isRandomMode)
  {
    if (millis() - lastRandomUpdate >= randomWait)
    {
      if (chooseRandomColor)
      {
        uint32_t col = getRandomColor();
        redValue = col >> 16;
        greenValue = col >> 8;
        blueValue = (byte)col;
      }
      
      int m;
      do {
        m = random(0, effectCount);
      } while (m == modeIndex);
      modeIndex = m;
      randomWait = random(randomTimeMin[speedIndex], randomTimeMax[speedIndex]);
      lastRandomUpdate = millis();
      startEffect();
    }
  }
}

void startEffect()
{
  pixels.clear();
  effectCounter = 0;
  effects[modeIndex]->onStart();
}
void handleEffects()
{
  if (!effects[modeIndex]->getIsStatic() && millis() - lastEffectUpdate >= effects[modeIndex]->getSpeed())
  {
    effects[modeIndex]->onUpdate(effectCounter);
    effectCounter++;
    lastEffectUpdate = millis();
  }
}
void setRandomMode(bool value)
{
  isRandomMode = value;
  flashSingleAnim(isRandomMode ? green : red);
  if (isRandomMode)
  {
    lastRandomUpdate = millis();
    randomWait = random(randomTimeMin[speedIndex], randomTimeMax[speedIndex]);
  }
  else
  {
    modeIndex = constrain(EEPROM.read(0), 0, effectCount - 1);
    redValue = EEPROM.read(1);
    greenValue = EEPROM.read(2);
    blueValue = EEPROM.read(3);
  }

  startEffect();
}


/* --- HTML --- */

void handleGetRed() { server.send(200, "text/plane", String(redValue)); }
void handleGetGreen() { server.send(200, "text/plane", String(greenValue)); }
void handleGetBlue() { server.send(200, "text/plane", String(blueValue)); }
void handleGetMode() { server.send(200, "text/plane", String(modeIndex)); }
void handleGetVCC() { server.send(200, "text/plane", String(getBatteryPercent()) + "% (" + String(getVoltage(), 2) + "V)"); }
void handleGetRandomColor() { server.send(200, "text/plane", (chooseRandomColor ? "1" : "0")); }
void handleGetSpeed() { server.send(200, "text/plane", String(speedIndex)); }
void handleGetBrightness() { server.send(200, "text/plane", String(brightnessIndex)); }
void handleRoot() 
{
  if (server.args() > 0) 
  {
    if (server.hasArg("red"))
    {
      redValue = (byte)server.arg("red").toInt();
      greenValue = (byte)server.arg("green").toInt();
      blueValue = (byte)server.arg("blue").toInt();
      modeIndex = (byte)server.arg("mode").toInt();

      isRandomMode = false;

      server.send(200, "text/html", index_html);

      startEffect();
    }
    else
    {
      chooseRandomColor = server.hasArg("randomcolor");
      EEPROM.write(4, chooseRandomColor ? 0 : 255);
      speedIndex = constrain(server.arg("speed").toInt(), 0, 3);
      EEPROM.write(5, speedIndex);
      brightnessIndex = constrain(server.arg("brightness").toInt(), 0, 3);
      EEPROM.write(6, brightnessIndex);

      if (pixels.getBrightness() != brightness[brightnessIndex])
        pixels.setBrightness(brightness[brightnessIndex]);

      server.send(200, "text/html", index_html);
  
      flashAnim(green);
      EEPROM.commit();
      delay(250);
      
      startEffect();
    }
  }
  else
    server.send(200, "text/html", index_html);
}