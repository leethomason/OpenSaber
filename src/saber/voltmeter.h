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

#ifndef SABER_VOLTMETER_INCLUDED
#define SABER_VOLTMETER_INCLUDED

#include "Grinliz_Util.h"
#include <stdint.h>

class Voltmeter
{
public:
    enum {SAMPLE_INTERVAL = 12};

    // vRef reference voltage in mVolts
    // rLower lower resistor (10000)
    // rUpper higher resistor (47000)
    // range max reading of vRef; 1023 or 4095
    // tune constant multiplied to answer 1000
    Voltmeter(uint32_t vRef, uint32_t rLower, uint32_t rHigher, uint32_t range, uint32_t tune)
        : m_averagePower(NOMINAL_VOLTAGE) 
    {       
        ASSERT(_instance == 0);
        _instance = this; 

        m_vRef = vRef;
        m_rLower = rLower;
        m_rHigher = rHigher;
        m_range = range;
        m_tune = tune;
    }
    static Voltmeter* instance() { return _instance; }

    // set the analog reference before calling
    void begin();

    /// Instantaneous power. (Noisy).
    uint32_t readVBat();
    /// Average power.
    uint32_t averagePower() const { return m_averagePower.average(); }

    /// Add a sample to the average power.
    uint32_t takeSample();

    static int vbatToPowerLevel(int32_t vbat, int nLevels);

private:
    static Voltmeter* _instance;
    uint32_t m_vRef = 0;
    uint32_t m_rLower = 0;
    uint32_t m_rHigher = 0;
    uint32_t m_range = 0;
    uint32_t m_tune = 0;
    AverageSample<uint16_t, uint32_t, 64> m_averagePower;
};

#endif // SABER_VOLTMETER_INCLUDED
