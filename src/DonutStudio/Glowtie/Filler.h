#ifndef FILLER_h
#define FILLER_h

class Filler : public LedEffect
{
  public:
    Filler() : LedEffect() { }

    void onStart()
    {
      _isRandomPhase = false;
      _randomColor = pixels.Color(redValue, greenValue, blueValue);
    }
    void onUpdate(int& i) 
    { 
      if (i == 7)
      {
        if (!_isRandomPhase)
          _randomColor = pixels.Color(shiftByte(redValue), shiftByte(greenValue), shiftByte(blueValue));
        else
          _randomColor = pixels.gamma32(pixels.Color(shiftByte(_randomRed), shiftByte(_randomGreen), shiftByte(_randomBlue)));
        return;
      }
      else if (i >= 15)
      {
        _isRandomPhase = !_isRandomPhase;

        if (!_isRandomPhase)
          _randomColor = pixels.Color(redValue, greenValue, blueValue);
        else
        {
          _randomColor = getRandomColor();
          _randomRed = _randomColor >> 16;
          _randomGreen = _randomColor >> 8;
          _randomBlue = (byte)_randomColor;
        }
          
        i = -1;
        return;
      }
      
      if (i < 7)
        setBar(i, _randomColor);
      else
        setBar(-i + 14, _randomColor);
        
      pixels.show();
    }

    private:
      uint32_t _randomColor;
      byte _randomRed;
      byte _randomGreen;
      byte _randomBlue;
      bool _isRandomPhase = false;
};

#endif