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

#include "saberUtil.h"
#include "bladePWM.h"
#include "sfx.h"
#include "bladeflash.h"

using namespace osbr;

UIModeUtil::UIModeUtil()
{
    lastActive = millis();
}

void UIModeUtil::setActive()
{
    lastActive = millis();
}

uint32_t UIModeUtil::millisPastIdle() const {
    if (isIdle()) {
        return millis() - (lastActive + IDLE_TIME);
    }
    return 0;
}

bool UIModeUtil::isIdle() const
{
    if (millis() - lastActive > IDLE_TIME)
        return true;
    return false;
}

void UIModeUtil::nextMode()
{
    m_mode = (UIMode)((int)m_mode + 1);
    if(m_mode == UIMode::NUM_MODES)
        m_mode = UIMode::NORMAL;

    switch (m_mode)
    {
    case UIMode::NORMAL:
        Log.p("mode: NORMAL").eol();
        break;

    case UIMode::PALETTE:
        Log.p("mode: PALETTE").eol();
        break;

    case UIMode::VOLUME:
        Log.p("mode: VOLUME").eol();
        break;

#if SABER_LOCK()
    case UIMode::LOCK:
        Log.p("mode: LOCK").eol();
        break;
#endif        

    default:
        ASSERT(false);
        m_mode = UIMode::NORMAL;
        break;
    }
}


LockTimer::LockTimer()
{
    timer.setEnabled(false);
}

bool LockTimer::enabled() const
{
    return timer.enabled();
}

bool LockTimer::click()
{
    //Log.p("click enabled=").p(timer.enabled()).p( " count=").p(timerCount).eol();
    bool retract = false;
    if (timerCount >= 4) {
        retract = true;
    }
    timerCount = 0;
    timer.setEnabled(false);
    return retract;
}

void LockTimer::start()
{
    timer.reset();
    timer.setPeriod(250);
    timer.setRepeating(true);
    timer.setEnabled(true);
    timerCount = 0;
}

void LockTimer::tick(uint32_t delta)
{
    timerCount += timer.tick(delta);
    if (timerCount >= 6) {
        timer.setEnabled(false);
        timerCount = 0;
    }
}

bool LockTimer::dark() const
{
    return timer.enabled() && !(timerCount & 1);
}
