#pragma once

#include <stdint.h>
#include "rgb.h"

enum class BladeState
{
    kOff,
    kIgnite,
    kOn,
    kRetract
};

// The input is the color: main color, impact color, interpolation between.
// Then this as adjusted for ignite / retract.
class BladeStateManager
{
public:
    void ignite(uint32_t igniteTime);
    void retract(uint32_t retractTime);

    BladeState state() const { return m_state; }

    bool isOff() const { return m_state == BladeState::kOff; }
    bool canIgnite() const { return m_state == BladeState::kOff; }
    bool canRetract() const { return m_state == BladeState::kOn; }

    // In: fullColor: the color of the blade if full on. Can be
    //                impact or normal color. 
    // In: msec:      the current time (not delta) in milliseconds
    // Returns:       the blade color, account for on/off/ignite/retract
    osbr::RGB process(osbr::RGB fullColor, uint32_t currentTimeMsec);

    static bool Test();

private:
    BladeState m_state = BladeState::kOff;
    uint32_t m_startTime = 0;
    uint32_t m_eventTime = 0;
    uint32_t m_currentTime = 0;
};