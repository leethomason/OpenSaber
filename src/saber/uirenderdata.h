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

#pragma once
#include <stdint.h>
#include "rgb.h"
#include "Grinliz_Util.h"

struct UIRenderData
{
    uint8_t volume = 0;     // 0-4
    uint8_t palette = 0;    // 0-7
    uint32_t mVolts = 0;    // actual voltage in milli-volts
    int soundBank = 0;
    CStr<10> fontName;

    osbr::RGB color;	// NOT the RGB of the LED. An GGB LED would be
                      // green if set to (1, 0, 0), so the bladeColor
                      // should be (0, 1, 0)

    static int powerLevel(uint32_t mVolts, int maxLevel) {
        static const int32_t HIGH_VOLTAGE = NOMINAL_VOLTAGE + VOLTAGE_RANGE;
        static const int32_t LOW_VOLTAGE = NOMINAL_VOLTAGE - VOLTAGE_RANGE;

        int32_t level = maxLevel * (mVolts - LOW_VOLTAGE) / (HIGH_VOLTAGE - LOW_VOLTAGE);
        if (level < 0) level = 0;
        if (level > maxLevel) level = maxLevel;
        return level;
    }

    UIRenderData() {}
};
