/*
  Copyright (c) Lee Thomason, Grinning Lizard Software

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

/*
    Blade flash bug:
    x is it voltmeter?
        NO. with volt setting turned off, still see flicker, specially at high-t during retract
    - is it fixed point?        
        Unlikely; bug existed before fixed.
    - is it LERP?
        Unlikely; test okay. Error would be in middle of lerp.
    - is it interupting calls?
        Unlikely; this has been addressed.
        Weirdness in Blade code (setVolts / setRGB coupling) has been addressed. LERP
            is outside.
    - PWM hardware?
        Maybe? but setting different values doesn't seem to matter.
        Is the set atomic?
    - Error in the state?
        Again, don't see anything. The BLADE_FLASH seems to be set correctly.
    x interrupts don't matter

    Does have to have something to do with the interpolation / color change. Don't see in-use
    flashes. Only during ignite / retract. 
*/

#include <Arduino.h>

#include "pins.h"
#include "bladePWM.h"
#include "Grinliz_Util.h"

using namespace osbr;

void calcGravity2(float ax, float ay, float az, float* g2, float* g2Normal)
{
    if (g2) {
        *g2 = ax*ax + ay*ay + az*az;
    }
    if (g2Normal) {
        #if ACCEL_BLADE_DIRECTION == 0
        *g2Normal = ay * ay + az * az;
        #elif ACCEL_BLADE_DIRECTION == 1
        *g2Normal = ax * ax + az * az;
        #elif ACCEL_BLADE_DIRECTION == 2
        *g2Normal = ax * ax + ay * ay;
        #endif
    }
}

const int8_t BladePWM::pinRGB[NCHANNELS] = { PIN_EMITTER_RED, PIN_EMITTER_GREEN, PIN_EMITTER_BLUE };
BladePWM* BladePWM::instance = 0;

BladePWM::BladePWM() 
{
    m_color.set(0);
    
    for (int i = 0; i < NCHANNELS; ++i) {
        pinMode(pinRGB[i], OUTPUT);
        digitalWrite(pinRGB[i], LOW);
        m_pwm[i] = 0;
    }

    m_vbat = NOMINAL_VOLTAGE;
    for (int i = 0; i < NCHANNELS; ++i) {
        m_throttle[i] = 1;
        m_color[i] = 0;
    }
    instance = this;
}


void BladePWM::setRGB(const RGB& rgb)
{
    if (rgb != m_color) {
        m_color = rgb;
        setThrottledRGB();
    }
}

void BladePWM::setThrottledRGB()
{
    for (int i = 0; i < NCHANNELS; ++i) {
        int32_t pwm = m_throttle[i].scale(m_color[i]);

        ASSERT(m_throttle[i] > 0);
        ASSERT(m_throttle[i] <= 1);
        ASSERT(pwm >= 0);
        ASSERT(pwm <= 255);

        m_pwm[i] = glClamp<int32_t>(pwm, 0, 255); // just in case...
        analogWrite(pinRGB[i], m_pwm[i]);
    }
}

void BladePWM::setVoltage(int milliVolts) 
{
    static const int32_t vF[NCHANNELS]   = { RED_VF, GREEN_VF, BLUE_VF };
    static const int32_t amps[NCHANNELS] = { RED_I,  GREEN_I,  BLUE_I };
    static const int32_t res[NCHANNELS]  = { RED_R,  GREEN_R,  BLUE_R };

    if (m_vbat == milliVolts)
        return;

    if (milliVolts < 3000 || milliVolts > 5400) {
        Log.p("BladePWM::setVoltage() vBat=").p(m_vbat).p(" mV=").p(milliVolts).eol();
        ASSERT(false);
    }

    m_vbat = milliVolts;
    for (int i = 0; i < NCHANNELS; ++i) {
        // Unlikely low power scenario.
        if (m_vbat <= vF[i]) {
            m_throttle[i] = 1;
            continue;
        }

        // throttle = I * R / (Vbat - Vf)
        // Denominator x1000 corrects for units (mAmps, mOhms, mVolts)
        m_throttle[i] = FixedNorm(amps[i] * res[i] / 1000, m_vbat - vF[i] );
        if (m_throttle[i] <= 0) {
            Log.p(i).p(": ").p(amps[i]).p(" ").p(res[i]).p(" ").p(m_vbat).p(" ").p(vF[i]).eol();
            ASSERT(false);
        }
        // Want more power than we have as the battery goes down:
        if (m_throttle[i] > 1)
            m_throttle[i] = 1;
    }
    setThrottledRGB();
}


/*static*/ RGB BladePWM::convertRawToPerceived(const RGB& raw)
{
    #ifdef LED_TYPE 
        RGB rgb;
        #if (LED_TYPE == LED_TYPE_BBG)
            rgb.r = 0;
            // Fancy +1 and clamp so that color=255 is halfed to 128 (not 127). Scaling
            // routines later tend to just use the high bit.
            rgb.g = (uint8_t) clamp((uint32_t(raw.r) + 1)/2 + (uint32_t(raw.g) + 1)/2, uint32_t(0), uint32_t(255));
            rgb.b = (raw.b + 1) / 2;
        #elif (LED_TYPE == LED_TYPE_WWG)
            uint32_t r = uint32_t(raw.r / 2) + uint32_t(raw.b /2 );
            uint32_t g = uint32_t(raw.r / 2) + uint32_t(raw.b /2 ) + uint32_t(raw.g);
            uint32_t b = uint32_t(raw.r / 2) + uint32_t(raw.b /2 );
            rgb.r = clamp(r, uint32_t(0), uint32_t(255));
            rgb.g = clamp(g, uint32_t(0), uint32_t(255));
            rgb.b = clamp(b, uint32_t(0), uint32_t(255));
        #elif (LED_TYPE == LED_TYPE_GGC)
            uint32_t r = 0;
            uint32_t g = uint32_t(raw.r / 2) + uint32_t(raw.g / 2 ) + uint32_t(raw.b / 2);
            uint32_t b = uint32_t(raw.b / 2 );
            rgb.r = clamp(r, uint32_t(0), uint32_t(255));
            rgb.g = clamp(g, uint32_t(0), uint32_t(255));
            rgb.b = clamp(b, uint32_t(0), uint32_t(255));
        #else
            #error LED_TYPE not defined.
        #endif
        return rgb;
    #else
        return raw;
    #endif
}
