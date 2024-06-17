#ifndef RAINBOW_h
#define RAINBOW_h

class Rainbow : public LedEffect
{
  public:
    Rainbow() : LedEffect(25) { }

    void onStart() { }
    void onUpdate(int& i) 
    {
      if (_firstPixelHue >= 5*65536)
        _firstPixelHue = 0;
      pixels.rainbow(_firstPixelHue);
      pixels.show();
      _firstPixelHue += 256;
    }

    private:
      long _firstPixelHue = 0;
};

class RainbowFill : public LedEffect
{
  public:
    RainbowFill() : LedEffect(25) { }

    void onStart() { }
    void onUpdate(int& i) 
    {
      pixels.fill(pixels.ColorHSV(_hue));
      pixels.show();
      _hue += 256;
    }

    private:
      int _hue = 0;
};

#endif