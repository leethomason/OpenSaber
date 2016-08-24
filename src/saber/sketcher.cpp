#include "sketcher.h"
#include "renderer.h"
#include "assets.h"

#include <math.h>
#include <string.h>

Sketcher::Sketcher()
{
    memset(data, 0, DATA_WIDTH);
}

textureData Sketcher::GetDial(int value)
{
    textureData td = get_dial0;
    switch (value) {
    case 1:
        td = get_dial1;
        break;
    case 2:
        td = get_dial2;
        break;
    case 3:
        td = get_dial3;
        break;
    case 4:
        td = get_dial4;
        break;
    default:
        break;
    }
    return td;
}

void Sketcher::Push(uint8_t val)
{
    data[pos] = val;
    pos++;
    if (pos == DATA_WIDTH) pos = 0;
}

void Sketcher::Draw(Renderer* d, uint32_t delta, int mode, const UIRenderData* info)
{
    d->Fill(0);

    //#define TEST5
#ifdef TEST1
    for (int i = 0; i < 11; ++i) {
        d->DrawStr("U", -5 + 127 * i / 10, -5 + 34 * i / 10, getGlypth_aurekBesh6); // , 10, 100);
    }
#endif
#ifdef TEST2
    for (int i = 0; i < 4; ++i) {
        d->DrawBitmap(100 * i / 3, 15 * i / 3, GetDial(0));
    }
#endif
#ifdef TEST3
    for (int i = 0; i < 4; ++i) {
        d->DrawBitmap(100 * i / 3, 15 - 15 * i / 3, GetDial(1), Renderer::FLIP_X);
    }
#endif
#ifdef TEST4
    for (int i = 0; i < 4; ++i) {
        d->DrawRectangle(0, i * 2, 128, 1);
    }
    for (int i = 0; i < 4; ++i) {
        d->DrawRectangle(i * 2, 10, 1, 118);
    }
    d->DrawRectangle(20, 10, 108, 22);
#endif
#ifdef TEST5
    for (int i = 0; i < 128; ++i) {
        d->DrawRectangle(i, 24 + i % 8, 1, 1);
    }
#endif

#if 1
    d->DrawBitmap(0, 0, GetDial(info->power));
    d->DrawBitmap(WIDTH - DIAL_WIDTH, 0, GetDial(info->volume), Renderer::FLIP_X);
    d->DrawStr("P", 23, 12, getGlypth_aurekBesh6);
    d->DrawStr("V", 97, 12, getGlypth_aurekBesh6);

    /* From Alain Bloch:
        There is no emotion. There is only stillness.
        There is no thought. There is only silence.
        There is no ignorance. There is only attention.
        There is no division. There is only perception.
        There is no self. There is only the Force.
    */

    /*
        The crystal is the heart of the blade.
        The heart is the crystal of the Jedi.
        The Jedi is the crystal of the Force.
        The Force is the blade of the heart.
    */

    static const int NLINES = 5;
    static const char* lines[NLINES] = {
        "THERE IS NO DISCORD, THERE IS ONLY STILLNESS.",
        "THERE IS NO THOUGHT, THERE IS ONLY SILENCE.",
        "THERE IS NO IGNORANCE, THERE IS ONLY ATTENTION.",
        "THERE IS NO DIVISION, THERE IS ONLY PERCEPTION.",
        "THERE IS NO SELF, THERE IS ONLY THE FORCE.",
    };

    animTime += delta;

    if (mode == REST_MODE) {
        // Render the Jedi Creed.
        int dx = animTime / 100;
        bool render = d->DrawStr(lines[line], WIDTH - DIAL_WIDTH - 1 - dx, 23, getGlypth_aurekBesh6,
                                 DIAL_WIDTH, WIDTH - DIAL_WIDTH);
        if (!render) {
            ++line;
            if (line == NLINES)
                line = 0;
            animTime = 0;
        }
    }

    if (mode == PALETTE_MODE || mode == VOLUME_MODE)
    {
        const char* label = mode == PALETTE_MODE ? "PAL" : "VOL";

        int wName = d->StrWidth(info->fontName, getGlypth_calibri8);
        int wPal = d->StrWidth(label, getGlypth_aurekBesh6);

        d->DrawStr(info->fontName, WIDTH / 2 - wName / 2, 14, getGlypth_calibri8);
        d->DrawStr(label, WIDTH / 2 - wPal / 2, 23, getGlypth_aurekBesh6);
    }

    if (mode == BLADE_ON_MODE) {
        // Render accelerometer data.
        uint8_t q = pos;
        static const int TOP = 15;
        static const int H = 16;

        for (int i = DATA_WIDTH - 1; i >= 0; --i) {
            int point = data[q];
            // Values less than one (u8 64) aren't currently graphed.
            if (point < 64) point = 64;  // 1g

            int h = (point + 1 - 64) * H / 192;
            d->DrawRectangle(i + INNERX, TOP + H - h, 1, h+1);
            q++;
            if (q == DATA_WIDTH) q = 0;
        }
    }

    // Current Palette
    for (int i = 0; i <= info->palette; ++i) {
        int x = 3 - (i % 4);
        int y = i / 4;
        d->DrawRectangle(INNERX + x * 6, y * 6, 5, 5);
    }

    // Current blade color
    static const int CSTART = WIDTH / 2 + 6;
    static const int CWIDTH = WIDTH - CSTART - INNERX;
    for (int i = 0; i < 3; ++i) {
        d->DrawRectangle(CSTART, i * 4, 1 + info->color[i] * CWIDTH / 256, 3);
    }
#endif
#if 0
    // Test pattern. dot-space-line
    uint8_t* buf = d->Buffer();
    for (int i = 0; i < 130; ++i) {
        buf[i] = i;
    }
#endif
}


void DotStarUI::Draw(DotStar::RGB* led, int mode, const UIRenderData* data)
{
    static const uint32_t COLOR_AUDIO_ON  = 0x0000FF;
    static const uint32_t COLOR_AUDIO_OFF = 0xFFD800;
    static const uint32_t COLOR_POWER_ON  = 0x00FF00;
    static const uint32_t COLOR_POWER_OFF = 0x800000;
    static const uint32_t PALETTE_ONE     = 0xFFFFFF;
    //static const uint32_t PALETTE_ZERO    = 0x202020;

    switch(mode) {
        case Sketcher::REST_MODE:
        {
            led[0].set(data->volume ? COLOR_AUDIO_ON : COLOR_AUDIO_OFF);
            led[1].set(data->volume ? COLOR_AUDIO_ON : COLOR_AUDIO_OFF);
            led[2].set(0);
            led[3].set(data->color[0], data->color[1], data->color[2]);
        }
        break;

        case Sketcher::BLADE_ON_MODE:
        {
            int i=0;
            for(; i<data->power && i < 4; ++i) {
                led[i].set(COLOR_POWER_ON);
            }
            for(; i<4; ++i) {
                led[i].set(COLOR_POWER_OFF);
            }
        }
        break;

        case Sketcher::PALETTE_MODE:
        {
            led[0].set((data->palette & 1) ? PALETTE_ONE : 0);
            led[1].set((data->palette & 2) ? PALETTE_ONE : 0);
            led[2].set((data->palette & 4) ? PALETTE_ONE : 0);
            led[3].set(data->color[0], data->color[1], data->color[2]);            
        }
        break;

        case Sketcher::VOLUME_MODE:
        {
            int i=0;
            for(; i<data->volume && i < 4; ++i) {
                led[i].set(COLOR_AUDIO_ON);
            }
            for(; i<4; ++i) {
                led[i].set(COLOR_AUDIO_OFF);
            }
        }
        break;
    }
}
