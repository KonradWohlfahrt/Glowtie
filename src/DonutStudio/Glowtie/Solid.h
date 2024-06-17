#ifndef SOLID_h
#define SOLID_h

class Solid : public LedEffect
{
  public:
    Solid() : LedEffect(75, true) { }

    void onStart()
    {
      pixels.fill(pixels.Color(redValue, greenValue, blueValue));
      pixels.show();
    }
    void onUpdate(int& i) { }

    private:
};

class SolidRainbow : public LedEffect
{
  public:
    SolidRainbow() : LedEffect(75, true) { }

    void onStart()
    {
      pixels.rainbow(0);
      pixels.show();
    }
    void onUpdate(int& i) { }

    private:
};

#endif