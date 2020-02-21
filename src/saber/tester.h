#ifndef SABER_TESTER_INCLUDED
#define SABER_TESTER_INCLUDED

#include <stdint.h>
#include "Grinliz_Arduino_Util.h"

class Button;
struct ButtonCBHandlers;
class Tester;
class SaberDB;
class BladePWM;
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
	virtual int process(Tester* tester, uint32_t event) = 0;

protected:
};


class Tester
{
public:
	Tester();
	void attach(Button* buttonA);
	void attachDB(SaberDB* _saberDB, BladePWM* _blade, BladeFlash* _bladeFlash) { saberDB = _saberDB; bladePWM = _blade; bladeFlash = _bladeFlash;}

	void runTests();
	void process();
	static Tester* instance() { return s_instance; }

	// queue interface
	void holdButton();
	void tapButton();
	void delay(uint32_t t);
	void sendEvent(uint32_t event);
	
	SaberDB* getSaberDB() { return saberDB; }
	BladePWM* getBladePWM() { return bladePWM; }
	BladeFlash* getBladeFlash() { return bladeFlash;}

	// check the common test termination - returns true
	// if the blade has turned on and then back off again.
	bool checkOnOff();

private:	
	void start();
	void done();

	enum {
		ACTION_NONE,
		ACTION_BUTTON,
		ACTION_WAIT,
		ACTION_EVENT
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

	CQueue<Action, 16> actionQueue;

	int currentTest = 0;
	bool running = false;
	bool wasOn = false;
	uint32_t lastProcessTime = 0;

	SaberDB* saberDB = 0;
	BladePWM* bladePWM = 0;
	BladeFlash* bladeFlash = 0;
	Button* button;

	static Tester* s_instance;
};

#endif // SABER_TESTER_INCLUDED
