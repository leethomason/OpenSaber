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

#include "pins.h"
#include "tester.h"
#include "sfx.h"
#include "bladePWM.h"
#include "saberUtil.h"
#include "sketcher.h"
#include "rgb.h"
#include "voltmeter.h"
#include "unittest.h"
#include "saberdb.h"
#include "Button.h"
#include "bladecolor.h"

#include "./src/util/Grinliz_Arduino_Util.h"
#include "./src/lsm303/grinliz_LSM303.h"
#include "./src/lis3dh/grinliz_lis3dh.h"
#include "./src/lsm6d/grinliz_lsm6d.h"

using namespace osbr;

Tester* Tester::s_instance = 0;

extern ACCELEROMETER accelMag;

static const int HOLD_TIME = Button::DEFAULT_HOLD_TIME + 100;
static const int PRESS_TIME = Button::DEFAULT_BOUNCE_DURATION * 2;


class IgniteRetractTest : public Test
{
    virtual const char* name() const {
        return "IgniteRetractTest";
    }

    virtual void start(Tester* tester)
    {
        tester->holdButton();   // ignite
        tester->delay(2000);   
        tester->holdButton();   // retract
    }

    virtual int process(Tester* tester, uint32_t /*event*/)
    {
        if (tester->checkOnOff())
            return TEST_SUCCESS;
        return TEST_CONTINUE;
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
        tester->holdButton();   // ignite
        tester->delay(1500);
        tester->tapButton();    // blaster 
        tester->delay(300);
        tester->tapButton();    // blaster 
        tester->delay(1000);
        tester->holdButton();   // retract
    }

    virtual int process(Tester* tester, uint32_t /*event*/)
    {
        if (tester->checkOnOff())
            return TEST_SUCCESS;
        return TEST_CONTINUE;
    }
};


class ChannelTest : public Test
{
    RGB bladeColor;

    virtual const char* name() const {
        return "ChannelTest";
    }

    virtual void start(Tester* tester)
    {
        tester->holdButton();   // ignite
        tester->delay(1200);
        tester->sendEvent(0);   // red
        tester->delay(1000);
        tester->sendEvent(1);   // green
        tester->delay(1000);
        tester->sendEvent(2);   // blue
        tester->delay(1000);
        tester->sendEvent(3);   // restore
        tester->delay(500);
        tester->holdButton();   // retract
    }

    virtual int process(Tester* tester, uint32_t event)
    {
        BladePWM* blade = tester->getBladePWM();
        ASSERT(blade);
        BladeColor* bladeFlash = tester->getBladeColor();
        ASSERT(bladeFlash);

        switch(event) {
            case 0: 
                bladeColor = blade->getRGB();
                bladeFlash->setBladeColor(RGB(0x400000));
                blade->setRGB(RGB(0x400000));
                break;
            case 1: 
                bladeFlash->setBladeColor(RGB(0x004000));
                blade->setRGB(RGB(0x400000));
                break;
            case 2: 
                bladeFlash->setBladeColor(RGB(0x000040));
                blade->setRGB(RGB(0x400000));
                break;
            case 3:
                bladeFlash->setBladeColor(bladeColor);
                blade->setRGB(bladeColor);
                break;
            default:
                break;
        }
        if (tester->checkOnOff())
            return TEST_SUCCESS;
        return TEST_CONTINUE;
    }
};

class AccelerometerTest : public Test
{
public:

	static const int NDATA = 20;
	int nSamples;
    Vec3<Fixed115> data[NDATA];
    uint32_t startTime;

    virtual const char* name() const {
        return "AccelerometerTest";
    }

    virtual void start(Tester* /*tester*/)
    {
        accelMag.test();
        accelMag.log(5);
    }

    virtual int process(Tester*, uint32_t /*event*/)
    {
        return TEST_SUCCESS;
    }
};

class ButtonTest : public Test
{
public:
    int t = 0;

    virtual const char* name() const {
        return "ButtonTest";
    }
    virtual void start(Tester*)
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
    }

    virtual int process(Tester*, uint32_t /*event*/)
    {
        return TEST_SUCCESS;
    }
};


ButtonTest buttonTest;
IgniteRetractTest igniteRetractTest;
ChannelTest channelTest;
BlasterTest blasterTest;
AccelerometerTest accelerometerTest;

Test* gTests[] = {
    &buttonTest,
    &igniteRetractTest,
    &channelTest,
    &blasterTest,
    &accelerometerTest,
    0
};

Tester::Tester()
{
    s_instance = this;
    button = 0;
    running = false;
}

void Tester::attach(Button* buttonA)
{
    button = buttonA;
}

void Tester::runTests()
{
    Log.attachSerial(&Serial);
    runUnitTests();

    Log.p("Features:").eol();
    Log.p("Accel: ").p(STRINGIZE(ACCELEROMETER)).eol();
    //Log.p("Magnetometer: ").p(accelMag.hasMag()).eol();
    Log.p("Tune: ").p(VOLTMETER_TUNE).eol();
    Log.p("Lock: ").p(SABER_LOCK()).eol();

    running = true;
    if (button) 
        button->enableTestMode(true);
    currentTest = 0;
    start();
}

void Tester::start()
{
    Log.attachSerial(&Serial);
    Test* test = gTests[currentTest];
    ASSERT(test);

    TEST_EXISTS(button != 0);
    wasOn = false;

    Log.p("Test start: '").p(test->name()).p("'").eol();
    lastProcessTime = millis();
    test->start(this);
}

bool Tester::checkOnOff()
{
    if (!wasOn && bladePWM->getRGB().get() != 0) {
        // Log.p("wasOn=true").eol();
        wasOn = true;
    }
    bool isOff = bladePWM->getRGB().get() == 0;
    //Log.p("RGB=").ptc(blade->getColor()).eol();
    //if (wasOn && isOff)
    //    Log.p("checkOnOff true").eol();
    return wasOn && isOff;
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
            if (button) 
                button->enableTestMode(false);
        }
        Log.p("---- Test run complete. -----").eol();
#if SERIAL_DEBUG == 0
        Log.attachSerial(0);
#endif
    }
}

void Tester::process()
{
    if (!running) {
        return;
    }

    Test* test = gTests[currentTest];
    ASSERT(test);

    uint32_t m = millis();
    uint32_t delta = m - lastProcessTime;
    lastProcessTime = m;

    int event = uint32_t(-1);
    if (delta > 0 && !actionQueue.empty()) {
        Action& action = actionQueue.front();
        if (!action.init) {
            switch(action.type) {
            case ACTION_BUTTON:
                // Log.p("init: ACTION_BUTTON press.").eol();
                button->testPress();
                break;

            default:
                break;
            }
            action.init = true;
        }
        else if (action.type == ACTION_EVENT) {
            // Log.p("init: ACTION_EVENT ").p(action.time).eol();
            event = action.time;
            actionQueue.pop();
        }
        else if (delta >= action.time) {
            switch (action.type) {
            case ACTION_BUTTON:
                // Log.p("ACTION_BUTTON release.").eol();
                button->testRelease();
                break;
            
            default:
                break;
            }
            // Log.p("action pop=").p(action.type).eol();
            actionQueue.pop();
        }
        else {
            action.time -= delta;
        }
    }

    int result = test->process(this, event);

    if (result == Test::TEST_ERROR) {
        Log.p("Test ERROR: '").p(test->name()).p("'").eol();
        done();
    }
    else if (result == Test::TEST_SUCCESS) {
        Log.p("Test PASS: '").p(test->name()).p("'").eol();
        done();
    }
}

void Tester::holdButton()
{
    Action action(ACTION_BUTTON, HOLD_TIME);
    actionQueue.push(action);
}

void Tester::tapButton()
{
    Action action(ACTION_BUTTON, PRESS_TIME);
    actionQueue.push(action);
}

void Tester::delay(uint32_t t)
{
    Action action(ACTION_WAIT, t);
    actionQueue.push(action);
}

void Tester::sendEvent(uint32_t t)
{
    Action action(ACTION_EVENT, t);
    actionQueue.push(action);
}

