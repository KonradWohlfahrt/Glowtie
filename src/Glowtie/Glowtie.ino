#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#include "webserver.h"

#define PIN            12
#define NUMPIXELS      13
#define BATTCHECKTIME 10000
#define EFFECTREFRESHTIME 75
#define LOWBATTERYVALUE 3050

const char *ssid = "Glowtie";
const char *password = "pleaseletmein";

ADC_MODE(ADC_VCC);

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

GlowtieMode mode = SOLID;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
ESP8266WebServer server(80);

unsigned long lastBatteryCheck = 0;
unsigned long lastEffectUpdate = 0;

byte redValue = 127;
byte greenValue = 127;
byte blueValue = 127;

int effectIndex = 0;

byte infinityeffect[] = { 0, 1, 2, 3, 4, 5, 6, 12, 11, 10, 9, 8, 7, 6 };
byte chasereffect[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 6 };
byte circleseffect[] = { 6, 0, 1, 2, 3, 4, 5, 6, 12, 11, 10, 9, 8, 7 };
byte symmetryeffect[] = { 6, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };

byte lastPixel = 0;
long firstPixelHue = 0;

void setup() 
{
  pixels.begin();
  pixels.setBrightness(20);

  lowBattery();


  ///* if you don't want to use the eeprom, remove this
  EEPROM.begin(4);
  delay(500);

  mode = (GlowtieMode)EEPROM.read(0);
  redValue = EEPROM.read(1);
  greenValue = EEPROM.read(2);
  blueValue = EEPROM.read(3);
  //*/


  if (WiFi.softAP(ssid, password)) 
    successAnim();
  else 
    errorAnim();

  server.on("/", handleRoot);
  server.on("/red", handleGetRed);
  server.on("/green", handleGetGreen);
  server.on("/blue", handleGetBlue);
  server.on("/vcc", handleGetVCC);
  server.begin();

  delay(100);

  updatePixels();
}
void loop() 
{
  server.handleClient();

  // check battery
  if (millis() - lastBatteryCheck >= BATTCHECKTIME) 
  {
    if (hasLowBattery())
    {
      WiFi.disconnect();
      errorAnim();
      lowBattery();
      while(true) delay(5);
    }

    lastBatteryCheck = millis();
  }

  // refresh the dynamic effects
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


/* --- HTML --- */

void handleGetRed() { server.send(200, "text/plane", String(redValue)); }
void handleGetGreen() { server.send(200, "text/plane", String(greenValue)); }
void handleGetBlue() { server.send(200, "text/plane", String(blueValue)); }
void handleGetVCC() { server.send(200, "text/plane", "ADC: " + String(ESP.getVcc())); }
void handleRoot() 
{
  if (server.args() > 0) 
  {
    redValue = (byte)server.arg("red").toInt();
    greenValue = (byte)server.arg("green").toInt();
    blueValue = (byte)server.arg("blue").toInt();
    mode = (GlowtieMode)server.arg("mode").toInt();

    
    ///* if you don't want to use the eeprom, remove this
    EEPROM.write(0, (byte)mode);
    EEPROM.write(1, redValue);
    EEPROM.write(2, greenValue);
    EEPROM.write(3, blueValue);
    EEPROM.commit();
    delay(500);
    //*/
    

    updatePixels();
  }

  server.send(200, "text/html", index_html);
}


/* --- UTILITY --- */

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
  }
}
bool hasLowBattery()
{
  // get the average supply voltage from 6 readings
  int avg = 0;
  for (byte b = 0; b < 6; b++)
  {
    avg += ESP.getVcc();
    delay(25);
  }
  avg /= 6;

  return avg < LOWBATTERYVALUE;
}
void successAnim()
{
  for (int i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, 0, 255, 0);
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
void errorAnim()
{
  for (int i = 0; i < 10; i++)
  {
    if (i % 2 == 0)
      pixels.fill(pixels.Color(255, 0, 0));
    else
      pixels.clear();
    
    pixels.show();
    delay(EFFECTREFRESHTIME);
  }
}
byte getAverage(byte a, byte b)
{
  return (byte)(((int)a + (int)b) / 2);
}


/* --- STATIC EFFECTS --- */

void lowBattery() 
{
  pixels.clear();
  pixels.setPixelColor(1, 50, 0, 0);
  pixels.setPixelColor(4, 50, 0, 0);
  pixels.setPixelColor(6, 50, 0, 0);
  pixels.setPixelColor(8, 50, 0, 0);
  pixels.setPixelColor(11, 50, 0, 0);
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
  if (effectIndex >= 27)
    effectIndex = 0;

  if (effectIndex == 0)
  {
    pixels.clear();

    pixels.setPixelColor(2, redValue, greenValue, blueValue);
    pixels.setPixelColor(3, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 2)
  {
    pixels.setPixelColor(2, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(3, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(1, redValue, greenValue, blueValue);
    pixels.setPixelColor(4, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 4)
  {
    pixels.clear();

    pixels.setPixelColor(1, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(4, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(0, redValue, greenValue, blueValue);
    pixels.setPixelColor(5, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 6)
  {
    pixels.clear();

    pixels.setPixelColor(0, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(5, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(6, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 8)
  {
    pixels.clear();

    pixels.setPixelColor(6, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(7, redValue, greenValue, blueValue);
    pixels.setPixelColor(12, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 10)
  {
    pixels.clear();

    pixels.setPixelColor(7, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(12, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(8, redValue, greenValue, blueValue);
    pixels.setPixelColor(11, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 12)
  {
    pixels.clear();

    pixels.setPixelColor(8, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(11, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(9, redValue, greenValue, blueValue);
    pixels.setPixelColor(10, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 14)
  {
    pixels.clear();

    pixels.setPixelColor(9, redValue, greenValue, blueValue);
    pixels.setPixelColor(10, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 16)
  {
    pixels.setPixelColor(9, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(10, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(8, redValue, greenValue, blueValue);
    pixels.setPixelColor(11, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 18)
  {
    pixels.clear();

    pixels.setPixelColor(8, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(11, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(7, redValue, greenValue, blueValue);
    pixels.setPixelColor(12, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 20)
  {
    pixels.clear();

    pixels.setPixelColor(7, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(12, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(6, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 22)
  {
    pixels.clear();

    pixels.setPixelColor(6, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(5, redValue, greenValue, blueValue);
    pixels.setPixelColor(0, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 24)
  {
    pixels.clear();

    pixels.setPixelColor(5, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(0, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(4, redValue, greenValue, blueValue);
    pixels.setPixelColor(1, redValue, greenValue, blueValue);
  }
  else if (effectIndex == 26)
  {
    pixels.clear();

    pixels.setPixelColor(4, redValue / 2, greenValue / 2, blueValue / 2);
    pixels.setPixelColor(1, redValue / 2, greenValue / 2, blueValue / 2);

    pixels.setPixelColor(3, redValue, greenValue, blueValue);
    pixels.setPixelColor(2, redValue, greenValue, blueValue);
  }

  if (effectIndex % 2 == 0)
    pixels.show();
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
  if (effectIndex >= 21)
    effectIndex = 0;

  if (effectIndex != 0 && effectIndex != 10)
    return;

  byte r1 = random(0, 256);
  byte g1 = random(0, 256);
  byte b1 = random(0, 256);

  byte r2 = getAverage(redValue, r1);
  byte g2 = getAverage(redValue, g1);
  byte b2 = getAverage(redValue, b1);
  
  pixels.fill(pixels.Color(redValue, greenValue, blueValue), 8, 4);
  pixels.fill(pixels.Color(redValue, greenValue, blueValue), 1, 4);

  pixels.setPixelColor(0, r2, g2, b2);
  pixels.setPixelColor(5, r2, g2, b2);
  pixels.setPixelColor(7, r2, g2, b2);
  pixels.setPixelColor(12, r2, g2, b2);

  pixels.setPixelColor(6, r1, g1, b1);
    
  pixels.show();
}