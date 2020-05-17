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

Filter::Filter()
{
    for(int i=0; i<N; ++i) {
        sample[i].setZero();
    }
}

void Filter::fill(const Vec3<int32_t> v)
{
    for(int i=0; i<N; ++i) {
        sample[i] = v;
    }
    current = 0;
    cached = true;
    ave = v;
}

void Filter::push(const Vec3<int32_t> v)
{
    sample[current] = v;
    current = (current + 1) % N;
    cached = false;
}

void Filter::calc(Vec3<int32_t>* vec3)
{
    if (!cached) {
        Vec3<int32_t> total;
        total.setZero();

        for (int i = 0; i < N; ++i) {
            total += sample[i];
        }
        ave.x = total.x >> SHIFT;
        ave.y = total.y >> SHIFT;
        ave.z = total.z >> SHIFT;
        cached = true;
    }
    *vec3 = ave;
}


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
        m_filter.fill(x);
        return;
    }

    m_filter.push(x);
    Vec3<int32_t> newPos;
    m_filter.calc(&newPos);

    // But we need normals.
    // This is a lot of math for our poor little microprocessor.
    Vec3<float> a = m_prevPosNorm;
    Vec3<float> b = normalize(newPos, mMin, mMax);
    //Log.p("in: ").v3(b.x, b.y, b.z).eol();

    // sin(t) = t, for small t (in radians)
    Vec3<float> c = b - a;
    float dist = sqrtf(c.x * c.x + c.y * c.y + c.z * c.z);

    m_speed = dist * m_dtINV;
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
    // Test filtering at 45deg / second
    static const Vec3<int32_t> mMin = {-100, -200, -300};
    static const Vec3<int32_t> mMax = { 300,  200,  100};

    static const Vec3<int32_t> x0 = {300,   0, -100};    // (1, 0, 0)
    static const Vec3<int32_t> x1 = {242, 142, -100};    // (0.71, 0.71, 0)

    static const float speedDeg = 3.1459f / 4.f;

    Swing swing(100);
    //Log.p("Target speed=").p(speedDeg).eol();

    for(int i=0; i<=10; ++i) {
        Vec3<int32_t> x;
        x.x = x0.x + i * (x1.x - x0.x) / 10;
        x.y = x0.y + i * (x1.y - x0.y) / 10;
        x.z = x0.z + i * (x1.z - x0.z) / 10;

        swing.push(x, mMin, mMax);
        //Log.p("Swing speed (").p(i).p(")=").p(swing.speed()).eol();
        TEST_IS_TRUE(swing.speed() >= 0 && swing.speed() <= speedDeg * 1.2f);

        if (i == 0)
            swing.setOrigin();
    }
    TEST_IS_TRUE(swing.speed() > speedDeg * 0.8f && swing.speed() < speedDeg * 1.2f);
    // We won't get to 0.71 because of filtering.
    TEST_IS_TRUE(swing.dotOrigin() > 0.71f && swing.dotOrigin() < 0.80f);

    return true;
}