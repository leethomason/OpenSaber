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

Swing::Swing(int msecPerSample)
{
    m_speed = 0;
    m_dotOrigin = 0;
    m_origin.setZero();
    m_prevPosNorm.setZero();
    m_init = false;
    m_dtINV = 1000.0f / msecPerSample;
}

void Swing::recalibrate()
{
    m_init = false; 
    m_speed = 0; 
    m_dotOrigin = 0;
    Log.p("Swing.recalibrate()").eol();
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
        m_prevPosNorm = normalize(x, mMin, mMax);
        Log.p("Swing initial push: ").v3(m_prevPosNorm.x, m_prevPosNorm.y, m_prevPosNorm.z).eol();
        return;
    }

    // But we need normals.
    // This is a lot of math for our poor little microprocessor.
    Vec3<float> a = m_prevPosNorm;
    Vec3<float> b = normalize(x, mMin, mMax);
    //Log.p("in: ").v3(b.x, b.y, b.z).eol();

    // sin(t) = t, for small t (in radians)
    Vec3<float> c = b - a;
    float square = c.x * c.x + c.y * c.y + c.z * c.z;
    if (square > 0.0001f) {
        float dist = sqrtf(square);
        m_speed = dist * m_dtINV;
    }
    else {
        m_speed = 0.0f;
    }
    m_prevPosNorm = b;

    if (!m_origin.isZero()) {
        m_dotOrigin = m_origin.x * b.x + m_origin.y * b.y + m_origin.z * b.z;
    }
}


void Swing::setOrigin()
{
    if (m_init) {
        m_origin = m_prevPosNorm;
    }
}


bool Swing::test()
{
    static const Vec3<int32_t> mMin = { -100, -200, -300 };
    static const Vec3<int32_t> mMax = { 300,  200,  100 };
    static const Vec3<int32_t> delta = mMax - mMin;
    static const Vec3<int32_t> half = delta / 2;

    static const float speedDeg = 3.1459f / 2.f;
    Swing swing(10);

    // 90 deg/second
    for (int i = 0; i < 100; ++i) {
        float x = cosf(speedDeg * i / 100.0f);
        float y = sinf(speedDeg * i / 100.0f);
        if (i==10)
            swing.setOrigin();

        Vec3<int32_t> v;
        v.x = int32_t(mMin.x + x * half.x + half.x);
        v.y = int32_t(mMin.y + y * half.y + half.y);
        v.z = int32_t(mMin.z + half.z);

        swing.push(v, mMin, mMax);
    }
    TEST_IS_TRUE(swing.speed() >= speedDeg * 0.8f && swing.speed() <= speedDeg * 1.2f);
    TEST_IS_TRUE(swing.dotOrigin() > -0.3f && swing.dotOrigin() < 0.3f);

    return true;
}