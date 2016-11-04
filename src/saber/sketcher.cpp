#include "sketcher.h"
#include "renderer.h"
#include "assets.h"

#include <math.h>
#include <string.h>

static int16_t gSinTable[256] = { 0 };

int16_t isin(uint16_t x)
{
	if (gSinTable[64] == 0) {
		for (int i = 0; i < 256; ++i) {
			float x = 2 * 3.14159f * float(i) / 256.0f;
			gSinTable[i] = int16_t(sin(x) * 256.0f);
		}
	}
	return gSinTable[x & 0xff];
}

Sketcher::Sketcher()
{
    memset(data, 0, DATA_WIDTH);
}

textureData Sketcher::GetDial(int value)
{
    textureData td = get_dial0;
    switch (value) {
	case 1: td = get_dial1; break;
	case 2: td = get_dial2; break;
	case 3: td = get_dial3; break;
	case 4: td = get_dial4; break;
    default:
		OSASSERT(false);
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
	{
		d->DrawBitmap(X0, 0, GetDial(info->power));
		d->DrawBitmap(X1 - DIAL_WIDTH, 0, GetDial(info->volume), Renderer::FLIP_X);
		d->DrawStr("P", X0 + 23, 12, getGlypth_aurekBesh6);
		d->DrawStr("V", X1 - 31, 12, getGlypth_aurekBesh6);
	}
	static const int NLINES = 5;
	static const char* lines[NLINES] = {
		"THERE IS NO EMOTION, THERE IS ONLY STILLNESS.",
		"THERE IS NO THOUGHT, THERE IS ONLY SILENCE.",
		"THERE IS NO IGNORANCE, THERE IS ONLY ATTENTION.",
		"THERE IS NO DIVISION, THERE IS ONLY PERCEPTION.",
		"THERE IS NO SELF, THERE IS ONLY THE FORCE.",
	};

	animTime += delta;

	if (mode == REST_MODE) {
		// Render the Jedi Creed.
		int dx = animTime / 100;
		bool render = d->DrawStr(lines[line],
			X1 - DIAL_WIDTH - 1 - dx, 23,
			getGlypth_aurekBesh6,
			X0 + DIAL_WIDTH,
			X1 - DIAL_WIDTH);

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

		int wPal = d->StrWidth(label, getGlypth_aurekBesh6);

		if (mode == PALETTE_MODE) {
			char volts[5] = { 0 };
			volts[0] = '0' + info->mVolts / 1000;
			volts[1] = '0' + (info->mVolts % 1000) / 100;
			volts[2] = '0' + (info->mVolts % 100) / 10;
			volts[3] = '0' + (info->mVolts % 10);
			int wName = d->StrWidth(volts, getGlypth_calibri8);
			d->DrawStr(volts, CENTER - wName / 2, 14, getGlypth_calibri8);
		}
		else {
			int wName = d->StrWidth(info->fontName, getGlypth_calibri8);
			d->DrawStr(info->fontName, CENTER - wName / 2, 14, getGlypth_calibri8);
		}
		d->DrawStr(label, CENTER - wPal / 2, 23, getGlypth_aurekBesh6);
	}

	if (mode == BLADE_ON_MODE) {
		// Render accelerometer data.
		static const int TOP = 15;
		static const int H = 16;

		// Draw most to least recent.
		for (int i = 0; i < DATA_WIDTH; ++i) {
			int index = (pos - 1 - i + DATA_WIDTH * 2) % DATA_WIDTH;
			OSASSERT(index >= 0 && index < DATA_WIDTH);
			OSASSERT((i != 0) || (index == ((pos - 1 + DATA_WIDTH) % DATA_WIDTH)));
			int point = data[index];
			// Values less than one (u8 64) aren't currently graphed.
			if (point < 64) point = 64;  // 1g

			int h = (point + 1 - 64) * H / 192;
			// Most recent is pos-1. Want it at the far right.
			d->DrawRectangle(CENTER + DATA_WIDTH / 2 - i, TOP + H - h, 1, h + 1);
		}
	}

	{
		static const int GUTTER = 1;

		// Current Palette
		for (int i = 0; i <= info->palette; ++i) {
			int x = 3 - (i % 4);
			int y = i / 4;
			d->DrawRectangle(CENTER - GUTTER - 24 + x * 6, y * 6, 5, 5);
		}

		// Current blade color
		for (int i = 0; i < 3; ++i) {
			d->DrawRectangle(CENTER + GUTTER, i * 4, 1 + info->color[i] * (DATA_WIDTH / 2) / 256, 3);
		}
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


void DotStarUI::Draw(RGB* led, int mode, const UIRenderData* data)
{
    static const uint32_t COLOR_AUDIO_ON  = 0x0000FF;
    static const uint32_t COLOR_AUDIO_OFF = 0xFFD800;
    //static const uint32_t COLOR_POWER_ON  = 0x00FF00;
    //static const uint32_t COLOR_POWER_OFF = 0x800000;
    static const uint32_t PALETTE_ONE     = 0xFFFFFF;

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
                //led[i].set(COLOR_POWER_ON);
                led[i] = data->color;
            }
            for(; i<4; ++i) {
                //led[i].set(COLOR_POWER_OFF);
                led[i].set(0);
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
    for(int i=0; i<4; ++i) {
    	led[i].scale(8);
    }
}


void calcCrystalColor(uint32_t t, const RGB& base, RGB* out)
{
    static const int32_t RATIO = 220;
    static const int32_t RATIO_M1 = 256 - RATIO;
    static const int32_t DIV = 256;

    uint32_t tc[3] = { t / 53UL, t / 67UL, t / 79UL };

    for (int i = 0; i < 3; ++i) {
        int32_t s = isin(tc[i]);
        int32_t scaledColor = int32_t(base[i]) * RATIO + s * RATIO_M1;
        if (scaledColor < 0) scaledColor = 0;

        out[i] = uint8_t(scaledColor / DIV);
    }
}
