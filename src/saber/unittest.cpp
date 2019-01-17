#include "unittest.h"
#include "Grinliz_Util.h"
#include "sketcher.h"
#include "fixed.h"

#define RUN_TEST(T)     \
    pass = T();         \
    allPass = allPass && pass;   \
    Log.p("Unit test: " #T).p(pass ? " PASS" : " **FAIL**").eol();

bool FailTest() { return false; }

bool runUnitTests()
{
    bool pass = true;
    bool allPass = true;

    RUN_TEST(TestHexDec);
    RUN_TEST(TestEvent);
	RUN_TEST(DotStarUI::Test);
	RUN_TEST(TestCStr);
    RUN_TEST(TestCQueue);
    RUN_TEST(Random::Test);
    RUN_TEST(TestUtil);
    RUN_TEST(TestFixed);

	//RUN_TEST(FailTest);

	Log.p("Unit tests ").p(allPass ? "PASSED" : "**FAILED**").eol();
	ASSERT(allPass);
	return allPass;
}
