#ifndef CIRCLES_h
#define CIRCLES_h

const byte circleseffect[] = { 6, 0, 1, 2, 3, 4, 5, 6, 12, 11, 10, 9, 8, 7 };

class Circles : public LedEffect
{
  public:
    Circles() : LedEffect() { }

    void onStart() { }
    void onUpdate(int& i) 
    { 
      if (i >= 7)
        i = 0;

      pixels.setPixelColor(circleseffect[i], redValue, greenValue, blueValue);
      pixels.setPixelColor(circleseffect[(i + 6) % 7], redValue / 2, greenValue / 2, blueValue / 2);
      pixels.setPixelColor(circleseffect[(i + 5) % 7], 0, 0, 0);

      pixels.setPixelColor(circleseffect[i + 7], redValue, greenValue, blueValue);
      pixels.setPixelColor(circleseffect[(i + 6) % 7 + 7], redValue / 2, greenValue / 2, blueValue / 2);
      pixels.setPixelColor(circleseffect[(i + 5) % 7 + 7], 0, 0, 0);

      pixels.show();
    }

    private:
};

class CirclesFill : public LedEffect
{
  public:
    CirclesFill() : LedEffect() { }

    void onStart() { }
    void onUpdate(int& i) 
    { 
      if (i >= 14)
        i = 0;

      if (i < 7)
      {
        pixels.setPixelColor(circleseffect[i], redValue, greenValue, blueValue);
        pixels.setPixelColor(circleseffect[i + 7], redValue, greenValue, blueValue);
      }
      else
      {
        pixels.setPixelColor(circleseffect[i - 7], 0, 0, 0);
        pixels.setPixelColor(circleseffect[i], 0, 0, 0);
      }

      pixels.show();
    }

    private:
};

#endif