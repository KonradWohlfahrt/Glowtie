#ifndef BAR_h
#define BAR_h

class Bar : public LedEffect
{
  public:
    Bar() : LedEffect(150) { }

    void onStart() 
    { 
      _randomColor = pixels.Color(redValue, greenValue, blueValue);
      _halfColor = pixels.Color(redValue / 2, greenValue / 2, blueValue / 2);
    }
    void onUpdate(int& i) 
    { 
      if (i == 8)
        return;
      if (i >= 17)
      {
        i = 0;
        return;
      }
        
      int ind = i / 2;
      if (ind < 8)
      {
        pixels.clear();

        setBar(ind - 1, _halfColor);
        setBar(ind, _randomColor);
      }
      else 
      {
        pixels.clear();

        setBar(-ind + 16, _halfColor);
        setBar(-ind + 15, _randomColor);
      }
        
      pixels.show();
    }

    private:
      uint32_t _randomColor;
      uint32_t _halfColor;
};

#endif