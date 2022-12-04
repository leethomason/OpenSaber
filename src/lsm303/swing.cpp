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

#include "swing.h"
#include <math.h>

Swing::Swing()
{}

void Swing::push(const Vec3<int32_t>& _x, const Vec3<int32_t>& mMin, const Vec3<int32_t>& mMax)
{
    if (!m_init) {
        m_init = true;
        m_speed = 0;
        inputAve.fill(_x);
        return;
    }

    Vec3<int32_t> prevSample = inputAve.average();
    inputAve.push(_x);
    Vec3<int32_t> x = inputAve.average();

    /*
        I used to do a very careful analysis of the normal vectors to calculate
        the small sin approximation. Turns out just multiplying the deltas by
        a constant works just as well.
  
        (|dx| + |dy| + |dz|) / len
    */
    Vec3<int32_t> dVec = x - prevSample;
    Vec3<int32_t> dVecAbs = dVec.vAbs();
    Vec3<int32_t> range = (mMax - mMin);

    // 100 samples a second
    static const int32_t S0 = 2000;
    static const int32_t S1 = 20;       // these constants are lost to time. Need to re-derive.
                                            // the 2nd term is a kluge factor   
    int32_t decRadsSec = S0 * dVecAbs.x / range.x + S0 * dVecAbs.y / range.y + S0 * dVecAbs.z / range.z;
    ASSERT(decRadsSec >= 0);
    if (decRadsSec < 0) // overflow
        return;

    swingAve.push(decRadsSec);
    m_speed = swingAve.average() / float(S1);
}

bool Swing::test()
{
    static const Vec3<int32_t> mMin = { -200, -200, -300 };
    static const Vec3<int32_t> mMax = { 600,  200,  100 };
    static const Vec3<int32_t> delta = mMax - mMin;
    //static const Vec3<int32_t> half = delta / 2;

    static const int NTEST = 3;
    static const float speedRad[NTEST] = { 1.57f, 3.14f, 6.28f };

    for (int n = 0; n < NTEST; ++n) {
        Swing swing;

        for (int i = 0; i < 100; ++i) {
            float x = cosf(speedRad[n] * i / 100.0f);
            float y = sinf(speedRad[n] * i / 100.0f);

            Vec3<int32_t> v;
            v.x = int32_t(mMin.x + x * delta.x);
            v.y = int32_t(mMin.y + y * delta.y);
            v.z = int32_t(mMin.z + delta.z / 2);

            swing.push(v, mMin, mMax);
        }
        Log.p("Test speed=").p(swing.speed()).eol();
        TEST_IS_TRUE(swing.speed() >= speedRad[n] * 0.7f && swing.speed() <= speedRad[n] * 1.3f);
    }
    return true;
}
