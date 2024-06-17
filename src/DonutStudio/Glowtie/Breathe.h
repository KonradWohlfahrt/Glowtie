#ifndef BREATHE_h
#define BREATHE_h

class Breathe : public LedEffect
{
  public:
    Breathe() : LedEffect(150) { }

    void onStart() { }
    void onUpdate(int& i) 
    {
      if (i >= 11)
        i = 0;

      if (i == 0)
        pixels.fill(pixels.Color(redValue / 3, greenValue / 3, blueValue / 3));
      else if (i == 1)
        pixels.fill(pixels.Color(redValue * 2 / 3, greenValue * 2 / 3, blueValue * 2 / 3));
      else if (i == 2)
        pixels.fill(pixels.Color(redValue, greenValue, blueValue));
      else if (i == 5)
        pixels.fill(pixels.Color(redValue * 2 / 3, greenValue * 2 / 3, blueValue * 2 / 3));
      else if (i == 6)
        pixels.fill(pixels.Color(redValue / 3, greenValue / 3, blueValue / 3));
      else if (i == 7)
        pixels.clear();
      pixels.show();
    }

    private:
};

#endif