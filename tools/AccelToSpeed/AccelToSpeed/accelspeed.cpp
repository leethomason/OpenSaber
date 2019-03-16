#include "accelspeed.h"
#include <assert.h>
#include <math.h>

static const float G_LESS = 1.05f;
static const float G_MORE = 1.2f;
static const float DSPEED_EASING = 0.6f;
static const float MIX_EASING = 0.15f;
static const float MOTION_THRESHOLD = 2.0f;


AccelSpeed::AccelSpeed()
{
}

void AccelSpeed::calcMix()
{
    float mix = 0;
    if (m_maxDSpeed == 0.f) {
        mix = 0;
    }
    else if (m_minDSpeed < 0 && m_dSpeed > m_minDSpeed) {
        mix = 1.0f;
    }
    else {
        // dSpeed / maxDSpeed is roughly 1 to -1
        float f = 0.5f - 0.5f * (m_dSpeed / m_maxDSpeed);
        mix = Clamp(f, 0.0f, 1.0f);
    }
    m_mix = mix * MIX_EASING + m_mix1 * (1.0f - MIX_EASING);
    m_mix1 = m_mix;
}

void AccelSpeed::push(float ax_g, float ay_g, float az_g, uint32_t microDT)
{
    static const float MICRO_TO_S = float(1.0 / (1000.0 * 1000.0));
    static const float G = 9.81f;    // m/s2

    const float dts = MICRO_TO_S * float(microDT);

    // V(m/s) = V(m/s) + a(m/s2) * seconds
    vx = vx + ax_g * dts * G;
    vy = vy + ay_g * dts * G;
    vz = vz + az_g * dts * G;

    m_speed = (float)sqrt(vx*vx + vy * vy + vz * vz);

    // Use const time as an approximation.
    float dS = (m_speed - m_speed1) / dts;
    m_dSpeed = m_dSpeed * DSPEED_EASING + dS * (1.0f - DSPEED_EASING);

    m_speed1 = m_speed;

    if (m_maxDSpeed != 0.0f || m_speed > MOTION_THRESHOLD) {
        m_maxDSpeed = Max(m_dSpeed, m_maxDSpeed);
        m_minDSpeed = Min(m_dSpeed, m_minDSpeed);
    }
    calcMix();

    float g2 = ax_g * ax_g + ay_g * ay_g + az_g * az_g;
    bool more = (g2 > 0.5) && (g2 < 1.5);

    // Oppose velocity with acceleration equal to G
    if (m_speed != 0) {
        float gDrag = more ? G_MORE : G_LESS;
        float speedDrag = gDrag * G * dts;

        if (m_speed <= speedDrag) {
            vx = vy = vz = 0;
            m_maxDSpeed = 0;
            m_minDSpeed = 0;
            m_mix = 0;
            m_mix1 = 0;
        }
        else {
            vx += -speedDrag * (vx / m_speed);
            vy += -speedDrag * (vy / m_speed);
            vz += -speedDrag * (vz / m_speed);
        }
    }
}
