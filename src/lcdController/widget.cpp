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

#include <LiquidCrystal.h>
#include <Arduino.h>
#include "widget.h"

extern Stream* saberSerial;

const char* Widget::prefixMatch(const char* buffer, const char* cmd) {
  if (!cmd) return false;
  int len = strlen(cmd);
  if (memcmp(buffer, cmd, len) == 0 && buffer[len] == ' ') {
    return buffer + len + 1;
  }
  return 0;
}

void TextWidget::render(LiquidCrystal& lcd) {
	lcd.setCursor(x,y);
	lcd.print(text);
}

bool TextNumWidget::canFocus() const {
  return true;
}

void TextNumWidget::getFocus(int* fx, int* fy) const {
    *fx = x + (text ? strlen(text) : 0);
    *fy = y;
}

void TextNumWidget::render(LiquidCrystal& lcd) {
  lcd.setCursor(x,y);
  if (text) 
    lcd.print(text);

  if (type == TYPE_INT) {
    int w = 1;
    if      (iVal >= 1000) w = 4;
    else if (iVal >= 100)  w = 3;
    else if (iVal >= 10)   w = 2;
    int dw = width - w;

    for(int i=0; i<dw; ++i)
      lcd.print(' ');
    
    lcd.print(iVal);    
  }
  else if (type == TYPE_HEX) {
    lcd.print(int(iVal / 16), HEX);
    lcd.print(int(iVal & 15), HEX);
  }
  else {
    lcd.print(fVal, width-2);
  }
}


bool TextNumWidget::handleInput(const char* input) {
  const char* q = prefixMatch(input, idStr);
  if (q) {
    if (type == TYPE_INT) {
      iVal = atoi(q);
    }
    else if (type == TYPE_HEX) {
      // Set externally, for now.
      // iVal = hexToDec(q[0]) * 16 + hexToDec(q[1]);
    }
    else if (type == TYPE_FLOAT) {
      fVal = atof(q);
    }
    return true;
  }
  return false;
}


void TextNumWidget::valueChange(int bias)
{
    if (type == TYPE_INT) {
    if (bias == 2) bias = 10;
    if (bias == -2) bias = -10;
    iVal = iVal + bias;
    if (idStr) {
      saberSerial->print(idStr);
      saberSerial->print(" ");
      saberSerial->println(iVal);
    }
  }
  else if (type == TYPE_HEX) {
    if (bias == 2) bias = 16;
    if (bias == -2) bias = -16;
    iVal += bias;
    iVal = constrain(iVal, 0, 255);    
  }
  else if (type == TYPE_FLOAT) {
    fVal = fVal + 0.05f * float(bias);
    if (idStr) {
      saberSerial->print(idStr);
      saberSerial->print(" ");
      saberSerial->println(fVal);
    }
  }
}


void PercentX3Widget::render(LiquidCrystal& lcd) {
  lcd.setCursor(x, y);
  lcd.print(text);
  for(int i=0; i<3; ++i) {
    if (value[i] < 100) lcd.print(' ');
    if (value[i] < 10) lcd.print(' ');
    lcd.print(value[i]);
    if (i < 2)
      lcd.print(' ');
  }
}


bool PercentX3Widget::handleInput(const char* input) {
  const char* q = prefixMatch(input, idStr);
  if (q) {
    --q;  // back up so we find space
    for(int i=0; i<3; ++i) {
      q = strstr(q, " ");
      if (q) {
        value[i] = atoi(q);
        ++q;
      }
    }
    return true;
  }
  return false;
}

