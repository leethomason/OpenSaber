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
        Fixed115 a(4);
        Fixed115 ar = a.sqrt();
        TEST_IS_TRUE(ar == 2);

        Fixed115 b(16);
        Fixed115 br = b.sqrt();
        TEST_IS_TRUE(br == 4)
    }
    return true;
}
