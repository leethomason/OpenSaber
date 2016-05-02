#include "tester.h"

#include <Button.h>
#include <Grinliz_Arduino_Util.h>

Tester* Tester::s_instance = 0;

Tester::Tester()
{
	s_instance = this;
}

void Tester::attach(const Button* buttonA, const Button* buttonB)
{
	Log.p("Tester attached: ").p((uint32_t)buttonA).p(" ").p((uint32_t)buttonB).eol();
	this->buttonA = buttonA;
	this->buttonB = buttonB;
	if (buttonA) 
		buttonACB = buttonA->queryHandlers();
	if (buttonB)
		buttonBCB = buttonB->queryHandlers();
}

void Tester::start()
{
	testState = TEST_STATE_START;
}

void Tester::process()
{
	igniteHumRetract();	
}

bool Tester::igniteHumRetract()
{
	if (testState == TEST_STATE_NONE) 
		return false;

	ASSERT(buttonA);
	ASSERT(buttonACB);
	ASSERT(buttonACB->cb_onHold);

	if (testState == TEST_STATE_START) {
		Log.p("Test start: igniteHumRetract").eol();
		buttonACB->cb_onHold(*buttonA);
		testState = TEST_STATE_RUN;
	}
	else if (testState == TEST_STATE_RUN) {
		const char* e = Log.popEvent();
		if (strEqual(e, "[BLADE_ON]")) {
			buttonACB->cb_onHold(*buttonA);
		}
		else if (strEqual(e, "[BLADE_OFF]")) {
			testState = TEST_STATE_NONE;
			Log.p("Test done: igniteHumRetract").eol();
		}
	}
	return true;
}
