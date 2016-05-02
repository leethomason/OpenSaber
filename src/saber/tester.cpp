#include "tester.h"
#include "sfx.h"
#include <Button.h>
#include <Grinliz_Arduino_Util.h>

Tester* Tester::s_instance = 0;

#define TEST_ASSERT_EQUAL(expected, actual) \
	ASSERT(expected == actual);	\
	if (expected != actual){ Log.p("Expected:").p(expected).eol(); Log.p("Actual:").p(actual).eol(); }

#define TEST_ASSERT_RANGE(low, high, actual) 	\
	ASSERT(actual >= low && actual <= high);	\
	if (actual < low || actual > high) {		\
		Log.p("Low   :").p(low).eol(); 			\
		Log.p("High  :").p(high).eol();			\
		Log.p("Actual:").p(actual).eol();		\
		while(true);							\
	} 

void Test::assertButtonA(Tester* tester) 
{
	ASSERT(tester->buttonA);
	ASSERT(tester->buttonACB);
	ASSERT(tester->buttonACB->cb_onHold);
}


class IgniteRetractTest : public Test
{
	virtual const char* name() const { return "IgniteRetractTest"; }
	virtual void start(Tester* tester) 
	{
		assertButtonA(tester);
		tester->buttonACB->cb_onHold(*tester->buttonA);
	}

	virtual int process(Tester* tester, const char* event, const char* eventData) 
	{
		int result = TEST_CONTINUE;

		assertButtonA(tester);
		if (strEqual(event, "[BLADE_IGNITE]")) {
			tester->delay(100);
			tester->fire("ignite time");
		}
		else if (strEqual(event, "ignite time")) {
			uint32_t len = SFX::instance()->lengthMillis();
			Log.p("ignite time:").p(len).eol();
			TEST_ASSERT_RANGE(500, 2000, len);
		}
		else if (strEqual(event, "[BLADE_ON]")) {
			tester->delay(100);
			tester->fire("hum time");
		}
		else if (strEqual(event, "hum time")) {
			uint32_t len = SFX::instance()->lengthMillis();
			Log.p("hum time:").p(len).eol();
			TEST_ASSERT_RANGE(1000, 120000, len);

			tester->buttonACB->cb_onHold(*tester->buttonA);
			tester->delay(100);
			tester->fire("retract time");
		}
		else if (strEqual(event, "retract time")) {
			uint32_t len = SFX::instance()->lengthMillis();
			Log.p("retract time:").p(len).eol();
			TEST_ASSERT_RANGE(500, 2000, len);
		}
		else if (strEqual(event, "[BLADE_OFF]")) {
			result = TEST_SUCCESS;
		}		
		return result;
	}
};


IgniteRetractTest igniteRetractTest;

Tester::Tester()
{
	s_instance = this;
}

void Tester::attach(const Button* buttonA, const Button* buttonB)
{
//	Log.p("Tester attached: ").p((uint32_t)buttonA).p(" ").p((uint32_t)buttonB).eol();
	this->buttonA = buttonA;
	this->buttonB = buttonB;
	if (buttonA) 
		buttonACB = buttonA->queryHandlers();
	if (buttonB)
		buttonBCB = buttonB->queryHandlers();
}

void Tester::start()
{
	test = &igniteRetractTest;
	Log.p("Test start: '").p(test->name()).p("'").eol();
	test->start(this);
}

void Tester::process()
{
	if (millis() < delayTime) {
		return;
	}	

	if (test) {
		const char* e = 0;
		const char* d = 0;
		if (testEvent) {
			e = testEvent;
			testEvent = 0;
			Log.p("test event:").p(e).eol();
		}
		else {
			e = Log.popEvent(&d);
		}

		if (e) {
			//Log.p("dispatch: ").p(e).p(d ? d : "<none>").eol();
			int result = test->process(this, e, d);

			if (result == Test::TEST_ERROR) {
				Log.p("Tester ERROR: '").p(test->name()).p("'").eol();
				test = 0;
			}
			else if (result == Test::TEST_SUCCESS) {
				Log.p("Tester pass: '").p(test->name()).p("'").eol();
				test = 0;
			}
		}
	}
}

void Tester::delay(uint32_t t) 
{
	delayTime = millis() + t;
}

void Tester::fire(const char* e) 
{
	testEvent = e;
}
