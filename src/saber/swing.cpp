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
    m_speed = 0;
    m_dotOrigin = 0;
    m_origin.setZero();
    m_normal.setZero();
    m_init = false;
}

Vec3<float> Swing::normalize(const Vec3<int32_t> v, const Vec3<int32_t> &mMin, const Vec3<int32_t> &mMax)
{
    Vec3<float> a;
    a.x = -1.0f + 2.0f * (v.x - mMin.x) / (mMax.x - mMin.x);
    a.y = -1.0f + 2.0f * (v.y - mMin.y) / (mMax.y - mMin.y);
    a.z = -1.0f + 2.0f * (v.z - mMin.z) / (mMax.z - mMin.z);

    float lenAInv = 1.0f / sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
    a.scale(lenAInv);
    return a;
}

void Swing::push(const Vec3<int32_t>& x, const Vec3<int32_t>& mMin, const Vec3<int32_t>& mMax)
{
    if (!m_init) {
        m_init = true;
        m_speed = 0;
        m_prevSample = x;
        m_normal = normalize(x, mMin, mMax);
        m_origin.set(1, 0, 0);
        Log.p("Swing initial push: ").v3(m_normal.x, m_normal.y, m_normal.z).eol();
        return;
    }

    // I used to do a very careful analysis of the normal vectors to calculate
    // the small sin approximation. Turns out just multiplying the deltas by
    // a constant works just as well.
    /*
        (|dx| + |dy| + |dz|) / len
    */
    m_normal = normalize(x, mMin, mMax);
    m_dotOrigin = m_normal.x * m_origin.x + m_normal.y * m_origin.y + m_normal.z * m_origin.z;

    Vec3<int32_t> dVec = x - m_prevSample;
    Vec3<int32_t> origin = (mMax + mMin) / 2;
    Vec3<int32_t> dVecAbs = dVec.vAbs();
    Vec3<int32_t> originToX = x - origin;

    int32_t len = intSqrt(originToX.x * originToX.x + originToX.y * originToX.y + originToX.z * originToX.z);
    if (len == 0)
        return;

    // 100 samples a second
    int32_t decRadsSec = 1000 * (dVecAbs.x + dVecAbs.y + dVecAbs.z) / len;
    swingAve.push(glMin(decRadsSec, int32_t(SWING_MAX * 10)));
    m_speed = swingAve.average() / 10.0f;

    m_prevSample = x;
}


void Swing::setOrigin()
{
    if (m_init) {
        m_origin = m_normal;
    }
}


bool Swing::test()
{
    static const Vec3<int32_t> mMin = { -100, -200, -300 };
    static const Vec3<int32_t> mMax = { 300,  200,  100 };
    static const Vec3<int32_t> delta = mMax - mMin;
    static const Vec3<int32_t> half = delta / 2;

    static const float speedRad = 1.57f;
    Swing swing;

    // 90 deg/second, 1.57 rad/sec
    for (int i = 0; i < 100; ++i) {
        float x = cosf(speedRad * i / 100.0f);
        float y = sinf(speedRad * i / 100.0f);
        if (i==10)
            swing.setOrigin();

        Vec3<int32_t> v;
        v.x = int32_t(mMin.x + x * half.x + half.x);
        v.y = int32_t(mMin.y + y * half.y + half.y);
        v.z = int32_t(mMin.z + half.z);

        float xp = cosf(speedRad * (i-1) / 100.0f);
        float yp = sinf(speedRad * (i-1) / 100.0f);
        float dx = x - xp;
        float dy = y - yp;
        float approx = 100.0f * (glAbs(dx) + glAbs(dy));

        swing.push(v, mMin, mMax);
    }
    TEST_IS_TRUE(swing.speed() >= speedRad * 0.8f && swing.speed() <= speedRad * 1.2f);
    TEST_IS_TRUE(swing.dotOrigin() > 0.0f && swing.dotOrigin() < 0.3f);
   
    return true;
}


void MagFilter::push(const Vec3<int32_t> &magData)
{
    // scale everything to divide more smoothly
    aveMagX.push(magData.x * SCALE);
    aveMagY.push(magData.y * SCALE);
    aveMagZ.push(magData.z * SCALE);
}
