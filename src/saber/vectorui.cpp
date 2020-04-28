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

#include "vectorui.h"
#include "vrender.h"
#include "Grinliz_Util.h"
#include "rgb2hsv.h"
#include "renderer.h"   // for drawing the bitmap directly to framebuffer
#include "assets.h"     // source for bitmap


void VectorUI::Segment(VRender* ren, int width, int s, int num, int rgba)
{
    /*
        00
       5  1
       5  1
        66
       4  2
       4  2
        33    7
    */

    if (num < 0) num = 0;
    if (num > 9) num = 9;

    static const uint8_t segments[10] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f };

    uint8_t bit = segments[num];

    if (bit & (1 << 0))
        ren->DrawRect(0, 0, width, s, rgba);
    if (bit & (1 << 1))
        ren->DrawRect(width - s, 0, s, width, rgba);
    if (bit & (1 << 2))
        ren->DrawRect(width - s, width, s, width, rgba);
    if (bit & (1 << 3))
        ren->DrawRect(0, width * 2 - s, width, s, rgba);
    if (bit & (1 << 4))
        ren->DrawRect(0, width, s, width, rgba);
    if (bit & (1 << 5))
        ren->DrawRect(0, 0, s, width, rgba);
    if (bit & (1 << 6))
        ren->DrawRect(0, width - s / 2, width, s, rgba);
}

void VectorUI::DrawBar(VRender* ren, int x, int y, int width, int color, int fraction)
{
    int w = (width * fraction) >> 8;

    if (w == 0 || fraction == 0) {
        ren->DrawRect(x, y, width, 1, color);
    }
    else if (w == width || fraction == 255) {
        ren->DrawRect(x, y - 1, width, 3, color);
    }
    else {
        ren->DrawRect(x, y - 1, w, 3, color);
        ren->DrawRect(x + w, y, width - w, 1, color);
    }
}

void VectorUI::DrawMultiBar(VRender* ren, int x, bool flip, int yCutoff)
{
    int bias = flip ? -1 : 1;

    for (int r = 0; r < 8; ++r) {
        Fixed115 d = r - Fixed115(7, 2);
        Fixed115 fx = Fixed115(8, 10) * d * d;
        DrawBar(ren, x + fx.getInt() * bias , 29 - r * 4, BAR_W, 1, r < yCutoff ? 255 : 0);
    }

}


void VectorUI::DrawColorHSV(VRender* ren, int x, int h)
{
    static const int WHITE = 1;

    static const FixedNorm rotations[6] = {
        FixedNorm(0, 6), FixedNorm(1, 6), FixedNorm(2, 6),
        FixedNorm(3, 6), FixedNorm(4, 6), FixedNorm(5, 6)
    };

    for (int r = 0; r < 6; ++r) {
        ren->SetTransform(rotations[r], x, H / 2);
        ren->DrawRect(-1, 10, 2, 5, WHITE);
    }

    ren->SetTransform(FixedNorm(h, 180), x, H / 2);
    ren->DrawRect(-2, 0, 4, 12, WHITE);

    ren->ClearTransform();
}


void VectorUI::Draw(VRender* ren,
    uint32_t time,
    UIMode mode,
    bool bladeIgnited,
    const UIRenderData* data,
    uint8_t* pixels)
{
#if true
    ren->DrawRect(2, 2, 10, 10, 1);
    ren->DrawRect(20, 2, 10, 10, 1, 2);

    ren->SetTransform(FixedNorm(time % 8000, 8000), 50, 6);
    ren->DrawRect(-5, -5, 10, 10, 1, 2);
#else
    static const int WHITE = 1;

    if (lastTime == 0) lastTime = time;
    uint32_t deltaTime = time - lastTime;
    lastTime = time;

    ren->Clear();
  
    int p = UIRenderData::powerLevel(data->mVolts, 8);
    CStr<5> volts;
    volts.setFromNum(data->mVolts, true);

    uint8_t h, s, v;
    rgb2hsv(data->color.r, data->color.g, data->color.b, &h, &s, &v);

    // Power
    if (mode == UIMode::NORMAL) {
        DrawMultiBar(ren, 0, false, p);
    }
    else if (mode == UIMode::PALETTE) {
        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < 4; i++) {
                static const int S = 6;
                int count = j * 4 + i;
                ren->DrawRect(S + (S + 2)*i, H / 2 - 8 + 8 * j, S, S, WHITE, data->palette == count ? 1 : 0);
            }
        }
    }
    else if (mode == UIMode::VOLUME) {
        static const int S = 10;
        static const int XS = 12;
        ren->DrawRect(XS, H / 2 - S / 2, S, S, WHITE);
        for (int i = 2; i < 5; ++i) {
            ren->DrawRect(XS + 4 + 4 * i, H / 2 - (S + i * 4) / 2, 2, S + i * 4, WHITE);
        }
    }
    else if (mode == UIMode::MEDITATION) {
        if (pixels) {
            Renderer renderer;
            renderer.Attach(W, H, pixels);
            renderer.DrawBitmap(8, 0, get_jBird);
        }
    }
    else if (mode == UIMode::COLOR_WHEEL) {
        static const int S = 24;
        for (int i = 0; i < 6; ++i) {
            ren->SetTransform(FixedNorm(i * 30, 180), W / 4 - 6, H / 2);
            ren->DrawRect(-S / 2, -1, S, 2, WHITE);
        }
        ren->ClearTransform();
        ren->DrawRect(W / 4 - 6 - 10, H / 2 - 10, 20, 20, WHITE, 1);
    }

    // Audio
    if (mode == UIMode::NORMAL || mode == UIMode::VOLUME) {
        DrawMultiBar(ren, W - BAR_W, true, data->volume * 2);
    }
    // Color indicator
    {
        
        if (currentH < h) currentH++; // += deltaTime / 10;
        if (currentH > h) currentH--; // -= deltaTime / 10;
        DrawColorHSV(ren, W / 2, currentH);
        /*
        for (int i = 0; i < 3; ++i) {
            int y = H / 2 - 2 - 6 + 6 * i + 1;
            DrawBar(ren, W / 2 + 19, y, 12, WHITE, data->color[i]);
        }

        int digits[4];
        intToDigits(data->palette, digits, 4);
        ren->SetTransform(W - 30, H / 2 - TEXT);
        Segment(ren, TEXT, 2, digits[3], WHITE);
        */
    }

    // Power
    if (mode == UIMode::NORMAL) {
        int digits[4];
        intToDigits(data->mVolts, digits, 4);
        for (int i = 0; i < 4; ++i) {
            ren->SetTransform(20 + (TEXT+2)*i, H / 2 - TEXT);
            Segment(ren, TEXT, 2, digits[i], WHITE);
        }
    }
    // Call in wrapper!
    //ren->Render();
#endif
}
