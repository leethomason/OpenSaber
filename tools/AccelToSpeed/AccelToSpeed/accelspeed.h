#pragma once
#include <stdint.h>

class AccelSpeed
{
public:
    AccelSpeed();

    void push(float ax, float ay, float az, uint32_t microDT);

    // Current speed in m/s
    float speed() const { return m_speed; }

    // Normal vector derivative of speed. In m/s.
    float dSpeed() const { return m_dSpeed; }

    // mix of a to b, ranges from 0 to 1
    float mix() const { return m_mix; }

    float swingVolume() const;

private:
    template<class T> T Max(const T& a, const T& b) const {
        return a > b ? a : b;
    }
    template<class T> T Min(const T& a, const T& b) const {
        return a < b ? a : b;
    }
    template<class T> T Clamp(const T& v, const T& a, const T& b) const {
        if (v < a) return a;
        if (v > b) return b;
        return v;
    }

    void calcMix();

    float vx = 0;
    float vy = 0;
    float vz = 0;
    float m_speed = 0;

    float m_speed1 = 0;
    float m_dSpeed = 0;
    float m_maxDSpeed = 0;
    float m_minDSpeed = 0;

    float m_mix = 0;
    float m_mix1 = 0;
};
