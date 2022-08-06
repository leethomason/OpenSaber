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

#pragma once

#include <stdint.h>
#include "../util/Grinliz_Util.h"

// Accelerometer and magnetometer.
// Really like this chip and works well. As of 2022 can't buy more.
// I2C interface
class GrinlizLSM303
{
public:
    GrinlizLSM303(int /*_unusedPin*/ ) {}

    // divisor for raw data
    static const int DIV = 4096;    

    bool begin(const void*);

    void flushAccel(int reserve);

    bool sampleAccel(Fixed115* x, Fixed115* y, Fixed115* z);
    bool sampleAccel(Vec3<Fixed115>* v) {
        return sampleAccel(&v->x, &v->y, &v->z);
    }

    /* In my test bed, the rawData isn't even close to being correctly pre-set.
       the ranges [-400, 200] and [-500, 90] have come up. Also totally 
       possible that the presense of non-earth magnetic fields throws it off.

       The min/max is tracked by the library, and the x/y/z values are probably
       the ones to use.
    */
    bool hasMag() const { return true; }
    bool sampleMag(Vec3<int32_t> *v);
    Vec3<int32_t> getMagMin() const { return m_min; }
    Vec3<int32_t> getMagMax() const { return m_max; }
    bool magDataValid() const { return dataValid(INIT_T, m_min, m_max); }
    void recalibrateMag();

    void logMagStatus();

    // Reduces power consumption at lower frequency
    void setMagDataRate(int hz);   // 10, 20, 50, 100
    int getMagDataRate() const;

    void log(int nSamples) { logTest(nSamples, false); }
    void test() { logTest(20, true); }

private:
    static bool dataValid(int t, const Vec3<int32_t>& a, const Vec3<int32_t>& b) {
        return     (b.x - a.x > t) 
                && (b.y - a.y > t) 
                && (b.z - a.z > t);   
    }

    int available();

    void logTest(int nSamples, bool doAsserts);

    // how much delta do we need to operate? On the one hand, want to get the
    // swing on quickly. But after that, recalibrate should really have good 
    // data. So 2 constants: INIT_T and WARM_T
    static const int INIT_T = 100;  
    static const int WARM_T = 200;

    Vec3<int32_t> m_min, m_max;
    Vec3<int32_t> m_minQueued, m_maxQueued;

    void write8(uint8_t address, uint8_t reg, uint8_t value) const;
    uint8_t read8(uint8_t address, uint8_t reg) const;
};
