#pragma once
#include <stdint.h>
#include "rgb.h"



struct UIRenderData
{
    uint8_t volume = 0;     // 0-4
    uint8_t palette = 0;    // 0-7
    uint32_t mVolts = 0;    // actual voltage in milli-volts
    const char* fontName = 0;

    osbr::RGB color;	// NOT the RGB of the LED. An GGB LED would be
                        // green if set to (1, 0, 0), so the bladeColor
                        // should be (0, 1, 0)

    int powerLevel(int maxLevel) const {
        static const int32_t NOMINAL = 3700;
        static const int32_t VARIATION = 400;
        static const int32_t HIGH_VOLTAGE = NOMINAL + VARIATION;
        static const int32_t LOW_VOLTAGE = NOMINAL - VARIATION;

        int32_t level = maxLevel * (mVolts - LOW_VOLTAGE) / (HIGH_VOLTAGE - LOW_VOLTAGE);
        if (level < 0) level = 0;
        if (level > maxLevel) level = maxLevel;
        return level;
    }

    UIRenderData() {}
};
