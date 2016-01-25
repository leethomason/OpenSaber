/*
Copyright (c) 2016 Lee Thomason, Grinning Lizard Software

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.
*/

#ifndef LCD_CONTROLLER_WIDGET
#define LCD_CONTROLLER_WIDGET

class LiquidCrystal;

const static int NCHANNELS = 3;

class Widget {
public:
  enum {
    TYPE_INT,
    TYPE_HEX,
    TYPE_FLOAT
  };

	Widget(int _screen, int _x, int _y) : screen(_screen), x(_x), y(_y) {}  
  
	uint8_t screen;
	uint8_t x, y;

	virtual bool handleInput(const char* input)  { return false; }
	virtual void render(LiquidCrystal& lcd)	     {}
  virtual bool canFocus() const                { return false; }
  virtual void getFocus(int* x, int* y) const  { *x = 0; *y = 0; }
  virtual void valueChange(int bias)           {};

protected:
  const char* prefixMatch(const char* buffer, const char* cmd);
};


class TextWidget : public Widget {
public:
	TextWidget(int _screen, int _x, int _y, const char* _text) : Widget(_screen, _x, _y), text(_text)	{}

	virtual void render(LiquidCrystal& lcd);

private:
	const char* text;
};


class TextNumWidget : public Widget {
public:
  TextNumWidget(int _screen, int _x, int _y, const char* _text, int _width, const char* _idStr, int _type) 
    : Widget(_screen, _x, _y), 
      width(_width),
      type(_type),
      text(_text),
      idStr(_idStr)
      { iVal = 0; }

  virtual void render(LiquidCrystal& lcd);
  virtual bool handleInput(const char* input);
  virtual void getFocus(int* x, int* y) const;
  virtual bool canFocus() const;
  virtual void valueChange(int bias);

  void  setIVal(int v)  { iVal = v; }
  int   getIVal() const { return iVal; }
  float getFVal() const { return fVal; }

private:
  uint8_t   width;  
  uint8_t   type;
  union {
    int32_t iVal;
    float   fVal;
  };
  const char* text;
  const char* idStr;
};


class PercentX3Widget : public Widget {
public:
    PercentX3Widget(int _screen, int _x, int _y, const char* _text, const char* _idStr) 
      : Widget(_screen, _x, _y), 
        text(_text), 
        idStr(_idStr) 
    {
      for(int i=0; i<3; ++i) value[i] = 0;
    }

  virtual void render(LiquidCrystal& lcd);
  virtual bool handleInput(const char* input);

private:
  int value[3];
  const char* text;
  const char* idStr;
};

#endif //  LCD_CONTROLLER_WIDGET

