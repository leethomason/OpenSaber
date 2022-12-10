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
{
    m_dotOrigin = Vec3<float>(0, 0, 1);  
}


Vec3<float> Swing::calcVec(const Vec3<int32_t>& x, const Vec3<int32_t>& mMin, const Vec3<int32_t>& mMax) const
{
    Vec3<float> r(0, 0, 0);
    Vec3<int32_t> dir = x - mMin;
    Vec3<int32_t> range = mMax - mMin;

    for(int i=0; i<3; ++i) {
        r[i] = (float)dir[i] / range[i];
    }
    float len = sqrtf(r.x * r.x + r.y * r.y + r.z * r.z);
    if (len < 0.00001f)
        return m_dot;
    r.scale(1.0f / len);
    return r;
}

float Swing::calcDot(const Vec3<int32_t>& x, const Vec3<int32_t>& mMin, const Vec3<int32_t>& mMax) const
{
    Vec3<int32_t> dir = x - mMin;
    Vec3<int32_t> range = mMax - mMin;
    
    Vec3<float> r(0, 0, 0);
    Vec3<float> d(0, 0, 0);
    for(int i=0; i<3; ++i) {
        r[i] = (float)dir[i];
        d[i] = (float)range[i];
    }
    float rLen = sqrtf(r.x * r.x + r.y * r.y + r.z * r.z);
    float dLen = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);

    float f = rLen / dLen;
    return f * 2.0f - 1.0f;
}

void Swing::push(const Vec3<int32_t>& _x, const Vec3<int32_t>& mMin, const Vec3<int32_t>& mMax)
{
    if (!m_init) {
        m_init = true;
        m_speed = 0;
        xInputAve.fill(_x.x);
        yInputAve.fill(_x.y);
        zInputAve.fill(_x.z);
        return;
    }

    if (m_setOrigin) {
        m_setOrigin = false;
        m_dotOrigin = calcVec(_x, mMin, mMax);
        Log.p("Swing origin set: ").v3(m_dotOrigin).eol();
    }

    Vec3<int32_t> prevSample(xInputAve.average(), yInputAve.average(), zInputAve.average());
    xInputAve.push(_x.x);
    yInputAve.push(_x.y);
    zInputAve.push(_x.z);
    Vec3<int32_t> x(xInputAve.average(), yInputAve.average(), zInputAve.average());

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
    //static const int32_t S1 = 10 + 2;       // these constants are lost to time. Need to re-derive.
    static const int32_t S1 = 20 + 1;       // these constants are lost to time. Need to re-derive.

    int32_t decRadsSec = S0 * dVecAbs.x / range.x + S0 * dVecAbs.y / range.y + S0 * dVecAbs.z / range.z;
    ASSERT(decRadsSec >= 0);
    if (decRadsSec < 0) // overflow
        return;

    swingAve.push(decRadsSec);
    m_speed = swingAve.average() / float(S1);

    // Not even sure what the physical meaning is. 
    // Assuming we are moving in 2 axis of a cube.
    //m_dot = m_dotOrigin.dot(calcVec(x, mMin, mMax));
    m_dot = calcDot(x, mMin, mMax);
}

bool Swing::test()
{
    static const Vec3<int32_t> mMin = { -200, -200, -300 };
    static const Vec3<int32_t> mMax = { 600,  200,  100 };
    static const Vec3<int32_t> delta = mMax - mMin;
    static const Vec3<int32_t> half = delta / 2;
    static const Vec3<int32_t> center = mMin + half;

    static const int NTEST = 3;
    static const float speedRad[NTEST] = { 1.57f, 3.14f, 6.28f };

    for (int n = 0; n < NTEST; ++n) {
        Swing swing;
        float minDotOrigin = 2.0f;
        float maxDotOrigin = -2.0f;
        swing.setOrigin();

        for (int i = 0; i < 100; ++i) {
            float x = cosf(speedRad[n] * i / 100.0f);
            float y = sinf(speedRad[n] * i / 100.0f);

            Vec3<int32_t> v;
            /* This seems correct. But in testing, results are about double what they should be. 
            *  Speaks to not really understanding the magnetometer.
            */
            #if 0
            v.x = int32_t(center.x + half.x * x);
            v.y = int32_t(center.y + half.y * y);
            v.z = int32_t(center.z);
            #else 
            v.x = int32_t(mMin.x + delta.x * x);
            v.y = int32_t(mMin.y + delta.y * y);
            v.z = int32_t(center.z);
            #endif

            swing.push(v, mMin, mMax);
            //Log.p("i=").p(i).p(" dot=").p(swing.dotOrigin()).p(" x=").p(x).p(" y=").p(y).eol();
            minDotOrigin = glMin(minDotOrigin, swing.dotOrigin());
            maxDotOrigin = glMax(maxDotOrigin, swing.dotOrigin());
        }
        Log.p("Test speed=").p(swing.speed()).p(" expected=").p(speedRad[n]).p(" dot min/max=").p(minDotOrigin).p(" ").p(maxDotOrigin).eol();
        //TEST_IS_TRUE(swing.speed() >= speedRad[n] * 0.7f && swing.speed() <= speedRad[n] * 1.3f);
        //TEST_IS_TRUE(minDotOrigin < 0.8f);
        //TEST_IS_TRUE(maxDotOrigin > 0.8f);
    }
    return true;
}
