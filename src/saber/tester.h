#ifndef SABER_TESTER_INCLUDED
#define SABER_TESTER_INCLUDED

#include <stdint.h>
#include "Grinliz_Arduino_Util.h"

class Button;
struct ButtonCBHandlers;
class Tester;
class SaberDB;
class BladeFlash;

namespace osbr {
	struct RGB;
}

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
	virtual int process(Tester* tester, EventQueue* queue) = 0;

protected:
};


class Tester
{
public:
	Tester();
	void attach(Button* buttonA, Button* buttonB);
	void attachDB(SaberDB* _saberDB, BladeFlash* _bladeFlash) { saberDB = _saberDB; bladeFlash = _bladeFlash; }

	void runTests();
	void process();
	static Tester* instance() { return s_instance; }

	// interface for Test classes to call:
	void delay(uint32_t time);
	void fire(const char* event);
	void press(int button, uint32_t time);
	void delayedPress(int button, uint32_t wait, uint32_t time);

	// queue interface
	void holdButton();
	void tapButton();
	void delay(uint32_t t);
	
	SaberDB* getSaberDB() { return saberDB; }
	BladeFlash* getBladeFlash() { return bladeFlash; }

	int getOrder() const { return order; }
	void incrementOrder() { order++; }

private:	
	void start();
	void done();

	enum {
		TEST_STATE_NONE,
		TEST_STATE_START,
		TEST_STATE_RUN,
		TEST_STATE_DONE
	};

	enum {
		ACTION_NONE,
		ACTION_BUTTON,
		ACTION_WAIT
	};

	struct Action {
		Action() {}
		Action(int type, int time) {
			this->type = type;
			this->time = time;
		}
		bool init = false;
		int type = 0;
		uint32_t time = 0;
	};

	CQueue<Action> actionQueue;

	int currentTest = 0;
	bool running = false;
	uint32_t delayTime = 0;
	SaberDB* saberDB = 0;
	BladeFlash* bladeFlash = 0;
	int order = 0;
	uint32_t lastProcessTime = 0;

	struct Press {
		uint32_t start;
		uint32_t end;
		void reset() { start = end = 0; }
	};
	Press pressState;
	Button* button;

	static Tester* s_instance;
};

#endif // SABER_TESTER_INCLUDED
