#include "accelspeed.h"
#include <assert.h>
#include <math.h>

// Tuning constant to control jitter and make sure velocity curves go to zero.
//static const float F = 1.05f;
static const float F_UP = 1.05f;
static const float F_DOWN = 1.5f;

static const float MIX_LOWER = 0.2f;    // lower velocity that starts a swing sound
static const float MIX_CAP = 2.0f;      // fully mixed

AccelSpeed::AccelSpeed()
{
}

void AccelSpeed::push(float ax, float ay, float az, uint32_t microDT)
{

    static const float MICRO_TO_S = 1.f / 1000000.0f;
    static const float G = 9.81f;    // m/s2

    const float dts = MICRO_TO_S * float(microDT);

    // V(m/s) = V(m/s) + a(m/s2) * seconds
    vx = vx + ax * G * dts;
    vy = vy + ay * G * dts;
    vz = vz + az * G * dts;


    m_speed = (float)sqrt(vx*vx + vy*vy + vz*vz);
    if (m_speed > 0.5f) {
        // Once the speed jumps "enough", increase
        // damping.
        upEdge = false;
    }

    if (m_speed <= MIX_LOWER) {
        m_mix = 0;
        upEdge = true;
    }
    else if (m_speed >= MIX_CAP) {
        m_mix = 1;
    }
    else {
        m_mix = (m_speed - MIX_LOWER) / (MIX_CAP - MIX_LOWER);
    }

    // Oppose velocity with acceleration equal to G
    if (m_speed != 0) {
        const float F = upEdge ? F_UP : F_DOWN;
        float gx = -G * F * dts * vx / m_speed;
        float gy = -G * F * dts * vy / m_speed;
        float gz = -G * F * dts * vz / m_speed;
        
        vx += gx;
        vy += gy;
        vz += gz;
    }
}
