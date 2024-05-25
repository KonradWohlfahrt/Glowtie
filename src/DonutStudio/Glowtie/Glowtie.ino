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

#define LOWBATTERYVALUE 835
#define SHUTDOWNVALUE 820


const char *ssid = "Glowtie";
const char *password = "pleaseletmein";

const uint32_t red = 0xff0000;
const uint32_t green = 0x00ff00;

const byte brightness[] = { 10, 20, 30, 40 };
const unsigned long randomTimeMin[] = { 120000, 60000, 30000, 10000 };
const unsigned long randomTimeMax[] = { 360000, 120000, 60000, 20000 };

enum GlowtieMode
{
  // STATIC
  OFF = 0,
  SOLID = 1,
  TIE = 2,

  // DYNAMIC
  BREATHE = 3,
  PULSE = 4,
  BAR = 5,
  BURSTIN = 6,
  BURSTOUT = 7,

  INFINITYCIRCLE = 8,
  INFINITYCIRCLEFILL = 9,
  CHASER = 10,
  CHASERFILL = 11,
  CIRCLES = 12,
  CIRCLESFILL = 13,
  SYMMETRY = 14,
  SYMMETRYFILL = 15,
  
  STARFIELD = 16,
  RAINBOW = 17,
  FILLER = 18
};


const byte infinityeffect[] = { 0, 1, 2, 3, 4, 5, 6, 12, 11, 10, 9, 8, 7, 6 };
const byte chasereffect[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 6 };
const byte circleseffect[] = { 6, 0, 1, 2, 3, 4, 5, 6, 12, 11, 10, 9, 8, 7 };
const byte symmetryeffect[] = { 6, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
const byte bareffect[] = { 2, 1, 0, 6, 12, 11, 10, 3, 4, 5, 6, 7, 8, 9 };





Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
ESP8266WebServer server(80);

byte brightnessIndex = 1;
GlowtieMode mode = SOLID;
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
int effectIndex = 0;

unsigned long lastButtonPress = 0;
bool awaitingDoublePress = false;


byte lastPixel = 0;
long firstPixelHue = 0;

uint32_t halfColor;
uint32_t randomColor;
byte randomRed;
byte randomGreen;
byte randomBlue;
bool isRandomPhase = false;


void setup() 
{
  pixels.begin();
  pixels.setBrightness(20);

  pinMode(A0, INPUT);
  pinMode(BUTTONPIN, INPUT);

  lowBattery();

  EEPROM.begin(7);
  delay(500);
  mode = (GlowtieMode)(constrain(EEPROM.read(0), 0, 18));
  redValue = EEPROM.read(1);
  greenValue = EEPROM.read(2);
  blueValue = EEPROM.read(3);

  chooseRandomColor = EEPROM.read(4) == 0;
  speedIndex = constrain(EEPROM.read(5), 0, 3);
  brightnessIndex = constrain(EEPROM.read(6), 0, 3);
  if (brightnessIndex != 1)
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

  delay(100);

  updatePixels();
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
          disableDisplay();
          ESP.deepSleep(0);
        }
        delay(5);
      }
    }

    lastBatteryCheck = millis();
  }

  handleDynamicEffects();

  if (digitalRead(BUTTONPIN) == 0)
  {
    bool shortPress = true;
    unsigned long started = millis();
    while (digitalRead(BUTTONPIN) == 0) 
    {
      handleDynamicEffects();

      if (shortPress && millis() - started >= SAVETIME)
      {
        shortPress = false;
        flashAnim(green);
        EEPROM.write(0, (byte)mode);
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
        toggleRandomMode();
        awaitingDoublePress = false;
      }
    }
  }

  if (awaitingDoublePress && millis() - lastButtonPress >= DOUBLEPRESSINTERVAL)
  {
    awaitingDoublePress = false;
    showBatteryPercentage();
    updatePixels();
  }

  if (isRandomMode)
  {
    if (millis() - lastRandomUpdate >= randomWait)
    {
      uint32_t col = pixels.gamma32(pixels.Color(random(256), random(256), random(256)));
      redValue = col >> 4;
      greenValue = col >> 2;
      blueValue = col & 0x0000ff;
      GlowtieMode m = (GlowtieMode)random(1, 19);
      while (m == mode)
        m = (GlowtieMode)random(1, 19);
      mode = m;
      randomWait = random(randomTimeMin[speedIndex], randomTimeMax[speedIndex]);
      lastRandomUpdate = millis();
      updatePixels();
    }
  }
}


/* --- HTML --- */

void handleGetRed() { server.send(200, "text/plane", String(redValue)); }
void handleGetGreen() { server.send(200, "text/plane", String(greenValue)); }
void handleGetBlue() { server.send(200, "text/plane", String(blueValue)); }
void handleGetMode() { server.send(200, "text/plane", String((int)mode)); }
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
      mode = (GlowtieMode)server.arg("mode").toInt();

      isRandomMode = false;

      server.send(200, "text/html", index_html);

      updatePixels();
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
      

      isRandomMode = false;
      updatePixels();
    }
  }
  else
    server.send(200, "text/html", index_html);
}


/* --- UTILITY --- */

float getVoltage() { return 4.3f / 1023 * analogRead(A0); }
int getBatteryPercent() { return (int)(((float)(analogRead(A0)) - LOWBATTERYVALUE) / (1023 - LOWBATTERYVALUE) * 100); }
bool checkBattery(int minimum)
{
  int avg = 0;
  for (byte b = 0; b < BATTREADINGS; b++)
  {
    avg += analogRead(A0);
    delay(2);
  }
  avg /= BATTREADINGS;

  return avg <= minimum;
}
void updatePixels()
{
  effectIndex = 0;
  pixels.clear();
  switch (mode)
  {
    case OFF:
      disableDisplay();
      break;
    case SOLID:
      solid();
      break;
    case TIE:
      tie();
      break;
    case BAR:
      randomColor = pixels.Color(redValue, greenValue, blueValue);
      halfColor = pixels.Color(redValue / 2, greenValue / 2, blueValue / 2);
      break;
    case FILLER:
      isRandomPhase = false;
      randomColor = pixels.Color(redValue, greenValue, blueValue);
      break;
  }
}
void toggleRandomMode()
{
  isRandomMode = !isRandomMode;
  flashSingleAnim(isRandomMode ? green : red);
  if (isRandomMode)
  {
    lastRandomUpdate = millis();
    randomWait = random(randomTimeMin[speedIndex], randomTimeMax[speedIndex]);
  }
  else
    mode = (GlowtieMode)EEPROM.read(0);

  if (mode == SOLID || mode == TIE)
    updatePixels();
}
void scrollAnim(uint32_t color)
{
  for (int i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, color);
    pixels.show();
    delay(EFFECTREFRESHTIME);
  }
  for (int i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, 0, 0, 0);
    pixels.show();
    delay(EFFECTREFRESHTIME);
  }
}
void flashAnim(uint32_t color)
{
  for (int i = 0; i < 10; i++)
  {
    if (i % 2 == 0)
      pixels.fill(color);
    else
      pixels.clear();
    
    pixels.show();
    delay(EFFECTREFRESHTIME);
  }
}
void flashSingleAnim(uint32_t color)
{
  for (int i = 0; i < 10; i++)
  {
    if (i % 2 == 0)
      pixels.setPixelColor(6, color);
    else
      pixels.setPixelColor(6, 0);
    
    pixels.show();
    delay(EFFECTREFRESHTIME);
  }
}
void showBatteryPercentage()
{
  for (int i = 0; i < 7; i++)
  {
    setBar(i, green);
    pixels.show();
    delay(EFFECTREFRESHTIME);
  }

  int p = getBatteryPercent();
  int ledAmount = (int)(0.06f * p + 0.5f);
  uint32_t c = green;
  if (p <= 20)
    c = red;
  else if (p <= 45)
    c = 0xff4000;

  for (int i = 6; i >= 0; i--)
  {
    if (i > ledAmount)
      setBar(i, 0);
    else
      setBar(i, c);
    pixels.show();
    delay(EFFECTREFRESHTIME);
  }

  delay(2500);
}
byte getAverage(byte a, byte b)
{
  return (byte)(((int)a + (int)b) / 2);
}
byte shiftByte(byte a, byte shift)
{
  if (shift > a || a > 255 - shift)
    return a;
  return a + random(-shift, shift + 1);
}
byte getShift(byte c)
{
  int shift = c / 2;
  if (c > 127)
    shift = (255 - c) / 2;

  return shiftByte(c, shift);
}
void setBar(byte index, uint32_t color)
{
  if (0 > index || index > 6)
    return;
  pixels.setPixelColor(bareffect[index], color);
  pixels.setPixelColor(bareffect[index + 7], color);
}

void handleDynamicEffects()
{
  if (millis() - lastEffectUpdate >= EFFECTREFRESHTIME)
  {
    switch (mode)
    {
      case BREATHE:
        breathe();
        break;
      case PULSE:
        pulse();
        break;
      case BAR:
        bar();
        break;
      case BURSTIN:
        burstIn();
        break;
      case BURSTOUT:
        burstOut();
        break;

      case INFINITYCIRCLE:
        infinitycircle();
        break;
      case INFINITYCIRCLEFILL:
        infinitycirclefill();
        break;
      case CHASER:
        chaser();
        break;
      case CHASERFILL:
        chaserfill();
        break;
      case CIRCLES:
        circles();
        break;
      case CIRCLESFILL:
        circlesfill();
        break;
      case SYMMETRY:
        symmetry();
        break;
      case SYMMETRYFILL:
        symmetryfill();
        break;
      
      case STARFIELD:
        starfield();
        break;
      case RAINBOW:
        rainbow();
        break;
      case FILLER:
        filler();
        break;
    }
    effectIndex++;
    lastEffectUpdate = millis();
  }
}


/* --- STATIC EFFECTS --- */

void lowBattery() 
{
  pixels.clear();
  pixels.setPixelColor(6, 50, 0, 0);
  pixels.show();
}

void disableDisplay()
{
  pixels.clear();
  pixels.show();
}
void solid()
{
  pixels.fill(pixels.Color(redValue, greenValue, blueValue));
  pixels.show();
}
void tie()
{
  pixels.fill(pixels.Color(redValue, greenValue, blueValue));
  pixels.setPixelColor(0, 0, 0, 0);
  pixels.setPixelColor(5, 0, 0, 0);
  pixels.setPixelColor(7, 0, 0, 0);
  pixels.setPixelColor(12, 0, 0, 0);
  pixels.show();
}


/* --- DYNAMIC EFFECTS --- */

void breathe()
{
  if (effectIndex >= 22)
    effectIndex = 0;

  if (effectIndex == 0)
    pixels.fill(pixels.Color(redValue / 3, greenValue / 3, blueValue / 3));
  else if (effectIndex == 2)
    pixels.fill(pixels.Color(redValue * 2 / 3, greenValue * 2 / 3, blueValue * 2 / 3));
  else if (effectIndex == 4)
    pixels.fill(pixels.Color(redValue, greenValue, blueValue));

  else if (effectIndex == 10)
    pixels.fill(pixels.Color(redValue * 2 / 3, greenValue * 2 / 3, blueValue * 2 / 3));
  else if (effectIndex == 12)
    pixels.fill(pixels.Color(redValue / 3, greenValue / 3, blueValue / 3));
  else if (effectIndex == 14)
    pixels.clear();

  if (effectIndex % 2 == 0)
    pixels.show();
}
void pulse()
{
  if (effectIndex >= 27)
    effectIndex = 0;

  if (effectIndex == 0)
  {
    pixels.clear();
    pixels.setPixelColor(6, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 2)
  {
    pixels.setPixelColor(6, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(0, redValue, greenValue, blueValue);
    pixels.setPixelColor(5, redValue, greenValue, blueValue);
    pixels.setPixelColor(7, redValue, greenValue, blueValue);
    pixels.setPixelColor(12, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 4)
  {
    pixels.clear();

    pixels.setPixelColor(0, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(5, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(7, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(12, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(1, redValue, greenValue, blueValue);
    pixels.setPixelColor(4, redValue, greenValue, blueValue);
    pixels.setPixelColor(8, redValue, greenValue, blueValue);
    pixels.setPixelColor(11, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 6)
  {
    pixels.clear();

    pixels.setPixelColor(1, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(4, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(8, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(11, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(2, redValue, greenValue, blueValue);
    pixels.setPixelColor(3, redValue, greenValue, blueValue);
    pixels.setPixelColor(9, redValue, greenValue, blueValue);
    pixels.setPixelColor(10, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 8)
  {
    pixels.clear();

    pixels.setPixelColor(2, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(3, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(9, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(10, redValue / 2, greenValue / 2, blueValue / 2);
  }
  else if (effectIndex == 10)
  {
    pixels.setPixelColor(2, redValue, greenValue, blueValue);
    pixels.setPixelColor(3, redValue, greenValue, blueValue);
    pixels.setPixelColor(9, redValue, greenValue, blueValue);
    pixels.setPixelColor(10, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 12)
  {
    pixels.setPixelColor(2, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(3, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(9, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(10, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(1, redValue, greenValue, blueValue);
    pixels.setPixelColor(4, redValue, greenValue, blueValue);
    pixels.setPixelColor(8, redValue, greenValue, blueValue);
    pixels.setPixelColor(11, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 14)
  {
    pixels.clear();

    pixels.setPixelColor(1, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(4, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(8, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(11, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(0, redValue, greenValue, blueValue);
    pixels.setPixelColor(5, redValue, greenValue, blueValue);
    pixels.setPixelColor(7, redValue, greenValue, blueValue);
    pixels.setPixelColor(12, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 16)
  {
    pixels.clear();

    pixels.setPixelColor(0, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(5, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(8, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(12, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(6, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 18)
  {
    pixels.clear();
    pixels.setPixelColor(6, redValue / 2, greenValue / 2, blueValue / 2);
  }
  else if (effectIndex == 20)
  {
    pixels.clear();
  }

  if (effectIndex % 2 == 0)
    pixels.show();
}
void bar()
{
  if (effectIndex == 16)
    return;
  if (effectIndex >= 34)
  {
    effectIndex = 0;
    return;
  }
    

  if (effectIndex % 2 == 0)
  {
    int ind = effectIndex / 2;
    if (ind < 8)
    {
      pixels.clear();

      setBar(ind - 1, halfColor);
      setBar(ind, randomColor);
    }
    else 
    {
      pixels.clear();

      setBar(-ind + 16, halfColor);
      setBar(-ind + 15, randomColor);
    }
      
    pixels.show();
  }
}
void burstIn()
{
  if (effectIndex >= 17)
    effectIndex = 0;

  if (effectIndex == 0)
  {
    pixels.setPixelColor(2, redValue, greenValue, blueValue);
    pixels.setPixelColor(3, redValue, greenValue, blueValue);
    pixels.setPixelColor(9, redValue, greenValue, blueValue);
    pixels.setPixelColor(10, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 2)
  {
    pixels.setPixelColor(2, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(3, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(9, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(10, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(1, redValue, greenValue, blueValue);
    pixels.setPixelColor(4, redValue, greenValue, blueValue);
    pixels.setPixelColor(8, redValue, greenValue, blueValue);
    pixels.setPixelColor(11, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 4)
  {
    pixels.clear();

    pixels.setPixelColor(1, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(4, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(8, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(11, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(0, redValue, greenValue, blueValue);
    pixels.setPixelColor(5, redValue, greenValue, blueValue);
    pixels.setPixelColor(7, redValue, greenValue, blueValue);
    pixels.setPixelColor(12, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 6)
  {
    pixels.clear();

    pixels.setPixelColor(0, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(5, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(7, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(12, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(6, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 8)
  {
    pixels.clear();
    pixels.setPixelColor(6, redValue / 2, greenValue / 2, blueValue / 2);
  }
  else if (effectIndex == 10)
  {
    pixels.clear();
  }
  
  if (effectIndex % 2 == 0)
    pixels.show();
}
void burstOut()
{
  if (effectIndex >= 17)
    effectIndex = 0;

  if (effectIndex == 0)
  {
    pixels.setPixelColor(6, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 2)
  {
    pixels.setPixelColor(6, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(0, redValue, greenValue, blueValue);
    pixels.setPixelColor(5, redValue, greenValue, blueValue);
    pixels.setPixelColor(7, redValue, greenValue, blueValue);
    pixels.setPixelColor(12, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 4)
  {
    pixels.clear();

    pixels.setPixelColor(0, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(5, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(7, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(12, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(1, redValue, greenValue, blueValue);
    pixels.setPixelColor(4, redValue, greenValue, blueValue);
    pixels.setPixelColor(8, redValue, greenValue, blueValue);
    pixels.setPixelColor(11, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 6)
  {
    pixels.clear();

    pixels.setPixelColor(1, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(4, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(8, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(11, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(2, redValue, greenValue, blueValue);
    pixels.setPixelColor(3, redValue, greenValue, blueValue);
    pixels.setPixelColor(9, redValue, greenValue, blueValue);
    pixels.setPixelColor(10, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 8)
  {
    pixels.clear();

    pixels.setPixelColor(2, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(3, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(9, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(10, redValue / 2, greenValue / 2, blueValue / 2);
  }
  else if (effectIndex == 10)
  {
    pixels.clear();
  }
  
  if (effectIndex % 2 == 0)
    pixels.show();
}

void infinitycircle()
{
  if (effectIndex >= 14)
    effectIndex = 0;

  pixels.setPixelColor(infinityeffect[effectIndex], redValue, greenValue, blueValue);
  pixels.setPixelColor(infinityeffect[(effectIndex + 13) % 14], redValue * 2 / 3, greenValue * 2 / 3, blueValue * 2 / 3);
  pixels.setPixelColor(infinityeffect[(effectIndex + 12) % 14], redValue / 3, greenValue / 3, blueValue / 3);
  pixels.setPixelColor(infinityeffect[(effectIndex + 11) % 14], 0, 0, 0);

  pixels.show();
}
void infinitycirclefill()
{
  if (effectIndex >= 28)
    effectIndex = 0;

  if (effectIndex < 14)
    pixels.setPixelColor(infinityeffect[effectIndex], redValue, greenValue, blueValue);
  else
    pixels.setPixelColor(infinityeffect[effectIndex - 14], 0, 0, 0);

  pixels.show();
}
void chaser()
{
  if (effectIndex >= 14)
    effectIndex = 0;

  pixels.setPixelColor(chasereffect[effectIndex], redValue, greenValue, blueValue);
  pixels.setPixelColor(chasereffect[(effectIndex + 13) % 14], redValue * 2 / 3, greenValue * 2 / 3, blueValue * 2 / 3);
  pixels.setPixelColor(chasereffect[(effectIndex + 12) % 14], redValue / 3, greenValue / 3, blueValue / 3);
  pixels.setPixelColor(chasereffect[(effectIndex + 11) % 14], 0, 0, 0);

  pixels.show();
}
void chaserfill()
{
  if (effectIndex >= 28)
    effectIndex = 0;

  if (effectIndex < 14)
    pixels.setPixelColor(chasereffect[effectIndex], redValue, greenValue, blueValue);
  else
    pixels.setPixelColor(chasereffect[effectIndex - 14], 0, 0, 0);

  pixels.show();
}
void circles()
{
  if (effectIndex >= 7)
    effectIndex = 0;

  pixels.setPixelColor(circleseffect[effectIndex], redValue, greenValue, blueValue);
  pixels.setPixelColor(circleseffect[(effectIndex + 6) % 7], redValue / 2, greenValue / 2, blueValue / 2);
  pixels.setPixelColor(circleseffect[(effectIndex + 5) % 7], 0, 0, 0);

  pixels.setPixelColor(circleseffect[effectIndex + 7], redValue, greenValue, blueValue);
  pixels.setPixelColor(circleseffect[(effectIndex + 6) % 7 + 7], redValue / 2, greenValue / 2, blueValue / 2);
  pixels.setPixelColor(circleseffect[(effectIndex + 5) % 7 + 7], 0, 0, 0);

  pixels.show();
}
void circlesfill()
{
  if (effectIndex >= 14)
    effectIndex = 0;

  if (effectIndex < 7)
  {
    pixels.setPixelColor(circleseffect[effectIndex], redValue, greenValue, blueValue);
    pixels.setPixelColor(circleseffect[effectIndex + 7], redValue, greenValue, blueValue);
  }
  else
  {
    pixels.setPixelColor(circleseffect[effectIndex - 7], 0, 0, 0);
    pixels.setPixelColor(circleseffect[effectIndex], 0, 0, 0);
  }

  pixels.show();
}
void symmetry()
{
  if (effectIndex >= 7)
    effectIndex = 0;

  pixels.setPixelColor(symmetryeffect[effectIndex], redValue, greenValue, blueValue);
  pixels.setPixelColor(symmetryeffect[(effectIndex + 6) % 7], redValue / 2, greenValue / 2, blueValue / 2);
  pixels.setPixelColor(symmetryeffect[(effectIndex + 5) % 7], 0, 0, 0);

  pixels.setPixelColor(symmetryeffect[effectIndex + 7], redValue, greenValue, blueValue);
  pixels.setPixelColor(symmetryeffect[(effectIndex + 6) % 7 + 7], redValue / 2, greenValue / 2, blueValue / 2);
  pixels.setPixelColor(symmetryeffect[(effectIndex + 5) % 7 + 7], 0, 0, 0);

  pixels.show();
}
void symmetryfill()
{
  if (effectIndex >= 14)
    effectIndex = 0;

  if (effectIndex < 7)
  {
    pixels.setPixelColor(symmetryeffect[effectIndex], redValue, greenValue, blueValue);
    pixels.setPixelColor(symmetryeffect[effectIndex + 7], redValue, greenValue, blueValue);
  }
  else
  {
    pixels.setPixelColor(symmetryeffect[effectIndex - 7], 0, 0, 0);
    pixels.setPixelColor(symmetryeffect[effectIndex], 0, 0, 0);
  }

  pixels.show();
}

void starfield()
{
  if (effectIndex >= 10)
    effectIndex = 0;

  if (effectIndex == 0)
  {
    pixels.clear();
    lastPixel = random(13);
    pixels.setPixelColor(lastPixel, redValue * 2 / 3, greenValue * 2 / 3, blueValue * 2 / 3);
  }
  else if (effectIndex == 2)
    pixels.setPixelColor(lastPixel, redValue, greenValue, blueValue);
  else if (effectIndex == 4)
    pixels.setPixelColor(lastPixel, redValue * 2 / 3, greenValue * 2 / 3, blueValue * 2 / 3);
  else if (effectIndex == 6)
    pixels.setPixelColor(lastPixel, redValue / 3, greenValue / 3, blueValue / 3);
  else if (effectIndex == 8)
    pixels.setPixelColor(lastPixel, 0, 0, 0);

  if (effectIndex % 2 == 0)
    pixels.show();
}
void rainbow()
{
  effectIndex = 0;

  if (firstPixelHue >= 5*65536)
    firstPixelHue = 0;
    
  pixels.rainbow(firstPixelHue);
  pixels.show();

  firstPixelHue += 512;
}
void filler()
{
  if (effectIndex == 7)
  {
    if (!isRandomPhase)
      randomColor = pixels.Color(getShift(redValue), getShift(greenValue), getShift(blueValue));
    else
      randomColor = pixels.gamma32(pixels.Color(getShift(randomRed), getShift(randomGreen), getShift(randomBlue)));
    return;
  }
  else if (effectIndex >= 15)
  {
    isRandomPhase = !isRandomPhase;

    randomRed = random(256);
    randomGreen = random(256);
    randomBlue = random(256);

    if (!isRandomPhase)
      randomColor = pixels.Color(redValue, greenValue, blueValue);
    else
      randomColor = pixels.gamma32(pixels.Color(randomRed, randomGreen, randomBlue));
    effectIndex = -1;
    return;
  }
  
  if (effectIndex < 7)
    setBar(effectIndex, randomColor);
  else
    setBar(-effectIndex + 14, randomColor);
    
  pixels.show();
}