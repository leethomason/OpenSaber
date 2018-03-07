#include "pins.h"
#include "tester.h"
#include "sfx.h"
#include "blade.h"
#include "saberUtil.h"
#include "sketcher.h"
#include "rgb.h"
#include "voltmeter.h"
#include "unittest.h"

#include "Button.h"
#include "Grinliz_Arduino_Util.h"

Tester* Tester::s_instance = 0;

static const int HOLD_TIME = Button::DEFAULT_HOLD_TIME + 100;
static const int PRESS_TIME = Button::DEFAULT_BOUNCE_DURATION * 2;

#define TEST_EXISTS(expected) 						\
	ASSERT(expected);								\
	if (!(expected)) {								\
		Log.p("Expected true:").p(#expected).eol();	\
		while(true) {}								\
	}

#define TEST_EQUAL(expected, actual) 			\
	ASSERT(expected == actual);					\
	if (expected != actual) { 					\
		Log.p("Expected:").p(expected).eol(); 	\
		Log.p("Actual:").p(actual).eol();		\
		while(true)	{}							\
	 }

#define TEST_STR_EQUAL(expected, actual) 		\
	bool equal = strEqual(expected, actual);	\
	ASSERT(equal);								\
	if (!equal) { 								\
		Log.p("Expected:").p(expected).eol(); 	\
		Log.p("Actual:").p(actual).eol();		\
		while(true)	{}							\
	 }

#define TEST_RANGE(low, high, actual) 			\
	ASSERT(actual >= low && actual <= high);	\
	if (actual < low || actual > high) {		\
		Log.p("Low   :").p(low).eol(); 			\
		Log.p("High  :").p(high).eol();			\
		Log.p("Actual:").p(actual).eol();		\
		while(true) {}							\
	}

#define TEST_ORDER(index)						\
	TEST_EQUAL(tester->order, index);			\
	tester->order += 1;

class IgniteRetractTest : public Test
{
    uint16_t startEn = 0;
    uint16_t startDs = 0;

    virtual const char* name() const {
        return "IgniteRetractTest";
    }

    virtual void start(Tester* tester)
    {
        tester->press(0, HOLD_TIME);
    }

    virtual int process(Tester* tester, EventQueue* queue)
    {
        int result = TEST_CONTINUE;

        while(queue->hasEvent()) {
            EventQueue::Event e = queue->popEvent();
            if (!strStarts(e.name, "[BLADE_"))
                continue;

            static const char* EXPECTED[4] = {
                "[BLADE_IGNITE]",
                "[BLADE_ON]",
                "[BLADE_RETRACT]",
                "[BLADE_OFF]"
            };

            ASSERT(tester->getOrder() < 4);

            TEST_STR_EQUAL(EXPECTED[tester->getOrder()], e.name);
            if (tester->getOrder() == 1) {
                // Turn off again.
                tester->delayedPress(0, 1000, HOLD_TIME);
            }
            else if (tester->getOrder() == 3) {
                result = TEST_SUCCESS;
                break;
            }
            tester->incrementOrder();
        }
        return result;
    }
};


class BlasterTest : public Test
{
public:
    virtual const char* name() const {
        return "BlasterTest";
    }

    virtual void start(Tester* tester)
    {
        tester->press(0, HOLD_TIME);
    }

    virtual int process(Tester* tester, EventQueue* queue)
    {
        int result = TEST_CONTINUE;

        while(queue->hasEvent()) {
            EventQueue::Event e = queue->popEvent();
            //Log.p("BasterTest: ").p(e.name).p( "order=").p(tester->getOrder()).eol();
            if (!strStarts(e.name, "[BLADE_"))
                continue;

            static const char* EXPECTED[6] = {
                "[BLADE_IGNITE]",
                "[BLADE_ON]",
                "[BLADE_FLASH]",
                "[BLADE_ON]",
                "[BLADE_RETRACT]",
                "[BLADE_OFF]"
            };

            ASSERT(tester->getOrder() < 5);

            TEST_STR_EQUAL(EXPECTED[tester->getOrder()], e.name);
            switch(tester->getOrder()) {
            case 1:
                // Flash effect
                tester->delayedPress(0, 1000, PRESS_TIME);
                break;

            case 3:
                // Turn off again.
                tester->delayedPress(0, 1000, HOLD_TIME);
                break;

            default:
                break;
            }
            if (tester->getOrder() == 4) {
                result = TEST_SUCCESS;
                break;
            }
            tester->incrementOrder();
        }
        return result;
    }
};


class AveragePowerTest : public Test
{
public:

	static const int GOAL = 100;
	int nSamples = 0;

    virtual const char* name() const {
        return "AveragePowerTest";
    }

    virtual void start(Tester* tester)
    {
        AveragePower ave;
        TEST_EQUAL(NOMINAL_VOLTAGE, ave.power());
        for(int i=0; i<AveragePower::NUM_SAMPLES; ++i) {
            ave.push(4000);
        }
        TEST_EQUAL(4000, ave.power());
        ave.push(1000);
        TEST_EQUAL((4000*(AveragePower::NUM_SAMPLES-1) + 1000)/AveragePower::NUM_SAMPLES, ave.power());

        tester->press(0, HOLD_TIME);
    }

    virtual int process(Tester* tester, EventQueue* queue)
    {
        EventQueue::Event e;
        if (queue->hasEvent())
         	e = queue->popEvent();

        if (strEqual("[BLADE_ON]", e.name)) {
            TEST_RANGE(3200, 5300, Blade::blade().voltage());
            nSamples++;
            if (nSamples == GOAL) 
            	tester->press(0, HOLD_TIME);
        }
        else if (strEqual("[BLADE_OFF]", e.name)) {
        	return TEST_SUCCESS;
        }
        return TEST_CONTINUE;
    }
};


class ButtonTest : public Test
{
public:
    int t = 0;

    virtual const char* name() const {
        return "ButtonTest";
    }
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
        EventQ.event("[dummyEvent]");
    }

    virtual int process(Tester* tester, EventQueue* queue)
    {
        queue->popEvent();
        return TEST_SUCCESS;
    }
};


ButtonTest buttonTest;
IgniteRetractTest igniteRetractTest;
BlasterTest blasterTest;
AveragePowerTest averagePowerTest;

Test* gTests[] = {
    &buttonTest,
    &igniteRetractTest,
    &blasterTest,
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
    runUnitTests();

    running = true;

    for(int i=0; i<2; ++i) {
        if (button[i]) button[i]->enableTestMode(true);
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
#	ifdef SABER_TWO_BUTTON
    TEST_EXISTS(button[1] != 0);
#	endif
    r.setSeed(0);

    Log.p("Test start: '").p(test->name()).p("'").eol();
    while(EventQ.hasEvent())
        EventQ.popEvent();

    test->start(this);
}

void Tester::done()
{
	//Log.p("done() currentTest=").p(currentTest).eol();

    ++currentTest;
    Test* test = gTests[currentTest];

    if (test) {
        start();
    }
    else {
        running = false;
        for(int i=0; i<2; ++i) {
            if (button[i]) button[i]->enableTestMode(false);
        }
        Log.p("---- Test run complete. -----").eol();
#if SERIAL_DEBUG == 0
        Log.attachSerial(0);
#endif
    }
}

void Tester::process()
{
    if (!running)
        return;

    Test* test = gTests[currentTest];
    ASSERT(test);

    uint32_t m = millis();

    for(int i=0; i<2; ++i) {
        if (pressState[i].start && pressState[i].start <= m) {
            //Log.p("Firing press button=").p(i).eol();
            button[i]->testPress();
            pressState[i].start = 0;
        }
        if (pressState[i].end && pressState[i].end <= m) {
            //Log.p("Firing release button=").p(i).eol();
            button[i]->testRelease();
            pressState[i].end = 0;
        }
    }

    if (m < delayTime) {
        return;
    }

    int result = test->process(this, &EventQ);

    if (result == Test::TEST_ERROR) {
        Log.p("Test ERROR: '").p(test->name()).p("'").eol();
        done();
    }
    else if (result == Test::TEST_SUCCESS) {
        Log.p("Test PASS: '").p(test->name()).p("'").eol();
        done();
    }
}

void Tester::delay(uint32_t t)
{
    delayTime = millis() + t;
}

void Tester::press(int button, uint32_t time)
{
    uint32_t m = millis();
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
