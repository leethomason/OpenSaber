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

#ifndef BLADE_INCLUDED
#define BLADE_INCLUDED

#include <stdint.h>
#include "pins.h"

class Blade
{
  public:
    Blade();
    static Blade& blade() {
      return *instance;
    }

    void setVoltage(int milliVolts);
    int voltage() const {
      return vbat;
    }

    bool setInterp(uint32_t delta, uint32_t effectTime, const uint8_t* startColor, const uint8_t* endColor);
    void setRGB(const uint8_t* input);

    // power consumed by the main blade color, in milliAmps
    int32_t power() const;
    // power utilization, from 0-100 for a channel
    int util(int i) const   {
      return (f1000[i] + 5) / 10;
    }
    // The actual PWM setting, per channel.
    uint8_t pwmVal(int i) const {
      return pwm[i];
    }

    void getColor(uint8_t* c) {
      for(int i=0; i<NCHANNELS; ++i) {
        c[i] = color[i];
      }
    }

  private:
    uint8_t lerpU8(uint8_t a, uint8_t b, uint8_t t) const;

    static const int8_t pinRGB[NCHANNELS];
    static Blade* instance;

    uint8_t color[NCHANNELS];
    uint8_t pwm[NCHANNELS];
    int32_t vbat;
    int32_t f1000[NCHANNELS];
};

#endif // BLADE_INCLUDED
