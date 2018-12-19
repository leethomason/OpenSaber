#include "sketcher.h"
#include "renderer.h"
#include "assets.h"
#include "voltmeter.h"

#ifdef  _WIN32
#   define SABER_CRYSTAL			100
#   define SABER_CRYSTAL_LOW		60
#else
#   include "pins.h"
#endif

#include <math.h>

using namespace osbr;

int UIRenderData::powerLevel(int maxLevel) const
{
    return AveragePower::vbatToPowerLevel(mVolts, maxLevel);
}


Sketcher::Sketcher()
{
	for (int i = 0; i < DATA_WIDTH; ++i) {
		accelData[i] = 0;
	}
}


textureData Sketcher::GetDial(int value)
{
    textureData td = get_dial0;
    switch (value) {
    case 0: break;	// already set to default.
	case 1: td = get_dial1; break;
	case 2: td = get_dial2; break;
	case 3: td = get_dial3; break;
	case 4: td = get_dial4; break;
    default:
		ASSERT(false);
		//Log.p("value=").p(value).eol();
        break;
    }
    return td;
}

void Sketcher::Push(uint8_t val)
{
    accelData[pos] = val;
    pos++;
    if (pos == DATA_WIDTH) pos = 0;
}


void Sketcher::Draw(Renderer* d, uint32_t delta, UIMode mode, bool ignited, const UIRenderData* info)
{
	d->Fill(0);

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
	switch (mode) {
	case UIMode::NORMAL:
		DrawBladeMode(d, delta, ignited, info);
		break;
	case UIMode::PALETTE:
		DrawPaletteMode(d, delta, info);
		break;
	case UIMode::VOLUME:
		DrawVolumeMode(d, delta, info);
		break;
	case UIMode::MEDITATION:
		DrawMeditationMode(d, delta, info);
		break;
	default:
		ASSERT(false);
		break;
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


void Sketcher::DrawDials(Renderer* d, const UIRenderData* data, bool labels)
{
	d->DrawBitmap(X0, 0, GetDial(data->powerLevel(4)));
	d->DrawBitmap(X1 - DIAL_WIDTH, 0, GetDial(data->volume), Renderer::FLIP_X);
	if (labels) {
		d->DrawStr("P", X0 + 23, 12, getGlypth_aurekBesh6);
		d->DrawStr("V", X1 - 31, 12, getGlypth_aurekBesh6);
	}
}


void Sketcher::DrawStateDisplay(Renderer* d, const UIRenderData* data)
{
	static const int GUTTER = 1;

	// Current Palette
	for (int i = 0; i <= data->palette; ++i) {
		int x = 3 - (i % 4);
		int y = i / 4;
		d->DrawRectangle(CENTER - GUTTER - 24 + x * 6, y * 6, 5, 5);
	}

	// Current blade color
	for (int i = 0; i < 3; ++i) {
		d->DrawRectangle(CENTER + GUTTER, i * 4, 1 + data->color[i] * (DATA_WIDTH / 2) / 256, 3);
	}
}


void Sketcher::DrawBladeMode(Renderer* d, uint32_t time, bool ignited, const UIRenderData* data)
{
	DrawDials(d, data);

	static const int NLINES = 5;
	static const char* lines[NLINES] = {
		"THERE IS NO EMOTION, THERE IS ONLY STILLNESS.",
		"THERE IS NO THOUGHT, THERE IS ONLY SILENCE.",
		"THERE IS NO IGNORANCE, THERE IS ONLY ATTENTION.",
		"THERE IS NO DIVISION, THERE IS ONLY PERCEPTION.",
		"THERE IS NO SELF, THERE IS ONLY THE FORCE.",
	};

	animTime += time;

	if (!ignited) {
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
	else {
		// Render accelerometer data.
		static const int TOP = 15;
		static const int H = 16;

		// Draw most to least recent.
		for (int i = 0; i < DATA_WIDTH; ++i) {
			int index = (pos - 1 - i + DATA_WIDTH * 2) % DATA_WIDTH;
			ASSERT(index >= 0 && index < DATA_WIDTH);
			ASSERT((i != 0) || (index == ((pos - 1 + DATA_WIDTH) % DATA_WIDTH)));
			int point = accelData[index];
			// Values less than one (u8 64) aren't currently graphed.
			if (point < 64) point = 64;  // 1g

			int h = (point + 1 - 64) * H / 192;
			// Most recent is pos-1. Want it at the far right.
			d->DrawRectangle(CENTER + DATA_WIDTH / 2 - i, TOP + H - h, 1, h + 1);
		}
	}
	DrawStateDisplay(d, data);
}


void Sketcher::DrawPaletteMode(Renderer* d, uint32_t time, const UIRenderData* data)
{
	DrawDials(d, data);

	static const char* label = "PAL";
	const int wPal = d->StrWidth(label, getGlypth_aurekBesh6);

	CStr<5> volts;
	volts.setFromNum(data->mVolts, true);

	int wName = d->StrWidth(volts.c_str(), getGlypth_calibri8);
	d->DrawStr(volts.c_str(), CENTER - wName / 2, 14, getGlypth_calibri8);
	d->DrawStr(label, CENTER - wPal / 2, 23, getGlypth_aurekBesh6);

	DrawStateDisplay(d, data);
}


void Sketcher::DrawVolumeMode(Renderer* d, uint32_t time, const UIRenderData* data)
{
	DrawDials(d, data);

	static const char* label = "VOL";
	int wVol = d->StrWidth(label, getGlypth_aurekBesh6);

	int wName = d->StrWidth(data->fontName, getGlypth_calibri8);
	d->DrawStr(data->fontName, CENTER - wName / 2, 14, getGlypth_calibri8);

	d->DrawStr(label, CENTER - wVol / 2, 23, getGlypth_aurekBesh6);
	DrawStateDisplay(d, data);
}


void Sketcher::DrawMeditationMode(Renderer* d, uint32_t time, const UIRenderData* data)
{
	//d->DrawBitmap(0, 0, get_jBird, 0, 0, 15);
	//d->DrawBitmap(128-32, 0, get_jBird, 0, 113, 128);
	d->DrawBitmap(0, 0, get_jBird);

	static const int X = 33;

	d->DrawStr("no Emotion...Stillness",	X, 1, getGlypth_tomThumb5);
	d->DrawStr("no Thought...Silence",		X, 7, getGlypth_tomThumb5);
	d->DrawStr("no Ignorance...Attention",	X, 13, getGlypth_tomThumb5);
	d->DrawStr("no Division...Perception",	X, 19, getGlypth_tomThumb5);
	d->DrawStr("no Self...the Force",		X, 25, getGlypth_tomThumb5);
}


bool DotStarUI::Draw(osbr::RGB* led, int nLED, uint32_t time,
                     UIMode mode, bool ignited, const UIRenderData& data) const
{
    static const uint32_t COLOR_AUDIO_ON = 0x0000FF;
    static const uint32_t COLOR_AUDIO_OFF = 0xFFD800;
    static const uint32_t PALETTE_ONE = 0xFFFFFF;
    static const uint32_t MED_0 = 0x0000FF;
    static const uint32_t MED_1 = 0x0050a0;
    static const uint32_t MED_2 = 0x00a05F;
    static const uint32_t MED_3 = 0x00FF00;

    ASSERT(nLED == 4 || nLED == 6);
    osbr::RGB currentLED[6];
    for (int i = 0; i < nLED; ++i) {
        currentLED[i] = led[i];
    }

    if (ignited) {
        // Set the power level.
        int i = 0;
        int powerLevel = data.powerLevel(nLED);
        for (; i < powerLevel && i < nLED; ++i) {
            led[i] = data.color;
        }
        for (; i < nLED; ++i) {
            led[i].set(0);
        }
    }
    else {
        switch (mode) {
        case UIMode::NORMAL:
        {
            if (nLED == 4) {
                led[0].set(data.volume ? COLOR_AUDIO_ON : COLOR_AUDIO_OFF);
                led[1].set(data.volume ? COLOR_AUDIO_ON : COLOR_AUDIO_OFF);
            }
            else {
                // Not really recursvie; used to draw the 4-LED variant in 6-LED mode.
                this->Draw(led, 4, time, UIMode::VOLUME, ignited, data);
            }
            led[nLED - 2].set(0);
            led[nLED - 1] = data.color;
        }
        break;

        case UIMode::PALETTE:
        {
            led[0] = led[nLED - 1] = osbr::RGB::BLACK;
            for (int i = 1; i < nLED - 1; ++i) {
                led[i] = data.color;
            }
        }
        break;

        case UIMode::VOLUME:
        {
            int i = 0;
            for (; i < data.volume && i < 4; ++i) {
                led[i].set(COLOR_AUDIO_ON);
            }
            for (; i < 4; ++i) {
                led[i].set(COLOR_AUDIO_OFF);
            }
            for (; i < nLED; ++i) {
                led[i].set(0);
            }
        }
        break;

        case UIMode::MEDITATION:
        {
            static const int TIME_STEP = 800;
            for (int i = 0; i < nLED; ++i) {
                calcCrystalColor(time + TIME_STEP * i, SABER_CRYSTAL_LOW, SABER_CRYSTAL, data.color, &led[i]);
            }
        }
        break;
        }
    }

    for (int i = 0; i < nLED; ++i) {
        led[i].scale(m_brightness);
    }
    for (int i = 0; i < nLED; ++i) {
        if (currentLED[i] != led[i])
            return true;
    }
    return false;
}


bool DotStarUI::Test()
{
	osbr::RGB leds[6];

	UIRenderData data;
	data.mVolts = 3700;
	data.volume = 1;
	data.palette = 7;
	data.fontName = "FontName";
	data.color.set(0xff, 0, 0);

	ASSERT(data.color.get() == 0xff0000);
	{
		DotStarUI dotstar;
		
		dotstar.Draw(&leds[1], 4, 0, UIMode::NORMAL, false, data);
		ASSERT(leds[0].get() == 0);			// check memory
		ASSERT(leds[1].get() == 0x0000ff);	// sound on
		ASSERT(leds[2].get() == 0x0000ff);	// sound on
		ASSERT(leds[3].get() == 0);			// off
		ASSERT(leds[4].get() == 0xff0000);	// blade color
		ASSERT(leds[5].get() == 0);			// memory check

		dotstar.Draw(&leds[1], 4, 0, UIMode::NORMAL, true, data);
		ASSERT(leds[0].get() == 0);			// check memory
		ASSERT(leds[1] == data.color);	
		ASSERT(leds[2] == data.color);
		ASSERT(leds[3].get() == 0);			
		ASSERT(leds[4].get() == 0);			
		ASSERT(leds[5].get() == 0);			// memory check

		dotstar.Draw(&leds[1], 4, 0, UIMode::PALETTE, false, data);
		ASSERT(leds[0].get() == 0);			// check memory
		ASSERT(leds[1].get() == 0);
		ASSERT(leds[2] == data.color);
		ASSERT(leds[3] == data.color);
		ASSERT(leds[4].get() == 0);
		ASSERT(leds[5].get() == 0);			// memory check

		dotstar.Draw(&leds[1], 4, 0, UIMode::VOLUME, false, data);
		ASSERT(leds[0].get() == 0);			// check memory
		ASSERT(leds[1].get() == 0x0000ff);
		ASSERT(leds[2].get() == 0xFFD800);
		ASSERT(leds[3].get() == 0xFFD800);
		ASSERT(leds[4].get() == 0xFFD800);
		ASSERT(leds[5].get() == 0);			// memory check
	}
	{
		DotStarUI dotstar;
		dotstar.SetBrightness(128);
		osbr::RGB c2 = data.color;
		c2.scale(128);

		dotstar.Draw(&leds[1], 4, 0, UIMode::NORMAL, false, data);
		ASSERT(leds[0].get() == 0);			// check memory
		ASSERT(leds[1].get() == 0x00007f);	// sound on
		ASSERT(leds[2].get() == 0x00007f);	// sound on
		ASSERT(leds[3].get() == 0);			// off
		ASSERT(leds[4].get() == 0x7f0000);	// blade color
		ASSERT(leds[5].get() == 0);			// memory check

		dotstar.Draw(&leds[1], 4, 0, UIMode::NORMAL, true, data);
		ASSERT(leds[0].get() == 0);			// check memory
		ASSERT(leds[1] == c2);
		ASSERT(leds[2] == c2);
		ASSERT(leds[3].get() == 0);
		ASSERT(leds[4].get() == 0);
		ASSERT(leds[5].get() == 0);			// memory check

		dotstar.Draw(&leds[1], 4, 0, UIMode::PALETTE, false, data);
		ASSERT(leds[0].get() == 0);			// check memory
		ASSERT(leds[1].get() == 0);
		ASSERT(leds[2] == c2);
		ASSERT(leds[3] == c2);
		ASSERT(leds[4].get() == 0);
		ASSERT(leds[5].get() == 0);			// memory check

		dotstar.Draw(&leds[1], 4, 0, UIMode::VOLUME, false, data);
		ASSERT(leds[0].get() == 0);			// check memory
		ASSERT(leds[1].get() == 0x00007f);
		ASSERT(leds[2].get() == 0x7F6c00);
		ASSERT(leds[3].get() == 0x7F6c00);
		ASSERT(leds[4].get() == 0x7F6c00);
		ASSERT(leds[5].get() == 0);			// memory check
	}
	return true;
}


void calcCrystalColor(uint32_t t, int32_t lowVariation, int32_t highVariation, const osbr::RGB& base, osbr::RGB* out)
{
	uint32_t tc[3] = { t / 79UL, t / 101UL, t / 137UL };

    for (int i = 0; i < 3; ++i) {
    	
    	const int32_t VARIATION = base[i] > 128 ? highVariation : lowVariation;
		const int32_t INV = 256 - VARIATION;

		int32_t isin = iSin(tc[i]);
		int32_t scaledColor = (int32_t(base[i]) * INV + isin * VARIATION) / int32_t(256);
		if (scaledColor > 255) scaledColor = 255;
		if (scaledColor < 0) scaledColor = 0;
		out->set(i, uint8_t(scaledColor));
    }
}


uint8_t calcSingleCrystalColor(uint32_t t)
{
	static const uint32_t DIV = 23;
	static const uint32_t DIVL = DIV * 4;

	uint8_t s  = iSin255(uint16_t(t / uint32_t(DIV)));
	uint8_t sL = iSin255(uint16_t(t / uint32_t(DIVL)));
	uint32_t base = 10 + 50 * sL / 255;
	uint32_t scaledColor = base + uint32_t(s+1) * uint32_t(255 - base) / uint32_t(256);

	return uint8_t(clamp(scaledColor, uint32_t(0), uint32_t(255)));
}


bool TestCrystalColor()
{
	{
		const osbr::RGB base(200, 200, 200);
        osbr::RGB out;

		for (uint32_t t = 0; t < 2000; ++t) {
			calcCrystalColor(t, 20, 20, base, &out);
			for (int i = 0; i < 3; ++i) {
				ASSERT(out[i] >= base[i] - 20);
				ASSERT(out[i] <= base[i] + 20);
			}
		}
	}
	{
		const osbr::RGB base(0, 0, 255);
        osbr::RGB out;

		for (uint32_t t = 0; t < 10*1000; t += 10) {
			calcCrystalColor(t, 100, 100, base, &out);
			for (int i = 0; i < 3; ++i) {
				ASSERT(out[i] >= base[i] - 100);
				ASSERT(out[i] <= 255);
			}
		}
	}
	return true;
}


Pixel_7_5_UI::Pixel_7_5_UI()
{
    for (int i = 0; i < ALLOCATED; ++i)
        m_col[i] = 0;
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
void Pixel_7_5_UI::Draw(uint32_t time, UIMode mode, bool bladeIgnited, const UIRenderData* data)
{
	for (int i = 0; i < ALLOCATED; ++i)
		m_col[i] = 0;

	switch (mode) {
	case UIMode::NORMAL:
		getGlypth_tomThumb5('0' + data->palette, m_col + 0);
		DrawBar(5, data->powerLevel(4));
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
        m_col[3]            = 0x1f;
        break;

	default:
		break;
	}
}


void Pixel_7_5_UI::DrawBar(int x, int y)
{
	uint8_t* c = m_col + x;
	*c = 0;
	for (int i = 0; i < y; ++i) {
		*c |= (16 >> i);
	}
}


void Pixel_7_5_UI::DrawDot(int x, int y)
{
	m_col[x] |= (16 >> y);
}


void Digit4UI::Draw(UIMode mode, const UIRenderData* data)
{
	char scratch[6] = "     ";
	m_output.clear();
	uint32_t volts = data->mVolts / 1000;
	uint32_t deciVolts = (data->mVolts - volts * 1000) / 100;

	switch(mode) {
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
	}
	m_output = scratch;
}
