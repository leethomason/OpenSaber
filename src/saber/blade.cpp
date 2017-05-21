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

#include "blade.h"
#include "pins.h"
#include <Arduino.h>

const int8_t Blade::pinRGB[NCHANNELS] = { PIN_EMITTER_RED, PIN_EMITTER_GREEN, PIN_EMITTER_BLUE };
Blade* Blade::instance = 0;
static const int32_t LED_RANGE = 255;

Blade::Blade() {
    for (int i = 0; i < NCHANNELS; ++i) {
        pinMode(pinRGB[i], OUTPUT);
        digitalWrite(pinRGB[i], LOW);
        pwm[i] = 0;
    }

    vbat   = NOMINAL_VOLTAGE;
    for (int i = 0; i < NCHANNELS; ++i) {
        f1000[i] = 1000;
        color[i] = 0;
    }
    instance = this;
}


void Blade::setRGB(const RGB& input)
{
    static const int32_t DIV = int32_t(255) * int32_t(1000);
    for (int i = 0; i < NCHANNELS; ++i ) {
        color[i] = input[i];
        int32_t v = LED_RANGE * f1000[i] * int32_t(input[i]) / DIV;
        pwm[i] = constrain(v, 0, 255);  // just in case...

#if SERIAL_DEBUG == 1
        /*
          Serial.print("setRGB:");
          Serial.print(i);
          Serial.print(" input:" ); Serial.print(input[i]);
          Serial.print(" f:"); Serial.print(f1000[i]);
          Serial.print(" v:"); Serial.print(v);
          Serial.print(" div:"); Serial.print(DIV);
          Serial.print(" output:"); Serial.println(prime);
        */
#endif
        analogWrite(pinRGB[i], pwm[i]);
    }
}


bool Blade::setInterp(uint32_t delta, uint32_t effectTime, const RGB& startColor, const RGB& endColor)
{
    if (delta >= effectTime) {
        setRGB(endColor);
        return true;
    }
    else {
        uint8_t t = uint8_t(255 * delta / effectTime);
        RGB color;
        for (int i = 0; i < NCHANNELS; ++i) {
            color[i] = lerpU8(startColor[i], endColor[i], t);
        }
        setRGB(color);
    }
    return false;
}


void Blade::setVoltage(int milliVolts) {
    // If a driver is in use, just use straight PWM, and
    // leave f1000 = 1000. If a resistor is in use,
    // account for the voltage being above design voltage.

#if (LED_TOPOLOGY == LED_TOPOLOGY_RESISTOR)
    static const int32_t f = 256;   // 256 is "use new value"
    static const int32_t m = 256 - f;
    int32_t vF[NCHANNELS]   = { RED_VF, GREEN_VF, BLUE_VF };
    int32_t amps[NCHANNELS] = { RED_I,  GREEN_I,  BLUE_I };
    int32_t res[NCHANNELS]  = { RED_R,  GREEN_R,  BLUE_R };

    vbat = (int32_t(milliVolts) * f + vbat * m) / int32_t(256);

    for (int i = 0; i < NCHANNELS; ++i) {
        f1000[i] = amps[i] * res[i] / (vbat - vF[i]);
        f1000[i] = clamp(f1000[i], 0, 1000);
  }
#elif (LED_TOPOLOGY == LED_TOPOLOGY_DRIVER)
    vbat = int32_t(milliVolts);  // used for reporting on the command line, but f1000 isn't change,
    // so therefore the blade power doesn't change.
#endif
    setRGB(color);
}


int32_t Blade::power() const {
    int32_t amps = 0;
#if (LED_TOPOLOGY == LED_TOPOLOGY_RESISTOR)
    static const int32_t cV[NCHANNELS] = { RED_VF, GREEN_VF, BLUE_VF };
    static const int32_t cR[NCHANNELS] = { RED_R, GREEN_R, BLUE_R };

    for (int i = 0; i < NCHANNELS; ++i) {
        int32_t cI = int32_t(1000) * (NOMINAL_VOLTAGE - cV[i]) / cR[i];

        int32_t aChannel = int32_t(LED_RANGE) * cI * int32_t(color[i]) / int32_t(65025);
        amps += aChannel;
    }
#elif (LED_TOPOLOGY == LED_TOPOLOGY_DRIVER)
    static const int32_t cA[NCHANNELS] = { RED_I, GREEN_I, BLUE_I };

    for (int i = 0; i < NCHANNELS; ++i) {
        int32_t aChannel = cA[i] * int32_t(color[i]) / int32_t(255);
        amps += aChannel;
    }
#else
#error Topology not defined.
#endif
    return amps;
}


/*static*/ RGB Blade::convertRawToPerceived(const RGB& raw)
{
    #ifdef LED_TYPE 
        RGB rgb;
        #if (LED_TYPE == BBG)
            rgb.r = 0;
            // Fancy +1 and clamp so that color=255 is halfed to 128 (not 127). Scaling
            // routines later tend to just use the high bit.
            rgb.g = (uint8_t) clamp((uint32_t(raw.r) + 1)/2 + (uint32_t(raw.g) + 1)/2, 0, 255);
            rgb.b = (raw.b + 1) / 2;
        #else
            #error LED_TYPE not defined.
        #endif
        return rgb;
    #else
        return raw;
    #endif
}
