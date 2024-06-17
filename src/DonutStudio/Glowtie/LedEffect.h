#ifndef LEDEFFECT_h
#define LEDEFFECT_h

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
void setBar(byte index, uint32_t color)
{
  if (0 > index || index > 6)
    return;
  pixels.setPixelColor(bareffect[index], color);
  pixels.setPixelColor(bareffect[index + 7], color);
}


void showBatteryPercentage()
{
  for (int i = 0; i < 7; i++)
  {
    setBar(i, green);
    pixels.show();
    delay(EFFECTREFRESHTIME);
  }

  float p = getBatteryPercent();
  int ledAmount = (int)(0.1f * p + 0.5f);
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


void disableLEDs()
{
  pixels.clear();
  pixels.show();
}
void lowBattery() 
{
  pixels.clear();
  pixels.setPixelColor(6, 50, 0, 0);
  pixels.show();
}


byte shiftByte(byte c)
{
  byte shift = c / 2;
  if (c > 127)
    shift = (-(int)c + 255) / 2;

  if (c < 25)
    return c + shift;
  else if (c > 230)
    return c - shift;
  else
    return c + (random(2) == 0 ? shift : -shift);
}
uint32_t getRandomColor() { return pixels.ColorHSV(random(65536)); }
uint32_t getSimilarColor(uint32_t color) { return pixels.gamma32(pixels.Color(shiftByte((byte)(color >> 16)), shiftByte((byte)(color >> 8)), shiftByte((byte)color))); }


class LedEffect 
{
    public:
        LedEffect(int effectSpeed = 75, bool isStatic = false) : _effectSpeed(effectSpeed), _isStatic(isStatic) {}
        virtual void onStart() = 0;
        virtual void onUpdate(int& i) = 0;
        int getSpeed() { return _effectSpeed; }
        bool getIsStatic() { return _isStatic; }

    private:
        int _effectSpeed;
        bool _isStatic;
};

#endif