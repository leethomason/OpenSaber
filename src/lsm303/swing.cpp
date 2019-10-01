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
    m_pos4.setZero();
    m_speed = 0;
    m_time = 0;
}

void Swing::push(uint32_t t, const Vec3<int32_t>& x, const Vec3<int32_t>& x0, const Vec3<int32_t>& x1)
{
    #if false
    if (m_time == 0) {
        m_time = t;
        m_pos4 = x;
        m_filter.fill(m_time, m_pos4);
        return;
    }

    m_filter.push(t, x);

    Vec3<int32_t> speed4;
    uint32_t lastT;
    m_filter.calc(&lastT, &speed4);

    // sin(t) = t, for small t (in radians)
    Vec3 c;
    c.set(x - m_pos.x, y - m_pos.y, z - m_pos.z);
    float dist = sqrtf(c.x * c.x + c.y * c.y + c.z * c.z);

    float dt = (msec - m_lastSampleTime) / 1000.0f;
    float speed = dist / dt;

    //filter.push(speed);
    //m_speed = filter.average();
    m_speed = speed;

    m_lastSampleTime = msec;
    m_pos.set(x, y, z);
    #endif
}
