#include "fixed.h"
#include "Grinliz_Util.h"

bool TestFixed()
{
    {
        Fixed88 a(5, 2);
        TEST_IS_TRUE(a.getInt() == 2);
        TEST_IS_TRUE(a.getDec() == 65536 / 2);

        Fixed88 b;
        b = 2;
        Fixed88 c(1, 2);

        Fixed88 d = a + b + c;
        TEST_IS_TRUE(d == 5);
    }

    {
        Fixed115 a(5, 2);
        TEST_IS_TRUE(a.getInt() == 2);
        TEST_IS_TRUE(a.getDec() == 65536 / 2);

        Fixed115 b;
        b = 2;
        Fixed115 c(1, 2);

        Fixed115 d = a + b + c;
        TEST_IS_TRUE(d == 5);
    }
    {
        FixedNorm a(5, 2);
        TEST_IS_TRUE(a.getInt() == 2);
        TEST_IS_TRUE(a.getDec() == 65536 / 2);

        FixedNorm b;
        b = 2;
        FixedNorm c(1, 2);

        FixedNorm d = a + b + c;
        TEST_IS_TRUE(d == 5);
    }
    {
        Fixed115 a(-4);
        Fixed115 b(-7);

        Fixed115 c = a * b;
        TEST_IS_TRUE(c == 28);
        c = a + b;
        TEST_IS_TRUE(c == -11);
        c = a - b;
        TEST_IS_TRUE(c == 3);
        c = a / Fixed115(-1);
        TEST_IS_TRUE(c == 4);
        c = a / 2;
        TEST_IS_TRUE(c == -2);
    }
    {
        Fixed115 a(4);
        Fixed115 ar = a.sqrt();
        TEST_IS_TRUE(ar == 2);

        Fixed115 b(16);
        Fixed115 br = b.sqrt();
        TEST_IS_TRUE(br == 4)
    }
    {
        FixedNorm a(4, 100);
        FixedNorm ar = a.sqrt();
        TEST_IS_TRUE(ar.getRaw() == 817);
    }
    return true;
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

