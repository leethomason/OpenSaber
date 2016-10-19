#include "sketcher.h"
#include "renderer.h"
#include "assets.h"

#include <math.h>
#include <string.h>


Sketcher::Sketcher()
{
	color[0] = color[1] = color[2] = 0;
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

void Sketcher::Draw(Renderer* d, uint32_t delta, int mode)
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
		d->DrawBitmap(X0, 0, GetDial(power));
		d->DrawBitmap(X1 - DIAL_WIDTH, 0, GetDial(volume), Renderer::FLIP_X);
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
		bool render = d->DrawStr(
			lines[line], 
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
			uint32_t v = mVolts;
			volts[0] = '0' + mVolts / 1000;
			volts[1] = '0' + (mVolts % 1000) / 100;
			volts[2] = '0' + (mVolts % 100) / 10;
			volts[3] = '0' + (mVolts % 10);
			int wName = d->StrWidth(volts, getGlypth_calibri8);
			d->DrawStr(volts, CENTER - wName / 2, 14, getGlypth_calibri8);
		}
		else {
			int wName = d->StrWidth(fontName, getGlypth_calibri8);
			d->DrawStr(fontName, CENTER - wName / 2, 14, getGlypth_calibri8);
		}
		d->DrawStr(label, CENTER - wPal / 2, 23, getGlypth_aurekBesh6);
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
			d->DrawRectangle(i + CENTER - DATA_WIDTH / 2, TOP + H - h, 1, h + 1);
			q++;
			if (q == DATA_WIDTH) q = 0;
		}
	}

	{
		static const int GUTTER = 1;

		// Current Palette
		for (int i = 0; i <= palette; ++i) {
			int x = 3 - (i % 4);
			int y = i / 4;
			d->DrawRectangle(CENTER - GUTTER - 24 + x * 6, y * 6, 5, 5);
		}

		// Current blade color
		for (int i = 0; i < 3; ++i) {
			d->DrawRectangle(CENTER + GUTTER, i * 4, 1 + color[i] * (DATA_WIDTH/2) / 256, 3);
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
