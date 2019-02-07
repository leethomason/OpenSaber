#pragma once
#include <stdint.h>

class AccelSpeed
{
public:
    AccelSpeed();

    void push(float ax, float ay, float az, uint32_t microDT);

    // Current speed in m/s
    float speed() const { return m_speed; }
    // Mix of hum (0.0) to swing (1.0) sound
    float mix() const { return m_mix; }
    // if the edge is falling, it applies a larger damping force.
    bool isRisingEdge() const { return upEdge; }

    float getFRising() const { return F_UP; }
    float getFFalling() const { return F_DOWN; }
    float getLowerMix() const { return MIX_LOWER; }
    float getMixCap() const { return MIX_CAP; }

    void setFRising(float v) { F_UP = v; }
    void setFFalling(float v) { F_DOWN = v; }
    void setLowerMix(float v) { MIX_LOWER = v; }
    void setMixCap(float v) { MIX_CAP = v; }

private:
    // Tuning constant to control jitter and make sure velocity curves go to zero.
    //static const float F = 1.05f;
    float F_UP = 1.05f;
    float F_DOWN = 1.5f;
    float MIX_LOWER = 0.2f;    // lower velocity that starts a swing sound
    float MIX_CAP = 2.0f;      // fully mixed

    bool upEdge = true;
    float vx = 0;
    float vy = 0;
    float vz = 0;
    float m_speed = 0;
    float m_mix = 0;
};
