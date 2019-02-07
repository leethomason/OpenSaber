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
    bool risingEdge() const { return upEdge; }

private:
    bool upEdge = true;
    float vx = 0;
    float vy = 0;
    float vz = 0;
    float m_speed = 0;
    float m_mix = 0;
};
