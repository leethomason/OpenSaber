/*
  Copyright (c) Lee Thomason, Grinning Lizard Software

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
  of the Software, and to permit persons to whom the Software is furnished to do
  so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

/*
    Testing fixed point in the itsy M0.
    Very roughly a swing algorimth, simple math, square root.

    float r=1619  time=41289     floating point.
    fpm r=1619    time=35692     FPM 32 bit. Some results, but only slightly faster.
    fpm8 r=1213    time=8121     FPM 16 bit.
    fixed88 r=1202 time=6340     Fixed 16 bit. Fast, but close enough to fpm8.

    All in all, OpenSaber isn't about maintaining a fixed point math library.
    Been interesting, but time to move to an open source solution.

    The REAL test - OLED. Conversion was a wash, performance wise, which is good.
    - The different units for angles in a problem (but needs to be fixed)
    - There may be a slight quality loss because of the switch to 16.16 instead of
      hand set slopes. May fix later. Can see in rotated shapes.
    Biggest concern is the angle unit conversion.
*/

#ifndef FIXED_16_INCLUDED	
#define FIXED_16_INCLUDED

#include "../fpm/include/fpm/fixed.hpp"
#include "../fpm/include/fpm/math.hpp"
#include <stdint.h>

using Fixed88 = fpm::fixed<int16_t, int32_t, 8>;
using Fixed115 = fpm::fixed<int16_t, int32_t, 5>;
using FixedNorm = fpm::fixed<int16_t, int32_t, 12>;     // For things that are generally fractional
using Fixed610 = fpm::fixed<int16_t, int32_t, 10>;      // "wider" FixedNorm
using Fixed1616 = fpm::fixed<int32_t, int64_t, 16>;     // Note this isn't really faster than float. Use carefully.

constexpr float kPi_float = 3.1415926535897932384626433832795f;
constexpr float k2Pi_float = kPi_float * 2.0f;
constexpr Fixed88 kPi_Fixed88 = Fixed88{ 3.1415926535897932384626433832795f };
constexpr Fixed88 k2Pi_Fixed88 = Fixed88{ k2Pi_float };
constexpr Fixed115 kPi_Fixed115 = Fixed115{ 3.1415926535897932384626433832795f };
constexpr Fixed115 k2Pi_Fixed115 = Fixed115{ k2Pi_float };
constexpr FixedNorm kPi_FixedNorm = FixedNorm{ 3.1415926535897932384626433832795f };
constexpr FixedNorm k2Pi_FixedNorm = FixedNorm{ k2Pi_float };

constexpr FixedNorm kSqrt2Over2_FixedNorm{ 0.70710678118654752440084436210485 };

constexpr Fixed115 kZero_Fixed115{ 0 };
constexpr Fixed115 kOne_Fixed115{ 0 };

template<typename A, typename B, unsigned int C>
int32_t scale(fpm::fixed<A, B, C> fp, int32_t s) {
    static constexpr int32_t FIXED_1 = 1 << C;
    return s * fp.raw_value() / FIXED_1;
    }

template<typename A, typename B, unsigned int C>
bool isInt(fpm::fixed<A, B, C> fp) {
    const A mask = (1 << C) - 1;
    return (mask & fp.raw_value()) == 0;
    }

// Returns 0-1 on a sine curve, for an input from 0-1
template<typename T>
T sinLerp(T x) {
    // The actual range:
    // x    0  ->  1
    // sine pi/2 -> 3pi/2
    // correcting that the output is:
    // 1 -> -1

    if (x < T(0)) x = T(0);
    if (x > T(1)) x = T(1);

    T s = sin(T{ kPi_float * 0.5 } + x * T{ kPi_float });
    s = T{ 0.5 } *(-s) + T{ 0.5 };

    return s;
    }

template<typename T>
T cosLerp(T x) {
    return T{ 1 } - sinLerp(x);
    }

template<typename T>
T divToFixed(int32_t num, int32_t den) {
    static_assert(sizeof(T) == 2, "assumings an int16_t");

    if (den == 0) return T{ 0 };

    constexpr int32_t F1 = T{ 1 }.raw_value();
    int64_t n = num * F1 / den;
    return T::from_raw_value((int16_t)n);
        }

inline float toDegree(float rad) {
    return rad * 360.0f / k2Pi_float;
}

inline float toRad(float deg) {
    return deg * k2Pi_float / 360.0f;
}

bool TestFixed();

#endif // FIXED_16_INCLUDED
