#pragma once

#include <stdint.h>

class Swing
{
public:
    Swing(uint32_t sampleTime);

    void push(uint32_t milliSecondsAbs, float x, float y, float z);
    // speed in radians / second
    float speed() const { return m_speed; }

private:
    struct Vec3
    {
        float x, y, z;
        bool uninit() const { return x == 0 && y == 0 && z == 0; }
        void set(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }
    };
    uint32_t m_lastSampleTime;
    uint32_t m_sampleTime;
    Vec3 m_pos;

    class Filter
    {
    public:
        static const int N_VALUES = 4;

        void push(float v)
        {
            values[c] = v;
            c = (c + 1) % N_VALUES;

            ave = values[(c - 1) % N_VALUES] * 0.4f +
                  values[(c - 1) % N_VALUES] * 0.3f +
                  values[(c - 1) % N_VALUES] * 0.2f +
                  values[(c - 1) % N_VALUES] * 0.1f;
        }

        float average() const { return ave; }

    private:
        unsigned c = 0;
        float values[N_VALUES] = {0};
        float ave;
    };

    //static const int N_SPEED = 4;
    //float m_speedCache;
    float m_speed;
};
