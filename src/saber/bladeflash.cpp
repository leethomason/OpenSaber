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

#include "bladeflash.h"
#include "./src/util/Grinliz_Util.h"
#include "rgb2hsv.h"
#include "./src/util/fixed.h"
#include "rgb.h"

void BladeFlash::tick(uint32_t msec)
{
    if (msec - tStart >= LEN) {
        color = bladeColor;
    }
    else {
#if false
        int t = int(msec - tStart) * 1024 / LEN;
#else
        FixedNorm fn(msec - tStart, LEN*4);
        FixedNorm sn = iSin(fn);
        int t = sn.scale(1024);
#endif
        color.r = lerp1024(impactColor.r, bladeColor.r, t);
        color.g = lerp1024(impactColor.g, bladeColor.g, t);
        color.b = lerp1024(impactColor.b, bladeColor.b, t);
    }
}


osbr::RGB ColorRotated(const osbr::RGB& rgb, int angle360)
{
    uint8_t h, s, v;
    rgb2hsv(rgb.r, rgb.g, rgb.b, &h, &s, &v);
    h += angle360 / 2;
    osbr::RGB c;
    hsv2rgb(h, s, v, &c.r, &c.g, &c.b);
    return c;
}



osbr::RGB AccelToColor(FixedNorm x, FixedNorm z)
{
    // Blade direction.
    // -1  -> 0 -> 1
    FixedNorm angle0 = iInvSin(z);   // -90 -> 0 -> 90

                                     // Normal
                                     // 0   -> 1 -> 0
    FixedNorm angle1 = iInvCos(x);   // 90  -> 0 -> 90

    if (angle0 < 0)
        angle1 *= -1;

    // Angle from -90 to 90 (normal)
    FixedNorm angle = lerp(angle0, angle1, z.absolute());

    // Shift to 0 to 180 (normal), which is 0.0 to 0.5
    FixedNorm theta = FixedNorm(1, 4) - angle;

    osbr::RGB rgb;
    hsv2rgb(theta.scale(360), 255, 255, &rgb.r, &rgb.g, &rgb.b);
    return rgb;
}


bool TestAccelToColor()
{
    osbr::RGB rgb;

    // start out with basics.
    hsv2rgb(0, 255, 255, &rgb.r, &rgb.g, &rgb.b);
    TEST_IS_TRUE(rgb == osbr::RGB(255, 0, 0));

    hsv2rgb(90/2, 255, 255, &rgb.r, &rgb.g, &rgb.b);
    TEST_IS_TRUE(rgb == osbr::RGB(128, 255, 0));

    hsv2rgb(180 / 2, 255, 255, &rgb.r, &rgb.g, &rgb.b);
    TEST_IS_TRUE(rgb == osbr::RGB(0, 255, 255));

    hsv2rgb(270 / 2, 255, 255, &rgb.r, &rgb.g, &rgb.b);
    TEST_IS_TRUE(rgb == osbr::RGB(128, 0, 255));

    // z as blade: straight down (-1) to straight up (1)
    rgb = AccelToColor(0, 1);  // 0 -> 0
    TEST_IS_TRUE(rgb == osbr::RGB(255, 0, 0));

    rgb = AccelToColor(0, -1);  // 180 -> 360
    TEST_IS_TRUE(rgb == osbr::RGB(255, 0, 0));

    rgb = AccelToColor(1, 0);   // 90 -> 180
    TEST_IS_TRUE(rgb == osbr::RGB(0, 255, 255));

    rgb = AccelToColor(fixedNormSqrt2Over2, fixedNormSqrt2Over2); // 45 -> 90
    TEST_IS_TRUE(rgb == osbr::RGB(128, 255, 0));                

    rgb = AccelToColor(fixedNormSqrt2Over2, -fixedNormSqrt2Over2); // 135 -> -90
    TEST_IS_TRUE(rgb == osbr::RGB(128, 0, 255));

    return true;
}

