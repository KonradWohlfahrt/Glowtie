#ifndef SYMMETRY_h
#define SYMMETRY_h

const byte symmetryeffect[] = { 6, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };

class Symmetry : public LedEffect
{
  public:
    Symmetry() : LedEffect() { }

    void onStart() { }
    void onUpdate(int& i) 
    { 
      if (i >= 7)
        i = 0;

      pixels.setPixelColor(symmetryeffect[i], redValue, greenValue, blueValue);
      pixels.setPixelColor(symmetryeffect[(i + 6) % 7], redValue / 2, greenValue / 2, blueValue / 2);
      pixels.setPixelColor(symmetryeffect[(i + 5) % 7], 0, 0, 0);

      pixels.setPixelColor(symmetryeffect[i + 7], redValue, greenValue, blueValue);
      pixels.setPixelColor(symmetryeffect[(i + 6) % 7 + 7], redValue / 2, greenValue / 2, blueValue / 2);
      pixels.setPixelColor(symmetryeffect[(i + 5) % 7 + 7], 0, 0, 0);

      pixels.show();
    }

    private:
};

class SymmetryFill : public LedEffect
{
  public:
    SymmetryFill() : LedEffect() { }

    void onStart() { }
    void onUpdate(int& i) 
    { 
      if (i >= 14)
        i = 0;

      if (i < 7)
      {
        pixels.setPixelColor(symmetryeffect[i], redValue, greenValue, blueValue);
        pixels.setPixelColor(symmetryeffect[i + 7], redValue, greenValue, blueValue);
      }
      else
      {
        pixels.setPixelColor(symmetryeffect[i - 7], 0, 0, 0);
        pixels.setPixelColor(symmetryeffect[i], 0, 0, 0);
      }

      pixels.show();
    }

    private:
};

#endif