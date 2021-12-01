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
#include "./src/util/Grinliz_Util.h"
#include "rgb2hsv.h"
#include "renderer.h"   // for drawing the bitmap directly to framebuffer
#include "assets.h"     // source for bitmap


VectorUI::VectorUI()
{

}


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


void VectorUI::DrawBank(VRender* ren, int x, int y, int c)
{
    ren->DrawRect(x, y, 4, 4, 1, c);

    /*
    static const int OUT = 4;
    static const int IN = 1;
    static const int TRAVEL = OUT + IN;

    int left = IN;
    int right = OUT - IN;
    if (state < 100) {
        left = -OUT + TRAVEL * state / 100;
        right = left + OUT - IN * 2;
        if (left < 0) left = 0;
        if (right < 0) right = 0;
    }

    Vec2* points[] = {
        // rectangle
        {x,y}, {x + OUT, y}, {x + OUT, y + OUT}, {x, y + OUT}, {x, y},
        //
    };
    ren->DrawPoly(points, n, 1);*/
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
    
    const VRender::Vec2 p0[] = { {-4,2}, {0, 0}, {4,2}, {1,12}, {-1, 12}, VRender::VECEND };
    const VRender::Vec2 p1[] = { {-2,3}, {2,3}, {0, 10}, VRender::VECEND };
    const VRender::Vec2* p[] = { p0, p1 };

    ren->DrawPoly(p, 2, 1);
    ren->ClearTransform();
}


void VectorUI::DrawBanks(VRender* ren, int bank)
{
    bank = bank % 4;
    DrawBank(ren, W - BAR_W - 13, H / 2 - 5, bank == 0 ? 1 : 0);
    DrawBank(ren, W - BAR_W - 8, H / 2 - 5, bank == 1 ? 1 : 0);
    DrawBank(ren, W - BAR_W - 13, H / 2 - 0, bank == 2 ? 1 : 0);
    DrawBank(ren, W - BAR_W - 8, H / 2 - 0, bank == 3 ? 1 : 0);
}


int VectorUI::ValueTo32(float value, float minRange, float maxRange)
{
    float fy = (value - minRange) / (maxRange - minRange);
    if (fy < 0) fy = 0;
    if (fy > 1) fy = 1;
    int y = int(fy * 32.0f);
    return y;
}


void VectorUI::PushTestData(float value, float minRange, float maxRange, uint32_t time, float bar)
{
    int y = ValueTo32(value, minRange, maxRange);
    testBar = 0;
    if (bar != 0.0f) {
        testBar = ValueTo32(bar, minRange, maxRange);
    }
    hasTestData = true;

    int bucket = int((time / BUCKET_TIME) % 128);
    if (testBucket != bucket) {
        testData[bucket] = 0;
        static const int CLEAR = 8;
        for (int i = bucket + 1; i < bucket + 1 + CLEAR; i++) {
            testData[i % 128] = 0;
        }
    }
    testData[bucket] = glMax(testData[bucket], (uint8_t)y);
}


void VectorUI::DrawTestData(VRender* vRender)
{
    vRender->Clear();
    for (int i = 0; i < 128; ++i) {
        int d = testData[i];
        vRender->DrawRect(i, 32 - d, 1, 32, 1);
    }
    if (testBar) {
        for (int i = 0; i < 128; i += 4) {
            vRender->DrawRect(i, 32 - testBar, 2, 1, 1);
            vRender->DrawRect(i+2, 32 - testBar, 2, 1, 0);
        }
    }
}


void VectorUI::Draw(
    VRender* ren, Renderer* bRen,
    uint32_t time,
    UIMode mode,
    bool bladeIgnited,
    const UIRenderData* data)
{
#if false
    //ren->DrawRect(2, 2, 10, 10, 1);
    //ren->DrawRect(20, 2, 10, 10, 1, 2);

    //ren->SetTransform(FixedNorm(1, 64), 10, 6);
    ren->SetTransform(FixedNorm(time % 8000, 8000), 20, 20);
    ren->DrawRect(0, 0, 20, 20, 1, 1);
#else
    static const int WHITE = 1;

    if (lastTime == 0) lastTime = time;
    uint32_t deltaTime = time - lastTime;
    lastTime = time;

    for (int i = 0; i < 3; ++i) {
        if (colorProp[i].end() != data->color[i]) 
            colorProp[i].start(500, colorProp[i].value(), data->color[i]);
        colorProp[i].tick(deltaTime);
    }

    if (hasTestData && bladeIgnited) {
        DrawTestData(ren);
        return;
    }

    ren->Clear();

    int p = UIRenderData::powerLevel(data->mVolts, 8);
    CStr<5> volts;
    volts.setFromNum(data->mVolts, true);

    uint8_t h, s, v;
    rgb2hsv(data->color.r, data->color.g, data->color.b, &h, &s, &v);

    // Power bar.
    if (mode == UIMode::NORMAL || mode == UIMode::MEDITATION) {
        DrawMultiBar(ren, 0, false, p);
    }

    // Audio
    if (mode == UIMode::NORMAL || mode == UIMode::VOLUME || mode == UIMode::MEDITATION) {
        DrawMultiBar(ren, W - BAR_W, true, data->volume * 2);
    }

    // Sound bank
    if (mode == UIMode::NORMAL || mode == UIMode::VOLUME) {
        DrawBanks(ren, data->soundBank);
    }

    // Font indicator
    if (mode == UIMode::PALETTE) {
        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < 4; i++) {
                static const int S = 8;
                static const int X = 6;
                int count = j * 4 + i;
                ren->DrawRect(X + (S + 2)*i, H / 2 - 10 + 10 * j, S, S, WHITE, data->palette == count ? 1 : 0);
            }
        }
        DrawBanks(ren, data->soundBank);
    }

    // Font name
    if (mode == UIMode::VOLUME) {
        if (bRen) {
            bRen->DrawStr(data->fontName.c_str(), 2, H / 2 - 4, getGlypth_calibri8);
        }
    }

    // Meditation j-bird
    if (mode == UIMode::MEDITATION) {
        if (bRen) {
            int w, h;
            get_jBird(&w, &h);
            bRen->DrawBitmap(W/2 - w/2, 0, get_jBird);
        }
    }
    
    // Color dial
    if (mode == UIMode::NORMAL || mode == UIMode::VOLUME || mode == UIMode::PALETTE /*|| mode == UIMode::COLOR_WHEEL*/) {
        if (hProp != h) {
            hTime += deltaTime;
            
            const int SPEED = 10;
            int rate = hTime / SPEED;
            hTime -= rate * SPEED;

            int left = hProp - h;
            if (left < 0) left += 180;
            int right = h - hProp;
            if (right < 0) right += 180;

            if (left <= rate || right <= rate) {
                hProp = h;
                hTime = 0;
            }
            else {
                if (left < right)
                    hProp -= rate;
                else
                    hProp += rate;
            }

            if (hProp < 0) hProp += 180;
            if (hProp >= 180) hProp -= 180;
        }

        DrawColorHSV(ren, W / 2, hProp);
        
        for (int i = 0; i < 3; ++i) {
            int y = H / 2 - 2 - 6 + 6 * i + 1;
            DrawBar(ren, W / 2 + 19, y, 12, WHITE, colorProp[i].value());
        }
    }

    // Power and bank text
    if (mode == UIMode::NORMAL) {
#if false
        static const int TX = 24;
        if (bRen) {
            CStr<5> str;
            str.setFromNum(data->mVolts, true);
            bRen->DrawStr(str.c_str(), TX, H / 2 - 4, getGlypth_calibri8);
        }
#endif
        #if true
        int digits[4] = { 0 };
        intToDigits(data->mVolts, digits, 4);

        static const int TX = 24;
        static const int DTX = 2;
        static const int TY = 2;

        for (int i = 0; i < 3; ++i) {
            ren->SetTransform(TX + (TEXT + DTX) * i, H / 2 - TEXT * 2 + TY);
            Segment(ren, TEXT, 2, digits[i], WHITE);
        }

        intToDigits(data->palette, digits, 4);
        ren->SetTransform(TX + (TEXT + DTX) * 2, H / 2 + TEXT * 0 + 2 + TY);
        Segment(ren, TEXT, 2, digits[3], WHITE);

        ren->ClearTransform();
#endif
    }
#endif
}
