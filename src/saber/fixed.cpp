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

