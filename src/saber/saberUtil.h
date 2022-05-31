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

#ifndef SABER_UTIL_INCLUDED
#define SABER_UTIL_INCLUDED

#include <stdint.h>
/* Don't include pins.h, etc. Keep this header (but not cpp)
   cross platform so that it can be included with test / debug
   code.
*/
//#include "pins.h" 
#include "modes.h"
#include "rgb.h"
#include "./src/util/Grinliz_util.h"

class BladePWM;
class BladeFlash;

// Update everything from a DB change.
void syncToDB();

class UIModeUtil
{
public:
    static const uint32_t IDLE_TIME = 10 * 1000;

	UIModeUtil();

	void set(UIMode mode) { m_mode = mode; }
    void nextMode();

    void setActive();
    bool isIdle() const;
    uint32_t millisPastIdle() const;
    
    UIMode mode() const {
        return m_mode;
    }

private:
    UIMode m_mode = UIMode::NORMAL;
    uint32_t lastActive = 0;
};

/*
    Implements a countdown:
    2 - 1 - click
    flash to retract the lightsaber in "locked" mode 
*/
class LockTimer
{
public:
    LockTimer();

    bool enabled() const;
    bool dark() const;

    void start();
    void tick(uint32_t delta);
    bool click();   // return true if it should retract

private:
    Timer2      timer;
    int         timerCount = 0;
};

#endif // SABER_UTIL_INCLUDED
