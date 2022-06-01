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

#ifndef SABER_TESTER_INCLUDED
#define SABER_TESTER_INCLUDED

#include <stdint.h>
#include "./src/util/Grinliz_Arduino_Util.h"

class Button;
struct ButtonCBHandlers;
class Tester;
class SaberDB;
class BladePWM;
class BladeColor;

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
	virtual void start(Tester*)	{}
	virtual int process(Tester*, uint32_t /*event*/) = 0;

protected:
};


class Tester
{
public:
	Tester();
	void attach(Button* buttonA);
	void attachDB(SaberDB* _saberDB, BladePWM* _blade, BladeColor* _bladeFlash) { saberDB = _saberDB; bladePWM = _blade; bladeFlash = _bladeFlash;}

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
	BladeColor* getBladeColor() { return bladeFlash;}

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
	BladeColor* bladeFlash = 0;
	Button* button;

	static Tester* s_instance;
};

#endif // SABER_TESTER_INCLUDED
