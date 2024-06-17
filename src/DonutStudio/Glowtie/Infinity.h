#ifndef INFINITY_h
#define INFINITY_h

const byte infinityeffect[] = { 0, 1, 2, 3, 4, 5, 6, 12, 11, 10, 9, 8, 7, 6 };

class Infinity : public LedEffect
{
  public:
    Infinity() : LedEffect() { }

    void onStart() { }
    void onUpdate(int& i) 
    { 
      if (i >= 14)
        i = 0;

      pixels.setPixelColor(infinityeffect[i], redValue, greenValue, blueValue);
      pixels.setPixelColor(infinityeffect[(i + 13) % 14], redValue * 2 / 3, greenValue * 2 / 3, blueValue * 2 / 3);
      pixels.setPixelColor(infinityeffect[(i + 12) % 14], redValue / 3, greenValue / 3, blueValue / 3);
      pixels.setPixelColor(infinityeffect[(i + 11) % 14], 0, 0, 0);

      pixels.show();
    }

    private:
};

class InfinityFill : public LedEffect
{
  public:
    InfinityFill() : LedEffect() { }

    void onStart() { }
    void onUpdate(int& i) 
    { 
      if (i >= 28)
        i = 0;

      if (i < 14)
        pixels.setPixelColor(infinityeffect[i], redValue, greenValue, blueValue);
      else
        pixels.setPixelColor(infinityeffect[i - 14], 0, 0, 0);

      pixels.show();
    }

    private:
};

#endif