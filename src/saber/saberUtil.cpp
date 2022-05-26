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

void BladeState::change(uint8_t state)
{
    ASSERT(state >= BLADE_OFF && state <= BLADE_RETRACT);
    m_currentState = state;
    m_startTime = millis();
}

bool BladeState::bladeOn() const
{
    ASSERT(m_currentState >= BLADE_OFF && m_currentState <= BLADE_RETRACT);
    return m_currentState >= BLADE_ON && m_currentState < BLADE_RETRACT;
}

void BladeState::processUI(BladePWM *blade, const BladeFlash& /*saber*/, uint32_t /*time*/, UIMode mode, int vol4, osbr::RGB color)
{
    if (mode == UIMode::PALETTE) {
        blade->setRGB(color);
    }
    else if (mode == UIMode::VOLUME) {
        osbr::RGB c;
        c.r = 128 * vol4 * vol4 / 16;
        c.g = 128 * vol4 * vol4 / 16;
        c.b = 128 * (4 - vol4) / 4;
        blade->setRGB(c);
    }
    else {
        blade->setRGB(osbr::RGB::BLACK);
    }
}

void BladeState::process(BladePWM *blade, const BladeFlash &saber, uint32_t /*time*/)
{
    SFX *sfx = SFX::instance();

    switch (state())
    {
    case BLADE_OFF:
        break;

    case BLADE_ON:
        blade->setRGB(saber.getColor());
        break;

    case BLADE_IGNITE:
    {
        uint32_t igniteTime = sfx->getIgniteTime();
        uint32_t t = millis() - startTime();

        if (t >= igniteTime)
        {
            blade->setRGB(saber.getColor());
            change(BLADE_ON);
        }
        else
        {
            RGB c = RGB::lerp1024(RGB(RGB::BLACK), saber.getColor(), t * 1024 / igniteTime);
            blade->setRGB(c);
        }
    }
    break;

    case BLADE_RETRACT:
    {
        uint32_t retractTime = sfx->getRetractTime();
        uint32_t t = millis() - startTime();

        if (t >= retractTime)
        {
            blade->setRGB(RGB(RGB::BLACK));
            change(BLADE_OFF);
        }
        else
        {
            RGB c = RGB::lerp1024(saber.getColor(), RGB(RGB::BLACK), t * 1024 / retractTime);
            blade->setRGB(c);
        }
    }
    break;

    default:
        ASSERT(false);
        break;
    }
}

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
