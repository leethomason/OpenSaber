#pragma once
#include <stdint.h>

class AccelSpeed
{
public:
    AccelSpeed();

    void push(float ax, float ay, float az, uint32_t microDT);
    float speed() const { return m_speed; }

private:
    float prevAX[2], prevAY[2], prevAZ[2];

    float vx = 0;
    float vy = 0;
    float vz = 0;
    float m_speed = 0;
};
