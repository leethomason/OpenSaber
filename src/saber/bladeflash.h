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

#include "rgb.h"
#include "fixed.h"

class BladeFlash
{
public:
    BladeFlash() {}

    void setBladeColor(const osbr::RGB& rgb) { bladeColor = rgb;  }
    void setImpactColor(const osbr::RGB& rgb) { impactColor = rgb; }

    void tick(uint32_t msec);

    void doFlash(uint32_t msec) { tStart = msec; }

    osbr::RGB getColor() const { return color;  }

private:
    static const uint32_t LEN = 500;
    uint32_t tStart = 0;
    osbr::RGB bladeColor;
    osbr::RGB impactColor;
    osbr::RGB color;
};

// Could be someplace better, but shared code (with win32) in this location.
osbr::RGB AccelToColor(FixedNorm x, FixedNorm z);
osbr::RGB ColorInverse(const osbr::RGB& rgb);

bool TestAccelToColor();

