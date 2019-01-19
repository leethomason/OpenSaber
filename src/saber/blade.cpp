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

#include <Arduino.h>

#include "pins.h"
#include "blade.h"
#include "Grinliz_Util.h"

using namespace osbr;

#define DEBUG_DEEP

const int8_t Blade::pinRGB[NCHANNELS] = { PIN_EMITTER_RED, PIN_EMITTER_GREEN, PIN_EMITTER_BLUE };
Blade* Blade::instance = 0;

Blade::Blade() {
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


void Blade::setRGB(const RGB& input)
{
    ASSERT(!m_interpMode);      // blade ignite / retract can't be interupted
    setThrottledRGB(input);
}


void Blade::setThrottledRGB(const osbr::RGB& input)
{
    m_color = input;

    for (int i = 0; i < NCHANNELS; ++i ) {
        int32_t pwm = m_throttle[i].scale(input[i]);
        
        ASSERT(pwm >= 0);
        ASSERT(pwm <= 255);
        m_pwm[i] = constrain(pwm, 0, 255);  // just in case...
        
        analogWrite(pinRGB[i], m_pwm[i]);
    }
}


bool Blade::setInterp(uint32_t delta, uint32_t effectTime, const RGB& startColor, const RGB& endColor)
{
    if (delta >= effectTime) {
        setThrottledRGB(endColor);
        m_interpMode = false;
        return true;
    }
    else {
        m_interpMode = true;
        RGB color;
        uint8_t t = (delta * 256 / effectTime);
        for (int i = 0; i < NCHANNELS; ++i) {
            color[i] = lerpU8(startColor[i], endColor[i], t);
        }
        setThrottledRGB(color);
    }
    return false;
}


void Blade::setVoltage(int milliVolts) {
    static const int32_t vF[NCHANNELS]   = { RED_VF, GREEN_VF, BLUE_VF };
    static const int32_t amps[NCHANNELS] = { RED_I,  GREEN_I,  BLUE_I };
    static const int32_t res[NCHANNELS]  = { RED_R,  GREEN_R,  BLUE_R };

    #ifdef DEBUG_DEEP
        // Shouldn't see rapid change, and power should be averaged.
        if(milliVolts == 3700 || (m_vbat == 3700) || (abs(milliVolts - m_vbat) < 100)) {
            // all well.
        } 
        else {
            Log.p("Blade::setVoltage() vBat=").p(m_vbat).p(" mV=").p(milliVolts).eol();
            ASSERT(false);
        }
    #endif

    m_vbat = milliVolts;
    for (int i = 0; i < NCHANNELS; ++i) {
        // throttle = I * R / (Vbat - Vf)
        // Denominator x1000 corrects for units (mAmps, mOhms, mVolts)
        m_throttle[i] = FixedNorm(amps[i] * res[i] / 1000, m_vbat - vF[i] );
    }
    // This bug took forever to find. But obivous here: don't set the color
    // if the color in currently changing!
    if (!m_interpMode) {
        setRGB(m_color);
    }
}


/*static*/ RGB Blade::convertRawToPerceived(const RGB& raw)
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
