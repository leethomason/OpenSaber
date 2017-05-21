#ifndef SABER_TESTER_INCLUDED
#define SABER_TESTER_INCLUDED

#include <stdint.h>
#include "Grinliz_Arduino_Util.h"

class Button;
struct ButtonCBHandlers;
class Tester;
struct RGB;

class Test
{
public:
	enum {
		TEST_CONTINUE,
		TEST_ERROR,
		TEST_SUCCESS,
	};

	virtual const char* name() const = 0;
	virtual void start(Tester* tester)	{}
	virtual int process(Tester* tester, const char* event, const char* eventData) = 0;
	virtual bool buttonRelease(Tester* tester, int sequence) { return false; }

	int finalResult = 0;

protected:
};


class Tester
{
public:
	Tester();
	void attach(Button* buttonA, Button* buttonB);
	void attachUI(const RGB* uiLEDs) { leds = uiLEDs; }

	void runTests(int count = 1, bool longTestTime=false);
	void process();
	static Tester* instance() { return s_instance; }

	// interface for Test classes to call:
	void delay(uint32_t time);
	void fire(const char* event);
	void press(int button, uint32_t time);
	void delayedPress(int button, uint32_t wait, uint32_t time);
	void checkAudio(const char* name, uint32_t low, uint32_t high);
	uint32_t getRandom() { return r.rand16(); }
	bool longTest() const { return useLongTest; }
	const RGB* getLEDs() const { return leds; }

	int order = 0;

private:	
	void start();
	Test* done();

	enum {
		TEST_STATE_NONE,
		TEST_STATE_START,
		TEST_STATE_RUN,
		TEST_STATE_DONE
	};

	int currentTest = 0;
	bool running = false;
	int testState = 0;
	uint32_t delayTime = 0;
	int passCount = 0;
	int nPasses = 1;
	bool useLongTest;
	int nButtonCalls = 0;
	const RGB* leds = 0;

	uint32_t testAudio = 0;
	const char* audioName = 0;
	uint32_t audioLow = 0;
	uint32_t audioHigh = 0;

	struct Press {
		uint32_t start;
		uint32_t end;
		void reset() { start = end = 0; }
	};
	Random r;
	Press pressState[2];
	Button* button[2];

	static Tester* s_instance;
};

#endif // SABER_TESTER_INCLUDED
