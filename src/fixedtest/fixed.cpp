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

#include "fixed.h"
#include "math.h"
#include "fixed.hpp"
#include "math.hpp"

using namespace fpm;
using F16 = fpm::fixed_16_16;
using F8 = fpm::fixed<std::int16_t, std::int32_t, 8>;

Fixed16 sqrt(Fixed16 f) {
    return f.sqrt();
}

Fixed88 sqrt(Fixed88 f) {
    return f.sqrt();
}

template<typename T>
int SpeedTest() 
{
    static const int N = 1000;
    // Simulate swing calc.
    static const int NUM_SAMPLES = 8;
    static const T sample[NUM_SAMPLES] = { 3.0f, 3.1f, 3.2f, 3.3f, 3.4f, 3.5f, 3.6f, 3.7f };

    int count = 0;
    T rotation = 0;
    const T time = 0.1f;

    for (int i = 0; i < N; ++i) {
        T gyro[3] = { 
            sample[(i + 0) % NUM_SAMPLES], 
            sample[(i + 1) % NUM_SAMPLES], 
            sample[(i + 2) % NUM_SAMPLES]
        };
        T speed2 = gyro[0] * gyro[0] + gyro[1] * gyro[1] + gyro[2] * gyro[2];
        T speed = sqrt(speed2);

        rotation += speed * time;
        if (rotation > 180)
            rotation -= 360;

        if (speed > 3.5f)
            ++count;
        if (rotation > 0)
            ++count;
    }
    return count;
}

int SpeedTestFloat() {
    return SpeedTest<float>();
}

int SpeedTestFixed16() {
    return SpeedTest<Fixed16>();
}

int SpeedTestFixed88() {
    return SpeedTest<Fixed88>();
}

int SpeedTestFPM8() 
{
    static const int N = 1000;
    // Simulate swing calc.
    static const int NUM_SAMPLES = 8;
    static const F8 sample[NUM_SAMPLES] = { F8{3.0f}, F8{3.1f}, F8{3.2f}, F8{3.3f}, F8{3.4f}, F8{3.5f}, F8{3.6f}, F8{3.7f} };

    int count = 0;
    F8 rotation{ 0 };
    const F8 time { 0.1f };

    for (int i = 0; i < N; ++i) {
        F8 gyro[3] = { 
            sample[(i + 0) % NUM_SAMPLES], 
            sample[(i + 1) % NUM_SAMPLES], 
            sample[(i + 2) % NUM_SAMPLES]
        };
        F8 speed2 = gyro[0] * gyro[0] + gyro[1] * gyro[1] + gyro[2] * gyro[2];
        F8 speed = fpm::sqrt(speed2);

        rotation += speed * time;
        if (rotation > F8{180})
            rotation -= 360;

        if (speed > F8{3.5f})
            ++count;
        if (rotation > F8{0})
            ++count;
    }
    return count;
}

int SpeedTestFPM() 
{
    static const int N = 1000;
    // Simulate swing calc.
    static const int NUM_SAMPLES = 8;
    static const F16 sample[NUM_SAMPLES] = { F16{3.0f}, F16{3.1f}, F16{3.2f}, F16{3.3f}, F16{3.4f}, F16{3.5f}, F16{3.6f}, F16{3.7f} };

    int count = 0;
    F16 rotation{ 0 };
    const F16 time { 0.1f };

    for (int i = 0; i < N; ++i) {
        F16 gyro[3] = { 
            sample[(i + 0) % NUM_SAMPLES], 
            sample[(i + 1) % NUM_SAMPLES], 
            sample[(i + 2) % NUM_SAMPLES]
        };
        F16 speed2 = gyro[0] * gyro[0] + gyro[1] * gyro[1] + gyro[2] * gyro[2];
        F16 speed = fpm::sqrt(speed2);

        rotation += speed * time;
        if (rotation > F16{180})
            rotation -= 360;

        if (speed > F16{3.5f})
            ++count;
        if (rotation > F16{0})
            ++count;
    }
    return count;
}


// Such a great article: http://www.coranac.com/2009/07/sines/
// sin() approximation.
// x: angle with 2^15 units/circle (32768)
// return: sin, 12 bits (range -4096 to 4096)
int32_t iSin_S3(int32_t x)
{
    // S(x) = x * ( (3<<p) - (x*x>>r) ) >> s
    // n : Q-pos for quarter circle             13
    // A : Q-pos for output                     12
    // p : Q-pos for parentheses intermediate   15
    // r = 2n-p                                 11
    // s = A-1-p-n                              17

    static const int qN = 13, 
                     qA = 12, 
                     qP = 15, 
                     qR = 2 * qN - qP, 
                     qS = qN + qP + 1 - qA;

    x = x << (30 - qN);          // shift to full s32 range (Q13->Q30)

    if ((x ^ (x << 1)) < 0)     // test for quadrant 1 or 2
        x = (1 << 31) - x;

    x = x >> (30 - qN);

    return x * ((3 << qP) - (x*x >> qR)) >> qS;
}

int32_t iInvSin_S3(int32_t x)
{
    static const int16_t TABLE[66] = {
        0,    81,   163,  245,  326,  408,  490,  572, 654, 
        736,  818,  901,  984,  1067, 1150, 1234, 1318, 
        1402, 1487, 1572, 1658, 1744, 1830, 1917, 2005, 
        2093, 2182, 2271, 2362, 2453, 2544, 2637, 2731, 
        2825, 2921, 3018, 3116, 3215, 3315, 3417, 3521, 
        3626, 3733, 3842, 3953, 4067, 4183, 4301, 4423, 
        4548, 4676, 4809, 4946, 5089, 5237, 5393, 5556, 
        5730, 5916, 6117, 6338, 6589, 6885, 7269, 8192, 8192
    };
    if (x > ISINE_ONE) x = ISINE_ONE;
    if (x < -ISINE_ONE) x = -ISINE_ONE;
    int sign = 1;
    if (x < 0) {
        x = -x;  // symmetric
        sign = -1;
    }
    
    static const int DIV = ISINE_ONE / 64;
    int index = x / DIV;
    int fraction = x - index * DIV;
    int16_t a = TABLE[index];
    int16_t b = TABLE[index + 1];
    int32_t result = lerp1024(a, b, fraction * 1024 / DIV);

    return result * sign;
}

