#include "swing.h"
#include <math.h>

Swing::Swing(uint32_t sampleTime)
{
    m_sampleTime = m_sampleTime;
    m_lastSampleTime = 0;
    m_pos.x = m_pos.y = m_pos.z = 0;
    m_speed = 0;
}

void Swing::push(uint32_t msec, float x, float y, float z)
{
    if ((msec - m_lastSampleTime) <= m_sampleTime
        && x == m_pos.x 
        && y == m_pos.y
        && z == m_pos.z)
    {
        // duplicate sample
        return;   
    }

    if (m_pos.uninit()) {
        m_lastSampleTime = msec;
        m_pos.set(x, y, z);
        return;
    }

    // sin(t) = t, for small t (in radians)
    Vec3 c;
    c.set(x - m_pos.x, y - m_pos.y, z - m_pos.z);
    float dist = sqrtf(c.x * c.x + c.y * c.y + c.z * c.z);

    float dt = (msec - m_sampleTime) / 1000.0f;
    m_speed = dist / dt;

    m_lastSampleTime = msec;
    m_pos.set(x, y, z);
}
