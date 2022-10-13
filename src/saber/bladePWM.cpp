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
#include "linear.h"
#include "SAMD21turboPWM.h"

#include "./src/util/Grinliz_Util.h"

using namespace osbr;

void calcGravity2(Fixed115 ax, Fixed115 ay, Fixed115 az, Fixed115* g2, Fixed115* g2Normal)
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
TurboPWM turboPWM;

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

    turboPWM.setClockDivider(1, false);    
    turboPWM.timer(0, 1, 1000, true);
    turboPWM.timer(1, 1, 1000, true);
    turboPWM.timer(2, 1, 1000, true);
}


void BladePWM::setRGB(const RGB& rgb)
{
    // Can't early out. The voltage may have changed.
    // if (rgb == m_color)
    //    return;

    m_color = rgb;
    setThrottledRGB();
}

void BladePWM::setThrottledRGB()
{
    bool changed = false;
    for (int i = 0; i < NCHANNELS; ++i) {
        int32_t pwm = int32_t(m_throttle[i] * m_color[i] * 1000.0f / 255.0f);
        pwm = glClamp<int32_t>(pwm, 0, 1000);
        if (pwm != m_pwm[i])
            changed = true;
        m_pwm[i] = pwm;
    }
    if (changed) {
        // noInterrupts(); Doesn't seem to help with the flash.
        //for (int i = 0; i < NCHANNELS; ++i) {
        //    analogWrite(pinRGB[i], m_pwm[i]);
        //}

        turboPWM.analogWrite(9,  m_pwm[0]);
        turboPWM.analogWrite(10, m_pwm[1]);
        turboPWM.analogWrite(11, m_pwm[2]);        
#if 0        
        // Useful for debugging.
        // Doesn't show the flash.
        Log.p("pwm=").p(m_pwm[0]).p(" ").p(m_pwm[1]).p(" ").p(m_pwm[2])
            .p(" throttle=").p(m_throttle[0]).p(" ").p(m_throttle[1]).p(" ").p(m_throttle[2])
            .p(" vbat=").p(m_vbat)
            .eol();
#endif        
    }
}

void BladePWM::setVoltage(int milliVolts) 
{
    static const int32_t DIV = 1000;
    static const int32_t vF[NCHANNELS]   = { RED_VF, GREEN_VF, BLUE_VF };
    static const int32_t amps[NCHANNELS] = { RED_I,  GREEN_I,  BLUE_I };
    static const int32_t res[NCHANNELS]  = { RED_R,  GREEN_R,  BLUE_R };
    static const int32_t sNum[NCHANNELS] = {
        amps[0] * res[0] / DIV,    // 400 * 4000 / 1000 = 1600
        amps[1] * res[1] / DIV,    // 400 * 1000 / 1000 =  400
        amps[2] * res[2] / DIV,    // 400 * 1350 / 1000 =  540
    };

    milliVolts = glClamp<int32_t>(milliVolts, 3000, 5400);
    if (m_vbat == milliVolts)
        return;
    m_vbat = milliVolts;

    for (int i = 0; i < NCHANNELS; ++i) {
        // throttle = I * R / (Vbat - Vf)

        const int32_t num = sNum[i];            // ~1000
        const int32_t denom = m_vbat - vF[i];   // high: 4100 - 3000 = 1100
                                                // low:  3400 - 3000 =  400

        if (num >= denom) {
            m_throttle[i] = 1;
        }
        else {
            m_throttle[i] = float(num) / denom;
        }
    }
    // The setRGB loop will update for the voltage change.
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
