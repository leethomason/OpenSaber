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

#ifndef _WIN32
#include <Arduino.h>
#include "Grinliz_Arduino_Util.h"
#endif

#include "Grinliz_Util.h"
#include "pins.h"
#include "voltmeter.h"

Voltmeter* Voltmeter::_instance = 0;

void Voltmeter::begin()
{
    m_averagePower.fill(3800);

    #ifdef SABER_VOLTMETER
    analogRead(PIN_VMETER);     // warm up the ADC to avoid spurious initial value.
    Log.p("Voltmeter open.").eol();
    delay(10);

    for(int i=0; i<m_averagePower.numSamples(); ++i) {
        m_averagePower.push(readVBat());
        delay(2);
    }
    Log.p("Vbat: ").p(readVBat()).p(" Ave:").p(averagePower()).eol();
    Log.p("Voltmeter initialized.").eol();
    #endif
}


uint32_t Voltmeter::readVBat() 
{
    #ifdef SABER_VOLTMETER
        uint32_t analog = analogRead(PIN_VMETER);
        
        // Voltage divider:
        //        Vref * s    (Rl + Rh)
        // Vhi = (--------) * --------- * k
        //          range         Rl

        // But don't want to overflow, so go in stages.
        uint32_t term0 = m_vRef * analog / m_range;
        uint32_t term1 = (m_rLower + m_rHigher) * m_tune / m_rLower;
        uint32_t mV = term0 * term1 / 1000; // 1000 normalizes k
        return mV;
    #else
        return NOMINAL_VOLTAGE;
    #endif
}

uint32_t Voltmeter::takeSample()
{
    uint32_t sample = readVBat();
    m_averagePower.push(sample);
    if (m_averagePower.origin())
        m_eased.push(m_averagePower.average());

    return sample;
}
