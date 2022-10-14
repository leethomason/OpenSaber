#include "fixed.h"
#include "../util/Grinliz_Util.h"

bool TestFixed()
{
	TEST_EQUAL(Fixed88{ 0 }, sinLerp(Fixed88{ 0 }));
	TEST_EQUAL(Fixed88{ 1 }, sinLerp(Fixed88{ 1 }));
	Fixed88 l = sinLerp(Fixed88{ 0.5 });
	TEST_EQUAL_RANGE(Fixed88{ 0.5 }, l, Fixed88{ 0.01 });

	TEST_EQUAL(Fixed88{ 1 }, cosLerp(Fixed88{ 0 }));
	TEST_EQUAL(Fixed88{ 0 }, cosLerp(Fixed88{ 1 }));

	TEST_EQUAL(FixedNorm{ 0 }, sinLerp(FixedNorm{ 0 }));
	TEST_EQUAL(FixedNorm{ 1 }, sinLerp(FixedNorm{ 1 }));
	TEST_EQUAL_RANGE(FixedNorm{ 0.5 }, sinLerp(FixedNorm{ 0.5 }), FixedNorm{ 0.01 });

	Fixed88 d = divToFixed<Fixed88>(1, 1);
	TEST_EQUAL(Fixed88{ 1 }, d);
	TEST_EQUAL(Fixed88(1), divToFixed<Fixed88>(1000, 1000));
	TEST_EQUAL(Fixed88(2), divToFixed<Fixed88>(2000, 1000));
	TEST_EQUAL(Fixed88(0.5), divToFixed<Fixed88>(500, 1000));
    return true;
}