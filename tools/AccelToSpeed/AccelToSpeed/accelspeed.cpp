#include "accelspeed.h"
#include <assert.h>
#include <math.h>


AccelSpeed::AccelSpeed()
{
    for (int i = 0; i < 2; ++i) {
        prevAX[i] = prevAY[i] = prevAZ[i] = 0.6f;
    }
}

void AccelSpeed::push(float _ax, float _ay, float _az, uint32_t microDT)
{
    static const float MICRO_TO_S = 1.f / 1000000.0f;
    static const float G = 9.81f;    // m/s2
    static const float EASING = 0; // 0.15f;

    const float dts = MICRO_TO_S * float(microDT);

    /*
    float ax = _ax * 0.6f + prevAX[0] * 0.3f + prevAX[1] * 0.1f;
    float ay = _ay * 0.6f + prevAY[0] * 0.3f + prevAY[1] * 0.1f;
    float az = _az * 0.6f + prevAZ[0] * 0.3f + prevAZ[1] * 0.1f;

    prevAX[1] = prevAX[0];
    prevAY[1] = prevAY[0];
    prevAZ[1] = prevAZ[0];
    prevAX[0] = _ax;
    prevAY[0] = _ay;
    prevAZ[0] = _az;
    */

    float ax = _ax;
    float ay = _ay;
    float az = _az;

    // V(m/s) = V(m/s) + a(m/s2) * seconds
    vx = vx + ax * G * dts;
    vy = vy + ay * G * dts;
    vz = vz + az * G * dts;

    m_speed = (float)sqrt(vx*vx + vy * vy + vz * vz);

    // Oppose velocity with acceleration equal to G
    if (m_speed != 0) {
        float ease = 1.0f + m_speed * EASING;
        static const float F = 1.1f;

        float gx = -G * F * dts * ease * vx / m_speed;
        float gy = -G * F * dts * ease * vy / m_speed;
        float gz = -G * F * dts * ease * vz / m_speed;
        
        vx += gx;
        vy += gy;
        vz += gz;
    }
}
