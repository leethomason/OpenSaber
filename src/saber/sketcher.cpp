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

#include "sketcher.h"
#include "rgb2hsv.h"
#include "./src/util/fixed.h"
#include "./src/util/Grinliz_Util.h"

using namespace osbr;

void calcCrystalColorHSV(uint32_t msec, const osbr::RGB &base, osbr::RGB *out)
{
    uint8_t h = 0, s = 0, v = 0;
    rgb2hsv(base.r, base.g, base.b, &h, &s, &v);

    static const uint32_t BREATH_CYCLE = 13 * 1000;     // milliseconds to cycle
    static const uint32_t HUE_CYCLE    = 23 * 1000;     // milliseconds to cycle
    static const uint32_t SAT_CYCLE    = 47 * 1000;     // milliseconds to cycle

    // "breathing" - v
    {
        static const int32_t VARIATION = 96;
        static const int32_t BASE = 256 - VARIATION * 2;

        Fixed610 dt = divToFixed<Fixed610>(msec % BREATH_CYCLE, BREATH_CYCLE);
        Fixed610 var = sin(dt * Fixed610{k2Pi_float});
        int32_t v32 = BASE + VARIATION + scale(var, VARIATION);
        v = uint8_t(glClamp(v32, int32_t(0), int32_t(255)));
    }
    // Hue
    {
        static const int HUE_VAR = 20;

        Fixed610 dt = divToFixed<Fixed610>(msec % HUE_CYCLE, HUE_CYCLE);
        Fixed610 var = sin(dt * Fixed610{k2Pi_float});
        int32_t hPrime = h + scale(var, HUE_VAR);
        while (hPrime >= 180)
            hPrime -= 180;
        while (hPrime < 0)
            hPrime += 180;
        h = uint8_t(hPrime);
    }

    // Saturation
    {
        static const int32_t SAT_VAR = 32;
        Fixed610 dt = divToFixed<Fixed610>(msec % SAT_CYCLE, SAT_CYCLE);
        Fixed610 var = sin(dt * Fixed610{k2Pi_float});
        int32_t sPrime = s - SAT_VAR / 2 + scale(var, SAT_VAR);
        if (sPrime < 0)
            sPrime = 0;
        if (sPrime > 255)
            sPrime = 255;
        s = sPrime;
    }
    hsv2rgb(h, s, v, &out->r, &out->g, &out->b);
}

#if false
Pixel_7_5_UI::Pixel_7_5_UI()
{
}

/*
    .x.X.x.
    x..X..x
    Xx.X.xX
    XX.X.XX
    .XXXXX.

    cols: 01110 0x0c
          11101 0x19
          10000 0x10
          11111 0x1f
*/
void Pixel_7_5_UI::Draw(uint32_t /*time*/, UIMode mode, bool /*bladeIgnited*/, const UIRenderData *data)
{
    for (int i = 0; i < ALLOCATED; ++i)
        m_col[i] = 0;

    switch (mode)
    {
    case UIMode::NORMAL:
        getGlypth_tomThumb5('0' + data->palette, m_col + 0);
        DrawBar(5, UIRenderData::powerLevel(data->mVolts, 4));
        DrawBar(6, data->volume);
        break;

    case UIMode::PALETTE:
        getGlypth_tomThumb5('0' + data->palette, m_col + 4);
        DrawBar(0, data->color.r / 50);
        DrawBar(1, data->color.g / 50);
        DrawBar(2, data->color.b / 50);
        break;

    case UIMode::VOLUME:
        getGlypth_tomThumb5('V', m_col);
        DrawBar(4, data->volume);
        DrawBar(5, data->volume);
        DrawBar(6, data->volume);
        break;

    case UIMode::MEDITATION:
        m_col[0] = m_col[6] = 0x0e;
        m_col[1] = m_col[5] = 0x19;
        m_col[2] = m_col[4] = 0x10;
        m_col[3] = 0x1f;
        break;

    default:
        break;
    }
}

void Pixel_7_5_UI::DrawBar(int x, int y)
{
    uint8_t *c = m_col + x;
    *c = 0;
    for (int i = 0; i < y; ++i)
    {
        *c |= (16 >> i);
    }
}

void Pixel_7_5_UI::DrawDot(int x, int y)
{
    m_col[x] |= (16 >> y);
}

void Digit4UI::Draw(UIMode mode, const UIRenderData *data)
{
    char scratch[6] = "     ";
    m_output.clear();
    uint32_t volts = data->mVolts / 1000;
    uint32_t deciVolts = (data->mVolts - volts * 1000) / 100;

    switch (mode)
    {
    case UIMode::NORMAL:
        scratch[0] = '0' + data->palette;
        scratch[1] = '0' + data->volume;
        scratch[2] = '0' + volts;
        scratch[3] = '.';
        scratch[4] = '0' + deciVolts;
        scratch[5] = 0;
        break;

    case UIMode::PALETTE:
        scratch[0] = '0' + data->palette;
        scratch[3] = 'P';
        scratch[4] = 0;
        break;

    case UIMode::VOLUME:
        scratch[1] = '0' + data->volume;
        scratch[3] = 'A';
        scratch[4] = 0;
        break;

    case UIMode::MEDITATION:
        scratch[0] = 'F';
        scratch[1] = 'O';
        scratch[2] = 'R';
        scratch[3] = 'C';
        scratch[4] = 0;
        break;

#if 0
    case UIMode::COLOR_WHEEL:
        scratch[0] = 'C';
        scratch[1] = 'L';
        scratch[2] = 'R';
        scratch[3] = 0;
        scratch[4] = 0;
        break;
#endif

    case UIMode::NUM_MODES:
    default:
        break;
    }

    m_output = scratch;
}
#endif