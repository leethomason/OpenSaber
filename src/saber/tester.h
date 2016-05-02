#ifndef SABER_TESTER_INCLUDED
#define SABER_TESTER_INCLUDED

class Button;
struct ButtonCBHandlers;

class Tester
{
public:
	Tester();
	void attach(const Button* buttonA, const Button* buttonB);
	void start();

	void process();

	bool igniteHumRetract();

	static Tester* instance() { return s_instance; }

private:	
	enum {
		TEST_STATE_NONE,
		TEST_STATE_START,
		TEST_STATE_RUN,
		TEST_STATE_DONE
	};
	int testState = 0;
	const Button* buttonA = 0;
	const Button* buttonB = 0;
	const ButtonCBHandlers* buttonACB = 0;
	const ButtonCBHandlers* buttonBCB = 0;

	static Tester* s_instance;
};

#endif // SABER_TESTER_INCLUDED
