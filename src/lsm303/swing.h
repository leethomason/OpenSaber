#pragma once

#include <stdint.h>
#include "Grinliz_Util.h"

class Filter
{
public:
    Filter();

    void push(uint32_t mSec, const Vec3<int32_t> sample);
    void fill(uint32_t mSec, const Vec3<int32_t> sample);

    static const int N = 4;
    static const int SHIFT = 2;

    // Calculate t (which is an average) and x/y/z which
    // are just added; so they are scaled by N.
    void calc(uint32_t *mSec, Vec3<int32_t> *vec3);

private:
    struct Sample
    {
        uint32_t t;
        Vec3<int32_t> m;
    };
    Sample sample[N];
    int current = 0;
};

class Swing
{
public:
    Swing();

    void push(uint32_t milliSecondsAbs, const Vec3<int32_t> &x, const Vec3<int32_t> &x0, const Vec3<int32_t> &x1);
    // speed in radians / second
    float speed() const { return m_speed; }

    static bool test();

private:
    Vec3<float> normalize(const Vec3<int32_t> x, const Vec3<int32_t> &x0, const Vec3<int32_t> &x1);

    uint32_t m_prevTime;
    float m_speed;
    Vec3<float> m_prevPosNorm;
    Filter m_filter;
};
