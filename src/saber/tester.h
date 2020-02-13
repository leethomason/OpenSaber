#ifndef SABER_TESTER_INCLUDED
#define SABER_TESTER_INCLUDED

#include <stdint.h>
#include "Grinliz_Arduino_Util.h"

class Button;
struct ButtonCBHandlers;
class Tester;
class SaberDB;
class Blade;
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
	void attachDB(SaberDB* _saberDB, Blade* _blade, BladeFlash* _bladeFlash) { saberDB = _saberDB; blade = _blade; bladeFlash = _bladeFlash;}

	void runTests();
	void process();
	static Tester* instance() { return s_instance; }

	// queue interface
	void holdButton();
	void tapButton();
	void delay(uint32_t t);
	void sendEvent(uint32_t event);
	
	SaberDB* getSaberDB() { return saberDB; }
	Blade* getBlade() { return blade; }
	BladeFlash* getBladeFlash() { return bladeFlash;}

	bool checkOnOff();

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
	SaberDB* saberDB = 0;
	Blade* blade = 0;
	BladeFlash* bladeFlash = 0;
	uint32_t lastProcessTime = 0;
	bool wasOn = false;
	Button* button;

	static Tester* s_instance;
};

#endif // SABER_TESTER_INCLUDED
