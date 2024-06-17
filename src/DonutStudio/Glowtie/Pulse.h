#ifndef PULSE_h
#define PULSE_h

class Pulse : public LedEffect
{
  public:
    Pulse() : LedEffect(150) { }

    void onStart() { }
    void onUpdate(int& i) 
    { 
      if (i >= 13)
        i = 0;

      if (i == 0)
      {
        pixels.clear();
        pixels.setPixelColor(6, redValue, greenValue, blueValue);
      }
      else if (i == 1)
      {
        pixels.setPixelColor(6, redValue / 2, greenValue / 2, blueValue / 2);

        pixels.setPixelColor(0, redValue, greenValue, blueValue);
        pixels.setPixelColor(5, redValue, greenValue, blueValue);
        pixels.setPixelColor(7, redValue, greenValue, blueValue);
        pixels.setPixelColor(12, redValue, greenValue, blueValue);
      }
      else if (i == 2)
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
      else if (i == 3)
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
      else if (i == 4)
      {
        pixels.clear();

        pixels.setPixelColor(2, redValue / 2, greenValue / 2, blueValue / 2);
        pixels.setPixelColor(3, redValue / 2, greenValue / 2, blueValue / 2);
        pixels.setPixelColor(9, redValue / 2, greenValue / 2, blueValue / 2);
        pixels.setPixelColor(10, redValue / 2, greenValue / 2, blueValue / 2);
      }
      else if (i == 5)
      {
        pixels.setPixelColor(2, redValue, greenValue, blueValue);
        pixels.setPixelColor(3, redValue, greenValue, blueValue);
        pixels.setPixelColor(9, redValue, greenValue, blueValue);
        pixels.setPixelColor(10, redValue, greenValue, blueValue);
      }
      else if (i == 6)
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
      else if (i == 7)
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
      else if (i == 8)
      {
        pixels.clear();

        pixels.setPixelColor(0, redValue / 2, greenValue / 2, blueValue / 2);
        pixels.setPixelColor(5, redValue / 2, greenValue / 2, blueValue / 2);
        pixels.setPixelColor(8, redValue / 2, greenValue / 2, blueValue / 2);
        pixels.setPixelColor(12, redValue / 2, greenValue / 2, blueValue / 2);

        pixels.setPixelColor(6, redValue, greenValue, blueValue);
      }
      else if (i == 9)
      {
        pixels.clear();
        pixels.setPixelColor(6, redValue / 2, greenValue / 2, blueValue / 2);
      }
      else if (i == 10)
        pixels.clear();

      pixels.show();
    }

    private:
};

#endif