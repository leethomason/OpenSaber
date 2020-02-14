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
#include "fixed.h"
#include "rgb.h"


void calcGravity2(float ax, float ay, float az, float* g2, float* g2Normal);
/*
    SaberDB: the database that holds the blade colors.
        Data. Just holds colors.

    BladeFlash: Animation code for an on blade.
        Assumes the blade is on; doesn't track the state.

    BladeState: state, whether the blade is on, off, ignite, retract.
        The 'process' method combines the state, color from BladeFlash,
        and writes the current color to the Blade.

    The BladePWM has no state (except it's target color and voltage.) It takes 
        an input color and translates it to an output electrical signal.
        The call to the color or voltage can be at any time, and it will update.
*/
class BladePWM
{
public:
    BladePWM();

    static BladePWM& bladePWM() {
        return *instance;
    }

    void setVoltage(int milliVolts);
    void setRGB(const osbr::RGB& rgb);
    osbr::RGB getRGB() const { return m_color; }

    // power utilization, from 0-100 for a channel
    int32_t util(int i) const   { return m_throttle[i].scale(100); }
    
    // The actual PWM setting, per channel.
    uint8_t pwmVal(int i) const {
        return m_pwm[i];
    }

    static osbr::RGB convertRawToPerceived(const osbr::RGB& raw);

private:
    void setThrottledRGB();

    static const int8_t pinRGB[NCHANNELS];
    static BladePWM* instance;

    osbr::RGB m_color;
    int32_t m_vbat;
    FixedNorm m_throttle[NCHANNELS];  // throttle = f(vbat).
    uint8_t m_pwm[NCHANNELS];         // pwm = f(color, throttle)
};

#endif // BLADE_INCLUDED
