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

private:
    float vx = 0;
    float vy = 0;
    float vz = 0;
    float m_speed = 0;

    float m_speed1 = 0;
    float m_speed2 = 0;
    float m_dSpeed = 0;
};
