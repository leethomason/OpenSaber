#include "swing.h"
#include <math.h>

Filter::Filter()
{
    for(int i=0; i<N; ++i) {
        sample[i].t = 0;
        sample[i].m.setZero();
    }
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
    m_prevPos4.setZero();
}

void Swing::push(uint32_t t, const Vec3<int32_t>& x, const Vec3<int32_t>& x0, const Vec3<int32_t>& x1)
{
    if (m_prevTime == 0) {
        m_prevTime = t;
        m_speed = 0;
        m_prevPos4 = x;
        m_filter.fill(t, x);
        return;
    }

    m_filter.push(t, x);
    uint32_t newTime;
    Vec3<int32_t> newPos;
    m_filter.calc(&newTime, &newPos);

    // sin(t) = t, for small t (in radians)
    Vec3<int32_t> c = newPos - m_prevPos4;

    c.set(x - m_pos.x, y - m_pos.y, z - m_pos.z);
    float dist = sqrtf(c.x * c.x + c.y * c.y + c.z * c.z);

    float dt = (msec - m_lastSampleTime) / 1000.0f;
    float speed = dist / dt;

    //filter.push(speed);
    //m_speed = filter.average();
    m_speed = speed;

    m_lastSampleTime = msec;
    m_pos.set(x, y, z);
}
