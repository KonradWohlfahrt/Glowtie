#ifndef STARFIELD_h
#define STARFIELD_h

class Starfield : public LedEffect
{
  public:
    Starfield() : LedEffect(150) { }

    void onStart() { }
    void onUpdate(int& i) 
    { 
      if (i >= 5)
        i = 0;

      if (i == 0)
      {
        pixels.clear();
        _lastPixel = random(NUMPIXELS);
        pixels.setPixelColor(_lastPixel, redValue * 2 / 3, greenValue * 2 / 3, blueValue * 2 / 3);
      }
      else if (i == 1)
        pixels.setPixelColor(_lastPixel, redValue, greenValue, blueValue);
      else if (i == 2)
        pixels.setPixelColor(_lastPixel, redValue * 2 / 3, greenValue * 2 / 3, blueValue * 2 / 3);
      else if (i == 3)
        pixels.setPixelColor(_lastPixel, redValue / 3, greenValue / 3, blueValue / 3);
      else if (i == 4)
        pixels.setPixelColor(_lastPixel, 0, 0, 0);

      pixels.show();
    }

    private:
      byte _lastPixel = 0;
};

#endif