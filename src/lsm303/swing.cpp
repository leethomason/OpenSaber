#include "swing.h"
#include <math.h>

Filter::Filter()
{
    for(int i=0; i<N; ++i) {
        sample[i].t = 0;
        sample[i].m.setZero();
    }
}

void Filter::fill(uint32_t mSec, const Vec3<int32_t> v)
{
    for(int i=0; i<N; ++i) {
        sample[i].m = v;
        sample[i].t = mSec;
    }
    current = 0;
}

void Filter::push(uint32_t mSec, const Vec3<int32_t> v)
{
    sample[current].t = mSec;
    sample[current].m = v;
    current = (current + 1) % N;
}

void Filter::calc(uint32_t* mSec, Vec3<int32_t>* vec3)
{
    Sample total;
    total.t = 0;
    total.m.setZero();

    for(int i=0; i<N; ++i) {
        total.t += sample[i].t;
        total.m += sample[i].m;
    }
    *mSec = total.t / N;
    *vec3 = total.m;
}


Swing::Swing()
{
    m_prevTime = 0;
    m_speed = 0;
    m_prevPosNorm.setZero();
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

void Swing::push(uint32_t t, const Vec3<int32_t>& x, const Vec3<int32_t>& mMin, const Vec3<int32_t>& mMax)
{
    if (m_prevTime == 0) {
        m_prevTime = t;
        m_speed = 0;
        m_prevPosNorm = normalize(x, mMin, mMax);
        m_filter.fill(t, x);
        return;
    }

    m_filter.push(t, x);
    uint32_t newTime;
    Vec3<int32_t> newPos;
    m_filter.calc(&newTime, &newPos);

    // But we need normals.
    // This is a lot of math for our poor little microprocessor.
    Vec3<float> a = m_prevPosNorm;
    Vec3<float> b = normalize(newPos, mMin, mMax);

    // sin(t) = t, for small t (in radians)
    Vec3<float> c = b - a;
    float dist = sqrtf(c.x * c.x + c.y * c.y + c.z * c.z);

    float dt = (newTime - m_prevTime) / 1000.0f;
    m_speed = dist / dt;

    m_prevTime = newTime;
    m_prevPosNorm = b;
}


bool Swing::test()
{
    // Test filtering at 45deg / second
    static const Vec3<int32_t> mMin = {-100, -200, -300};
    static const Vec3<int32_t> mMax = { 300, 200, 100};

    static const Vec3<int32_t> x0 = {300, -200, -300};  // (1, 0, 0)
    static const Vec3<int32_t> x1 = {100, 0, -300};     // (0.5, 0.5, 0)

    static const float speedDeg = 3.1459 / 8;

    Swing swing;

    for(int i=0; i<=10; ++i) {
        Vec3<int32_t> x;
        x.x = x0.x + i * (x1.x - x0.x) / 10;
        x.y = x0.y + i * (x1.y - x0.y) / 10;
        x.z = x0.z + i * (x1.z - x0.z) / 10;

        swing.push(i*100, x, mMin, mMax);
        Log.p("Swing speed (").p(i*100).p(")=").p(swing.speed()).eol();
        TEST_IS_TRUE(swing.speed() >= 0 && swing.speed() <= speedDeg);
    }
    TEST_IS_TRUE(swing.speed() > speedDeg - 0.01f && swing.speed() < speedDeg + 0.01f);

    return true;
}