#ifndef SABER_TESTER_INCLUDED
#define SABER_TESTER_INCLUDED

#include <stdint.h>

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

protected:
	void assertButtonA(Tester*);
};

class Tester
{
	friend class Test;
	friend class IgniteRetractTest;

public:
	Tester();
	void attach(const Button* buttonA, const Button* buttonB);
	void start();

	void process();
	static Tester* instance() { return s_instance; }

	// interface for Test classes to call:
	void delay(uint32_t time);
	void fire(const char* event);

private:	
	enum {
		TEST_STATE_NONE,
		TEST_STATE_START,
		TEST_STATE_RUN,
		TEST_STATE_DONE
	};

	int testState = 0;
	Test* test = 0;
	uint32_t delayTime = 0;
	const char* testEvent = 0;

	const Button* buttonA = 0;
	const Button* buttonB = 0;
	const ButtonCBHandlers* buttonACB = 0;
	const ButtonCBHandlers* buttonBCB = 0;	

	static Tester* s_instance;
};

#endif // SABER_TESTER_INCLUDED
