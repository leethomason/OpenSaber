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
#include "pins.h"

enum class UIMode {
    NORMAL,
    PALETTE,
    VOLUME,
#if SABER_UI_MEDITATION()
    MEDITATION,
#endif
#if SABER_UI_COLOR_WHEEL()
    COLOR_WHEEL,  // can't have WHEEL and TUNE
#endif
    NUM_MODES,

#if !SABER_UI_MEDITATION()
    MEDITATION = 255,
#endif
#if !SABER_UI_COLOR_WHEEL()
    COLOR_WHEEL = 256,  // can't have WHEEL and TUNE
#endif
};

enum {
    BLADE_OFF,
    BLADE_IGNITE,
    BLADE_ON,
    BLADE_RETRACT
};


