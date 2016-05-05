#include "pins.h"
#include "electrical.h"
#include "tester.h"
#include "sfx.h"
#include "blade.h"
#include "saberUtil.h"

#include <Button.h>
#include <Grinliz_Arduino_Util.h>

Tester* Tester::s_instance = 0;

static const int HOLD_TIME = Button::DEFAULT_HOLD_TIME + 100;
static const int PRESS_TIME = Button::DEFAULT_BOUNCE_DURATION * 2;
static const int AUDIO_LAG_TIME = 100;
static const int AUDIO_CHECKED_TIME = 200;
static const int POWER_BUTTON = 0;
static const int EFFECT_BUTTON = 1;

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
public:
	int numChanges;
	static const int NUM_ITERATION = 24;

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

			if (numChanges < NUM_ITERATION) {
				uint32_t offset = tester->getRandom() % 300;
				tester->delayedPress(1, AUDIO_CHECKED_TIME, 500);
				tester->delayedPress(0, AUDIO_CHECKED_TIME + offset, 100);
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

class EffectTest : public Test
{
public:
	int nFlash = 0;
	bool firstCall = true;

	virtual void start(Tester* tester) 
	{
		nFlash = 0;
		firstCall = true;
		tester->press(POWER_BUTTON, HOLD_TIME);
	}
};

class BlasterTest : public EffectTest
{
public:
	virtual const char* name() const { return "BlasterTest"; }

	virtual int process(Tester* tester, const char* event, const char* eventData) 
	{
		int result = TEST_CONTINUE;

		if (strEqual(event, "[BLADE_ON]")) {
			tester->checkAudio("idle", 1000, 120000);	// hum
			//Blade::blade().getColor(bc);
			tester->delayedPress(EFFECT_BUTTON, AUDIO_CHECKED_TIME, 10);	// trigger flash
		}
		else if (strEqual(event, "[BLADE_FLASH]")) {
			//Blade::blade().getColor(ic);
			//bool equal = (bc[0] == ic[0] && bc[1] == ic[1] && bc[2] == ic[2]);
			//TEST_EQUAL(false, equal);
			tester->press(POWER_BUTTON, HOLD_TIME);
		}
		else if (strEqual(event, "[BLADE_OFF]")) {
			result = TEST_SUCCESS;
		}		
		return result;
	}
};

class ClashTest : public EffectTest
{
public:

	virtual const char* name() const { return "ClashTest"; }

	virtual int process(Tester* tester, const char* event, const char* eventData) 
	{
		int result = TEST_CONTINUE;

		if (strEqual(event, "[BLADE_ON]")) {
			if (firstCall) {
				tester->checkAudio("idle", 1000, 120000);	// hum
				//Blade::blade().getColor(bc);
				tester->delayedPress(EFFECT_BUTTON, AUDIO_CHECKED_TIME,             3000);		// trigger clash
				tester->delayedPress(POWER_BUTTON, AUDIO_CHECKED_TIME + 3000 + 200, HOLD_TIME);	// queue blade off
				firstCall = false;
			}
		}
		else if (strEqual(event, "[BLADE_FLASH]")) {
			nFlash++;
		}
		else if (strEqual(event, "[BLADE_OFF]")) {
			TEST_RANGE(3, 40, nFlash);
			result = TEST_SUCCESS;
		}		
		return result;
	}
};


class AveragePowerTest : public Test
{
public:
	virtual const char* name() const { return "AveragePowerTest"; }
	virtual void start(Tester* tester) 
	{
		AveragePower ave;
		TEST_EQUAL(NOMINAL_VOLTAGE, ave.power());
		for(int i=0; i<AveragePower::NUM_SAMPLES; ++i) {
			ave.push(4000);			
		}
		TEST_EQUAL(4000, ave.power());
		ave.push(1000);
		TEST_EQUAL((4000*4 + 1000)/AveragePower::NUM_SAMPLES, ave.power());
		tester->press(0, 50);
	}

	virtual int process(Tester* tester, const char* event, const char* eventData) 
	{
		if (strEqual(event, "[Vbat]")) {
			TEST_RANGE(3200, 4300, Blade::blade().voltage());
			return TEST_SUCCESS;
		}
		return TEST_CONTINUE;
	}
};


class ButtonTest : public Test
{
public:
	int t = 0;

	virtual const char* name() const { return "ButtonTest"; }
	virtual void start(Tester* tester)	
	{
		Button button(255);
		button.enableTestMode(true);

		// Press with bounce.
		button.testPress();
		button.process();
		TEST_EQUAL(button.press(), true);
		delay(5);
		button.process();
		button.testRelease();
		button.process();
		// should still be down: release filtered by de-bounce.
		TEST_EQUAL(button.isDown(), true);
		delay(5);
		button.testPress();
		button.process();
		TEST_EQUAL(button.isDown(), true);

		button.testRelease();
		button.process();
		// still too soon to register.
		TEST_EQUAL(button.isDown(), true);

		delay(25);
		button.process();
		// Not it has time to catch up, after bounce filter.
		TEST_EQUAL(button.isDown(), false);

		// Fire an event:
		tester->press(0, 50);
	}

	virtual int process(Tester* tester, const char* event, const char* eventData) 
	{
		return TEST_SUCCESS;
	}
};


ButtonTest buttonTest;
IgniteRetractTest igniteRetractTest;
BlasterTest blasterTest;
ClashTest clashTest;
PaletteTest paletteTest;
AveragePowerTest averagePowerTest;

Test* gTests[] = {
	&buttonTest,
	&igniteRetractTest,
	&blasterTest,
	&clashTest,
	&paletteTest,
	&averagePowerTest,
	0
};

Tester::Tester()
{
	s_instance = this;
	for(int i=0; i<2; ++i) {
		pressState[i].start = pressState[i].end = 0;
		button[i] = 0;
	}
	running = false;
}

void Tester::attach(Button* buttonA, Button* buttonB)
{
	button[0] = buttonA;
	button[1] = buttonB;
}

void Tester::runTests()
{
	Log.attachSerial(&Serial);
	running = true;
	for(int i=0; i<2; ++i) {
		if (button[i]) button[i]->enableTestMode(true);
	}
	for(int i=0; gTests[i]; i++) {
		gTests[i]->finalResult = 0;
	}
	currentTest = 0;
	start();
}

void Tester::start()
{
	Test* test = gTests[currentTest];
	ASSERT(test);

	order = 0;
	TEST_EXISTS(button[0] != 0);
	TEST_EXISTS(button[1] != 0);
	r.setSeed(0);

	Log.p("Test start: '").p(test->name()).p("'").eol();
	test->start(this);
}

void Tester::process()
{
	if (!running) 
		return;

	Test* test = gTests[currentTest];
	ASSERT(test);

	if (test->finalResult == Test::TEST_ERROR || test->finalResult == Test::TEST_SUCCESS) {
		++currentTest;
		if (gTests[currentTest]) {
			start();
			test = gTests[currentTest];
		}
		else {
			running = false;
			for(int i=0; i<2; ++i) {
				if (button[i]) button[i]->enableTestMode(false);
			}
			Log.p("******").eol();
			for(int i = 0; gTests[i]; ++i) {
				if (gTests[i]->finalResult == Test::TEST_ERROR)
					Log.p("  Tester ERROR: '").p(gTests[i]->name()).p("'").eol();
				else
					Log.p("  Tester pass: '").p(gTests[i]->name()).p("'").eol();
			}
			Log.p("******").eol();
			#if SERIAL_DEBUG == 0
			Log.attachSerial(0);
			#endif
			return;
		}
	}
	ASSERT(test);

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

	const char* e = 0;
	const char* d = 0;

	if (testAudio && testAudio <= m) {
		testAudio = 0;
		uint32_t len = SFX::instance()->lengthMillis();
		Log.p("Audio test:").p(audioName).p(" time(millis)=").p(len).eol();
		TEST_RANGE(audioLow, audioHigh, len);
	}
	e = Log.popEvent(&d);

	if (e) {
		//Log.p("dispatch: ").p(e).p(d ? d : "<none>").eol();
		int result = test->process(this, e, d);

		if (result == Test::TEST_ERROR) {
			Log.p("**Tester ERROR: '").p(test->name()).p("'").eol();
			test->finalResult = Test::TEST_ERROR;
		}
		else if (result == Test::TEST_SUCCESS) {
			Log.p("**Tester pass: '").p(test->name()).p("'").eol();
			test->finalResult = Test::TEST_SUCCESS;
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
