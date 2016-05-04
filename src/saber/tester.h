#ifndef SABER_TESTER_INCLUDED
#define SABER_TESTER_INCLUDED

#include <stdint.h>
#include <Grinliz_Arduino_Util.h>

class Button;
struct ButtonCBHandlers;
class Tester;

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

	int finalResult = 0;

protected:
};


class Tester
{
public:
	Tester();
	void attach(Button* buttonA, Button* buttonB);

	void runTests();
	void process();
	static Tester* instance() { return s_instance; }

	// interface for Test classes to call:
	void delay(uint32_t time);
	void fire(const char* event);
	void press(int button, uint32_t time);
	void delayedPress(int button, uint32_t wait, uint32_t time);
	void checkAudio(const char* name, uint32_t low, uint32_t high);
	uint32_t getRandom() { return r.rand16(); }

	int order = 0;

private:	
	void start();
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

	uint32_t testAudio = 0;
	const char* audioName = 0;
	uint32_t audioLow = 0;
	uint32_t audioHigh = 0;

	struct Press {
		uint32_t start;
		uint32_t end;
	};
	Random r;
	Press pressState[2];
	Button* button[2];

	static Tester* s_instance;
};

#endif // SABER_TESTER_INCLUDED
