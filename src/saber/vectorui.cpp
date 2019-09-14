#include "vectorui.h"
#include "vrender.h"
#include "assets.h"
#include "Grinliz_Util.h"
#include "sketcher.h"
#include "rgb2hsv.h"


void VectorUI::NumToDigit(int num, int* digits)
{
    digits[0] = num / 1000;
    num -= digits[0] * 1000;
    digits[1] = num / 100;
    num -= digits[1] * 100;
    digits[2] = num / 10;
    num -= digits[2] * 10;
    digits[3] = num;
}

void VectorUI::Segment(VRender* ren, int width, int s, int num, osbr::RGBA rgba)
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

    static const uint8_t segments[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

    if (num < 0) num = 0;
    if (num > 9) num = 9;
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


void VectorUI::Draw(VRender* ren,
    uint32_t time,
    UIMode mode,
    bool bladeIgnited,
    const UIRenderData* data)
{
    ren->Clear();

    static const osbr::RGBA POWER_ON(43, 163, 255, 200);
    static const osbr::RGBA POWER_OFF(43, 163, 255, 40);
    static const osbr::RGBA POWER_TEXT(43, 163, 255, 200);

    static const osbr::RGBA AUDIO_ON(255, 163, 0, 200);
    static const osbr::RGBA AUDIO_OFF(43, 163, 255, 40);

    static const osbr::RGBA PALETTE_ON = POWER_ON;
    static const osbr::RGBA PALETTE_OFF = POWER_OFF;

    int p = data->powerLevel(8);
    CStr<5> volts;
    volts.setFromNum(data->mVolts, true);

#if true
    static const int W = 128;
    static const int H = 32;
    static const int BAR_W = 16;
    static const int TEXT = 5;

    static const osbr::RGBA WHITE(255, 255, 255);
    static const osbr::RGBA BLACK(0, 0, 0, 255);

    // Power
    if (mode == UIMode::NORMAL) {
        for (int r = 0; r < 8; ++r) {
            Fixed115 d = r - Fixed115(7, 2);
            Fixed115 fx = Fixed115(8, 10) * d * d;
            ren->DrawRect(fx.getInt(), 28 - r * 4, BAR_W, 3, WHITE, r < p ? 0 : 1);
        }
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
        for (int i = 0; i < 5; ++i) {
            static const int S = 6;
            ren->DrawRect(12 + 4 * i, H / 2 - (S + i * 4) / 2, 2, S + i * 4, WHITE);
        }
    }

    // Audio
    {
        for (int r = 0; r < 8; ++r) {
            Fixed115 d = r - Fixed115(7, 2);
            Fixed115 fx = Fixed115(8, 10) * d * d;
            ren->DrawRect(W - fx.getInt() - BAR_W, 28 - r * 4, BAR_W, 3, AUDIO_ON, r / 2 < data->volume ? 0 : 1);
        }
    }
    // Color indicator
    {
        FixedNorm rotations[6] = {
            FixedNorm(0, 6), FixedNorm(1, 6), FixedNorm(2, 6), 
            FixedNorm(3, 6), FixedNorm(4, 6), FixedNorm(5, 6) 
        };
        for (int r = 0; r < 6; ++r) {
            ren->SetTransform(rotations[r], W / 2, H / 2);
            ren->DrawRect(-1, 10, 2, 5, WHITE);
        }

        uint8_t h, s, v;
        rgb2hsv(data->color.r, data->color.g, data->color.b, &h, &s, &v);

        ren->SetTransform(FixedNorm(h, 180), W / 2, H / 2);
        ren->DrawRect(-2, 0, 4, 12, WHITE);

        ren->ClearTransform();

        for (int i = 0; i < 3; ++i) {
            int y = H / 2 - 2 - 6 + 6 * i;
            ren->DrawRect(W / 2 + 18, y, 12, 3, WHITE, 1);
            int c = data->color[i] / 25;
            if (c > 0)
                ren->DrawRect(W / 2 + 19, y + 1, c, 1, WHITE);
        }

        int digits[4];
        NumToDigit(data->palette, digits);
        ren->SetTransform(W - 30, H / 2 - TEXT);
        Segment(ren, TEXT, 2, digits[3], WHITE);
    }

    // Power
    if (mode == UIMode::NORMAL) {
        int digits[4];
        NumToDigit(data->mVolts, digits);
        for (int i = 0; i < 4; ++i) {
            ren->SetTransform(20 + (TEXT+2)*i, H / 2 - TEXT);
            Segment(ren, TEXT, 2, digits[i], WHITE);
        }
    }
    ren->Render();

#endif

#if false
    static const int W = 160;
    static const int H = 80;

    static const int BAR_W = 24;
    // Power
    {
        for (int r = 0; r < 8; ++r) {
            Fixed115 d = r - Fixed115(7, 2);
            Fixed115 fx = Fixed115(11, 10) * d * d + 2;
            ren->DrawRect(fx.getInt(), 72 - r * 10, BAR_W, 6, POWER_ON, r < p ? 0 : 1);
        }
    }

    // Audio
    {
        for (int r = 0; r < 8; ++r) {
            Fixed115 d = r - Fixed115(7, 2);
            Fixed115 fx = Fixed115(11, 10) * d * d + 2;
            ren->DrawRect(W - fx.getInt() - BAR_W, 72 - r * 10, BAR_W, 6, AUDIO_ON , r / 2 < data->volume ? 0 : 1);
        }
    }

    // Crystal
    static const int S = 14;
    static const int T = 4;
    {
        static const VRender::Vec2 DIAMOND[4] = {
            {0, -S}, {S, 0}, {0, S}, {-S, 0}
        };
        static const VRender::Vec2 ARROW[6] = {
            {0, -S}, {T, -S}, {T + S, 0}, {T, S}, {0, S}, {S, 0}
        };

        ren->SetTransform(W / 2, H / 2);
        ren->DrawPoly(DIAMOND, 4, osbr::RGBA(data->color.r, data->color.g, data->color.b));

        bool drawLeft = (mode == UIMode::NORMAL || mode == UIMode::PALETTE);
        bool drawRight = (mode == UIMode::NORMAL || mode == UIMode::VOLUME);

        for (int i = 0; i < 4; ++i) {
            osbr::RGBA c(data->color.r, data->color.g, data->color.b, 255 - i * 50);
            
            if (drawRight) {
                ren->SetScale(1, 1);
                ren->SetTransform(W / 2 + T + 2 * T * i, H / 2);
                ren->DrawPoly(ARROW, 6, c);
            }

            if (drawLeft) {
                ren->SetTransform(W / 2 - T - 2 * T * i, H / 2);
                ren->SetScale(-1, 1);
                ren->DrawPoly(ARROW, 6, c);
            }
        }
        ren->ClearTransform();
    }

    for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 4; ++i) {
            const static int S = 18;
            ren->DrawRect(
                W / 2 + S * (i - 2), 
                H / 2 + S * (j - 2), 
                S, S, 
                osbr::RGBA(255, 255, 255, 80), 1);
        }
    }

    // VoltMeter
    VRenderUtil::DrawStr(ren, volts.c_str(), W / 2 - 30, H / 2 + S + 4, getGlypth_calibri8, POWER_TEXT);


    // Palette
    /*{
        static const int D = 8;
        for (int j = 0; j < 2; ++j) {
            for (int i = 0; i < 4; ++i) {
                ren->DrawRect(W / 2 - 19 + i*10, 4 + j*10, D, D, POWER_ON);
            }
        }
    }*/

    ren->Render();
#endif
}
