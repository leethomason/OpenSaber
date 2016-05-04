#include "tester.h"
#include "sfx.h"
#include "blade.h"
#include <Button.h>
#include <Grinliz_Arduino_Util.h>

Tester* Tester::s_instance = 0;

static const int HOLD_TIME = Button::DEFAULT_HOLD_TIME + 100;
static const int PRESS_TIME = Button::DEFAULT_BOUNCE_DURATION * 2;
static const int AUDIO_LAG_TIME = 100;
static const int AUDIO_CHECKED_TIME = 200;

#define TEST_EXISTS(expected) 				\
	ASSERT(expected);								\
	if (!(expected)) {								\
		Log.p("Expected true:").p(#expected).eol();	\
		while(true) {}								\
	}

#define TEST_EQUAL(expected, actual) 	\
	ASSERT(expected == actual);					\
	if (expected != actual) { 					\
		Log.p("Expected:").p(expected).eol(); 	\
		Log.p("Actual:").p(actual).eol();		\
		while(true)	{}							\
	 }

#define TEST_RANGE(low, high, actual) 	\
	ASSERT(actual >= low && actual <= high);	\
	if (actual < low || actual > high) {		\
		Log.p("Low   :").p(low).eol(); 			\
		Log.p("High  :").p(high).eol();			\
		Log.p("Actual:").p(actual).eol();		\
		while(true) {}							\
	} 

#define TEST_ORDER(index)						\
	TEST_EQUAL(tester->order, index);	\
	tester->order += 1;

class IgniteRetractTest : public Test
{
	virtual const char* name() const { return "IgniteRetractTest"; }
	virtual void start(Tester* tester) 
	{
		tester->press(0, HOLD_TIME);
	}

	virtual int process(Tester* tester, const char* event, const char* eventData) 
	{
		int result = TEST_CONTINUE;

		if (strEqual(event, "[BLADE_IGNITE]")) {
			TEST_ORDER(0);
			tester->checkAudio("ignite", 500, 2000);	
		}
		else if (strEqual(event, "[BLADE_ON]")) {
			TEST_ORDER(1);
			TEST_RANGE(1, 255*3, Blade::blade().pwmVal(0) + Blade::blade().pwmVal(1) + Blade::blade().pwmVal(2));
			tester->checkAudio("idle", 1000, 120000);	// hum
			tester->delayedPress(0, AUDIO_CHECKED_TIME, HOLD_TIME);
		}
		else if (strEqual(event, "[BLADE_RETRACT]")) {
			TEST_ORDER(2);
			tester->checkAudio("retract", 500, 2000);
		}
		else if (strEqual(event, "[BLADE_OFF]")) {
			TEST_ORDER(3);
			result = TEST_SUCCESS;
		}		
		return result;
	}
};


class PaletteTest : public Test
{
	int numChanges;
	static const int NUM_TESTS = 24;

	virtual const char* name() const { return "PaletteTest"; }
	virtual void start(Tester* tester) 
	{
		numChanges = 0;
		tester->press(0, HOLD_TIME);
	}

	virtual int process(Tester* tester, const char* event, const char* eventData) 
	{
		int result = TEST_CONTINUE;

		if (strEqual(event, "[BLADE_IGNITE]")) {
			TEST_ORDER(0);
			tester->checkAudio("ignite", 500, 2000);	
		}
		else if (strEqual(event, "[BLADE_ON]")) {
			TEST_ORDER(1);
			TEST_RANGE(1, 255*3, Blade::blade().pwmVal(0) + Blade::blade().pwmVal(1) + Blade::blade().pwmVal(2));
			tester->checkAudio("idle", 1000, 120000);

			tester->delayedPress(1, AUDIO_CHECKED_TIME, 300);
			tester->delayedPress(0, AUDIO_CHECKED_TIME + 100, 100);
		}
		else if (strEqual(event, "[PALETTE]")) {
			TEST_ORDER(2 + numChanges);
			numChanges++;
			tester->checkAudio("idle", 1000, 120000);		

			if (numChanges < NUM_TESTS) {
				tester->delayedPress(1, AUDIO_CHECKED_TIME, 500);
				tester->delayedPress(0, AUDIO_CHECKED_TIME + 300, 100);
			}
			else {
				tester->delayedPress(0, AUDIO_CHECKED_TIME, HOLD_TIME);
			}
		}		
		else if (strEqual(event, "[BLADE_RETRACT]")) {
			tester->checkAudio("retract", 500, 2000);
		}
		else if (strEqual(event, "[BLADE_OFF]")) {
			result = TEST_SUCCESS;
		}		
		return result;
	}
};


IgniteRetractTest igniteRetractTest;
PaletteTest paletteTest;

Tester::Tester()
{
	s_instance = this;
	for(int i=0; i<2; ++i) {
		pressState[i].start = pressState[i].end = 0;
		button[i] = 0;
	}
}

void Tester::attach(Button* buttonA, Button* buttonB)
{
	button[0] = buttonA;
	button[1] = buttonB;
}

void Tester::start()
{
	order = 0;
	TEST_EXISTS(button[0] != 0);
	for(int i=0; i<2; ++i) {
		if (button[i]) button[i]->enableTestMode(true);
	}

	//test = &igniteRetractTest;
	test = &paletteTest;

	Log.p("Test start: '").p(test->name()).p("'").eol();
	test->start(this);
}

void Tester::process()
{
	uint32_t m = millis();

	for(int i=0; i<2; ++i) {
		if (pressState[i].start && pressState[i].start <= m) {
			//Log.p("test press ").p(i).p(" ").p(pressState[i].start).eol();
			//Log.p("millis ").p(m).eol();
			button[i]->testPress();
			pressState[i].start = 0;
		}
		if (pressState[i].end && pressState[i].end <= m) {
			//Log.p("test testRelease ").p(i).p(" ").p(pressState[i].end).eol();
			//Log.p("millis ").p(m).eol();
			button[i]->testRelease();
			pressState[i].end = 0;
		}
	}

	if (m < delayTime) {
		return;
	}	

	if (test) {
		const char* e = 0;
		const char* d = 0;

		if (testAudio && testAudio <= m) {
			testAudio = 0;
			uint32_t len = SFX::instance()->lengthMillis();
			Log.p("Audio test:").p(audioName).p(" time(millis)=").p(len).eol();
			TEST_RANGE
	(audioLow, audioHigh, len);
		}
		e = Log.popEvent(&d);

		if (e) {
			//Log.p("dispatch: ").p(e).p(d ? d : "<none>").eol();
			int result = test->process(this, e, d);
			bool done = false;

			if (result == Test::TEST_ERROR) {
				Log.p("**Tester ERROR: '").p(test->name()).p("'").eol();
				test = 0;
				done = true;
			}
			else if (result == Test::TEST_SUCCESS) {
				Log.p("**Tester pass: '").p(test->name()).p("'").eol();
				test = 0;
				done = true;
			}
			if (done) {
				for(int i=0; i<2; ++i) {
					if (button[i]) button[i]->enableTestMode(false);
				}
			}
		}
	}
}

void Tester::delay(uint32_t t) 
{
	delayTime = millis() + t;
}

void Tester::checkAudio(const char* name, uint32_t low, uint32_t high)
{
	testAudio = millis() + AUDIO_LAG_TIME;
	audioName = name;
	audioLow = low;
	audioHigh = high;
}

void Tester::press(int button, uint32_t time)
{
	uint32_t m = millis();
	//Log.p("press ").p(button).p(" ").p(time).p(" current ").p(m).eol();
	pressState[button].start = m;
	pressState[button].end = m + time;
}

void Tester::delayedPress(int button, uint32_t delay, uint32_t time)
{
	ASSERT(button >= 0 && button < 2);
	uint32_t m = millis();
	pressState[button].start = m + delay;
	pressState[button].end   = m + delay + time;
}
