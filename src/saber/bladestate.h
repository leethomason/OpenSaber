#pragma once

#include <stdint.h>
#include "rgb.h"

enum class BladeState2
{
    kOff,
    kIgnite,
    kOn,
    kRetract
};

class BladeStateManager
{
public:
    void ignite(uint32_t igniteTime);
    void retract(uint32_t retractTime);

    BladeState2 bladeState() const { return m_state; }
    bool isOff() const { return m_state == BladeState2::kOff; }

    // In: fullColor: the color of the blade if full on. Can be
    //                impact or normal color. 
    // In: msec:      the current time (not delta) in milliseconds
    // Returns:       the blade color, account for on/off/ignite/retract
    osbr::RGB process(osbr::RGB fullColor, uint32_t currentTimeMsec);

    static bool Test();

private:
    BladeState2 m_state = BladeState2::kOff;
    uint32_t m_startTime = 0;
    uint32_t m_eventTime = 0;
    uint32_t m_currentTime = 0;
};