#pragma once

#include <stdint.h>
#include "Grinliz_Util.h"

class Filter
{
public:
    Filter();

    // Assum constant time. Pool assumption, but variable time
    // is tricky to filter. Especially since the sample time
    // isn't known.
    void push(const Vec3<int32_t> sample);
    void fill(const Vec3<int32_t> sample);

    static const int N = 4;
    static const int SHIFT = 2;

    // Calculate t (which is an average) and x/y/z which
    // are just added; so they are scaled by N.
    void calc(Vec3<int32_t> *vec3);

private:
    Vec3<int32_t> sample[N];
    int current = 0;
};  

class Swing
{
public:
    Swing(int mSecPerSample);

    void push(const Vec3<int32_t> &x, const Vec3<int32_t> &xMin, const Vec3<int32_t> &xMax);
    // speed in radians / second
    float speed() const { return m_speed; }

    static bool test();

private:
    Vec3<float> normalize(const Vec3<int32_t> x, const Vec3<int32_t> &x0, const Vec3<int32_t> &x1);

    bool m_init;
    float m_speed;
    float m_dtINV;
    Vec3<float> m_prevPosNorm;
    Filter m_filter;
};
