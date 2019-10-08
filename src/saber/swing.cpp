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
}

void Filter::push(const Vec3<int32_t> v)
{
    sample[current] = v;
    current = (current + 1) % N;
}

void Filter::calc(Vec3<int32_t>* vec3)
{
    Vec3<int32_t> total;
    total.setZero();

    for(int i=0; i<N; ++i) {
        total += sample[i];
    }
    vec3->x = total.x >> SHIFT;
    vec3->y = total.y >> SHIFT;
    vec3->z = total.z >> SHIFT;
}


Swing::Swing(int msecPerSample)
{
    m_speed = 0;
    m_prevPosNorm.setZero();
    m_init = false;
    m_dtINV = 1000.0f / msecPerSample;
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
        Log.p("init push: ").v3(m_prevPosNorm.x, m_prevPosNorm.y, m_prevPosNorm.z).eol();
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
    Log.p("in: ").v3(b.x, b.y, b.z).eol();

    // sin(t) = t, for small t (in radians)
    Vec3<float> c = b - a;
    float dist = sqrtf(c.x * c.x + c.y * c.y + c.z * c.z);

    m_speed = dist * m_dtINV;
    m_prevPosNorm = b;
}


bool Swing::test()
{
    // Test filtering at 45deg / second
    static const Vec3<int32_t> mMin = {-100, -200, -300};
    static const Vec3<int32_t> mMax = { 300, 200, 100};

    static const Vec3<int32_t> x0 = {300,  0, -100};    // (1, 0, 0)
    static const Vec3<int32_t> x1 = {242, 142, -100};     // (0.71, 0.71, 0)

    static const float speedDeg = 3.1459f / 4.f;

    Swing swing(100);
    Log.p("Target speed=").p(speedDeg).eol();

    for(int i=0; i<=10; ++i) {
        Vec3<int32_t> x;
        x.x = x0.x + i * (x1.x - x0.x) / 10;
        x.y = x0.y + i * (x1.y - x0.y) / 10;
        x.z = x0.z + i * (x1.z - x0.z) / 10;

        swing.push(x, mMin, mMax);
        Log.p("Swing speed (").p(i).p(")=").p(swing.speed()).eol();
        TEST_IS_TRUE(swing.speed() >= 0 && swing.speed() <= speedDeg * 1.2f);
    }
    TEST_IS_TRUE(swing.speed() > speedDeg * 0.8f && swing.speed() < speedDeg * 1.2f);

    return true;
}