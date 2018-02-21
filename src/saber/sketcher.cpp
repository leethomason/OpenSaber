#include "sketcher.h"
#include "renderer.h"
#include "assets.h"

#include <math.h>

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
	d->DrawBitmap(X0, 0, GetDial(data->power));
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


bool DotStarUI::Draw(RGB* led, UIMode mode, bool ignited, const UIRenderData& data)
{
	static const uint32_t COLOR_AUDIO_ON 	= 0x0000FF;
	static const uint32_t COLOR_AUDIO_OFF 	= 0xFFD800;
	static const uint32_t PALETTE_ONE 		= 0xFFFFFF;
	static const uint32_t MED_0				= 0x0000FF;
	static const uint32_t MED_1				= 0x0050a0;
	static const uint32_t MED_2				= 0x00a05F;
	static const uint32_t MED_3				= 0x00FF00;

	RGB currentLED[4];
	for(int i=0; i<4; ++i) {
		currentLED[i] = led[i];
	}

	if (ignited) {
		// Set the power level.
		int i = 0;
		for (; i < data.power && i < 4; ++i) {
			led[i] = data.color;
		}
		for (; i < 4; ++i) {
			led[i].set(0);
		}
	}
	else {
		switch (mode) {
		case UIMode::NORMAL:
		{
			led[0].set(data.volume ? COLOR_AUDIO_ON : COLOR_AUDIO_OFF);
			led[1].set(data.volume ? COLOR_AUDIO_ON : COLOR_AUDIO_OFF);
			led[2].set(0);
			led[3] = data.color;
		}
		break;

		case UIMode::PALETTE:
		{
			led[0].set((data.palette & 1) ? PALETTE_ONE : 0);
			led[1].set((data.palette & 2) ? PALETTE_ONE : 0);
			led[2].set((data.palette & 4) ? PALETTE_ONE : 0);
			led[3] = data.color;
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
		}
		break;

		case UIMode::MEDITATION:
		{
			led[0].set(MED_0);
			led[1].set(MED_1);
			led[2].set(MED_2);
			led[3].set(MED_3);
		}
		break;

		}
	}
	bool black = true;
	for (int i = 0; i < 4; ++i) {
		led[i].scale(m_brightness);
		if (led[i].get()) {
			black = false;
		}
	}
	ASSERT(!black);
	for(int i=0; i<4; ++i) {
		if (currentLED[i] != led[i])
			return true;
	}
	return false;
}


bool DotStarUI::Test()
{
	RGB leds[6];

	UIRenderData data;
	data.power = 2;
	data.mVolts = 3700;
	data.volume = 1;
	data.palette = 7;
	data.fontName = "FontName";
	data.color.set(0xff, 0, 0);

	ASSERT(data.color.get() == 0xff0000);
	{
		DotStarUI dotstar;
		
		dotstar.Draw(&leds[1], UIMode::NORMAL, false, data);
		ASSERT(leds[0].get() == 0);			// check memory
		ASSERT(leds[1].get() == 0x0000ff);	// sound on
		ASSERT(leds[2].get() == 0x0000ff);	// sound on
		ASSERT(leds[3].get() == 0);			// off
		ASSERT(leds[4].get() == 0xff0000);	// blade color
		ASSERT(leds[5].get() == 0);			// memory check

		dotstar.Draw(&leds[1], UIMode::NORMAL, true, data);
		ASSERT(leds[0].get() == 0);			// check memory
		ASSERT(leds[1] == data.color);	
		ASSERT(leds[2] == data.color);
		ASSERT(leds[3].get() == 0);			
		ASSERT(leds[4].get() == 0);			
		ASSERT(leds[5].get() == 0);			// memory check

		dotstar.Draw(&leds[1], UIMode::PALETTE, false, data);
		ASSERT(leds[0].get() == 0);			// check memory
		ASSERT(leds[1].get() == 0xffffff);
		ASSERT(leds[2].get() == 0xffffff);
		ASSERT(leds[3].get() == 0xffffff);
		ASSERT(leds[4] == data.color);
		ASSERT(leds[5].get() == 0);			// memory check

		dotstar.Draw(&leds[1], UIMode::VOLUME, false, data);
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
		RGB c2 = data.color;
		c2.scale(128);

		dotstar.Draw(&leds[1], UIMode::NORMAL, false, data);
		ASSERT(leds[0].get() == 0);			// check memory
		ASSERT(leds[1].get() == 0x00007f);	// sound on
		ASSERT(leds[2].get() == 0x00007f);	// sound on
		ASSERT(leds[3].get() == 0);			// off
		ASSERT(leds[4].get() == 0x7f0000);	// blade color
		ASSERT(leds[5].get() == 0);			// memory check

		dotstar.Draw(&leds[1], UIMode::NORMAL, true, data);
		ASSERT(leds[0].get() == 0);			// check memory
		ASSERT(leds[1] == c2);
		ASSERT(leds[2] == c2);
		ASSERT(leds[3].get() == 0);
		ASSERT(leds[4].get() == 0);
		ASSERT(leds[5].get() == 0);			// memory check

		dotstar.Draw(&leds[1], UIMode::PALETTE, false, data);
		ASSERT(leds[0].get() == 0);			// check memory
		ASSERT(leds[1].get() == 0x7f7f7f);
		ASSERT(leds[2].get() == 0x7f7f7f);
		ASSERT(leds[3].get() == 0x7f7f7f);
		ASSERT(leds[4] == c2);
		ASSERT(leds[5].get() == 0);			// memory check

		dotstar.Draw(&leds[1], UIMode::VOLUME, false, data);
		ASSERT(leds[0].get() == 0);			// check memory
		ASSERT(leds[1].get() == 0x00007f);
		ASSERT(leds[2].get() == 0x7F6c00);
		ASSERT(leds[3].get() == 0x7F6c00);
		ASSERT(leds[4].get() == 0x7F6c00);
		ASSERT(leds[5].get() == 0);			// memory check
	}
	return true;
}


void calcCrystalColor(uint32_t t, int32_t lowVariation, int32_t highVariation, const RGB& base, RGB* out)
{
	uint32_t tc[3] = { t / 79UL, t / 101UL, t / 137UL };

    for (int i = 0; i < 3; ++i) {
    	
    	const int32_t VARIATION = base[i] > 128 ? highVariation : lowVariation;
		const int32_t INV = 256 - VARIATION;

		int32_t s = isin(tc[i]);
		int32_t scaledColor = (int32_t(base[i]) * INV + s * VARIATION) / int32_t(256);
		if (scaledColor > 255) scaledColor = 255;
		if (scaledColor < 0) scaledColor = 0;
		out->set(i, uint8_t(scaledColor));
    }
}


uint8_t calcSingleCrystalColor(uint32_t t)
{
	/*
	uint32_t tc[2] = { t / 17UL, t / 71UL };
	int32_t s0 = isin(tc[0]);
	int32_t s1 = isin(tc[1]);

	int32_t scaledColor = ((s0+256) * 2 + s1) / 5;
	//int32_t scaledColor = s1;
	scaledColor = (scaledColor + 256) / 2;
	scaledColor = clamp(scaledColor, 0, 255);
	*/

	int32_t s = isin(uint16_t(t / uint32_t(17)));
	uint32_t scaledColor = uint32_t(30) + uint32_t(s + 256) * uint32_t(205) / uint32_t(512);

	return uint8_t(clamp(scaledColor, uint32_t(0), uint32_t(255)));
}


bool TestCrystalColor()
{
	{
		const RGB base(200, 200, 200);
		RGB out;

		for (uint32_t t = 0; t < 2000; ++t) {
			calcCrystalColor(t, 20, 20, base, &out);
			for (int i = 0; i < 3; ++i) {
				ASSERT(out[i] >= base[i] - 20);
				ASSERT(out[i] <= base[i] + 20);
			}
		}
	}
	{
		const RGB base(0, 0, 255);
		RGB out;

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
}

void Pixel_7_5_UI::Draw(uint32_t time, UIMode mode, bool bladeIgnited, const UIRenderData* data)
{
	for (int i = 0; i < NCOLS; ++i)
		col[i] = 0;

	switch (mode) {
	case UIMode::NORMAL:
		getGlypth_tomThumb5('0' + data->palette, col);

		DrawBar(col + 4, data->power);
		DrawBar(col + 6, data->volume);
		break;

	case UIMode::PALETTE:
		getGlypth_tomThumb5('P', col);
		getGlypth_tomThumb5('0' + data->palette, col + 4);
		break;

	case UIMode::VOLUME:
		getGlypth_tomThumb5('V', col);
		getGlypth_tomThumb5('0' + data->volume, col + 4);
		break;

	default:
		break;
	}
}


void Pixel_7_5_UI::DrawBar(uint8_t* c, int v)
{
	*c = 0;
	for (int i = 0; i < v; ++i) {
		*c |= (16 >> i);
	}
}
