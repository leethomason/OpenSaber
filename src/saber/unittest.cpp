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

#include "unittest.h"
#include "./src/util/Grinliz_Util.h"
#include "sketcher.h"
#include "./src/util/fixed.h"
#include "swing.h"
#include "bladeflash.h"

#define RUN_TEST(T)     \
    pass = T();         \
    allPass = allPass && pass;   \
    Log.p("Unit test: " #T).p(pass ? " PASS" : " **FAIL**").eol();

bool FailTest() { return false; }

bool runUnitTests()
{
    bool pass = true;
    bool allPass = true;

    RUN_TEST(TestFixed);
    RUN_TEST(TestHexDec);
	RUN_TEST(DotStarUI::Test);
	RUN_TEST(TestCStr);
    RUN_TEST(TestAverageSample);
    RUN_TEST(TestCQueue);
    RUN_TEST(Random::Test);
    RUN_TEST(TestUtil);
    RUN_TEST(TestBase64);
    RUN_TEST(TestAccelToColor);
    RUN_TEST(Swing::test)

	//RUN_TEST(FailTest);

	Log.p("Unit tests ").p(allPass ? "PASSED" : "**FAILED**").eol();
	ASSERT(allPass);
	return allPass;
}
