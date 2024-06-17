#ifndef CHASER_h
#define CHASER_h

const byte chasereffect[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 6 };

class Chaser : public LedEffect
{
  public:
    Chaser() : LedEffect() { }

    void onStart() { }
    void onUpdate(int& i) 
    { 
      if (i >= 14)
        i = 0;

      pixels.setPixelColor(chasereffect[i], redValue, greenValue, blueValue);
      pixels.setPixelColor(chasereffect[(i + 13) % 14], redValue * 2 / 3, greenValue * 2 / 3, blueValue * 2 / 3);
      pixels.setPixelColor(chasereffect[(i + 12) % 14], redValue / 3, greenValue / 3, blueValue / 3);
      pixels.setPixelColor(chasereffect[(i + 11) % 14], 0, 0, 0);

      pixels.show();
    }

    private:
};

class ChaserFill : public LedEffect
{
  public:
    ChaserFill() : LedEffect() { }

    void onStart() { }
    void onUpdate(int& i) 
    { 
      if (i >= 28)
        i = 0;

      if (i < 14)
        pixels.setPixelColor(chasereffect[i], redValue, greenValue, blueValue);
      else
        pixels.setPixelColor(chasereffect[i - 14], 0, 0, 0);

      pixels.show();
    }

    private:
};

#endif