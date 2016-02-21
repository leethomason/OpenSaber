#include "sketcher.h"
#include "renderer.h"
#include "assets.h"

#include <math.h>

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

void Sketcher::Draw(Renderer* d, uint32_t time, bool restMode)
{
	d->Fill(0);

//#define TEST4
#ifdef TEST1
	for (int i = 0; i < 11; ++i) {
		d->DrawStr("U", -5 + 127 * i / 10, -5 + 34 * i / 10, getGlypth_aurekBesh6); // , 10, 100);
	}
#endif
#ifdef TEST2
	for (int i = 0; i < 4; ++i) {
		d->DrawBitmap(100*i/3, 15*i/3, GetDial(0));
	}
#endif
#ifdef TEST3
	for (int i = 0; i < 4; ++i) {
		d->DrawBitmap(100*i/3, 15 - 15*i/3, GetDial(1), Renderer::FLIP_X);
	}
#endif
#ifdef TEST4
	for(int i=0; i< 4; ++i) {
		d->DrawRectangle(0, i*2, 128, 1);
	}
	for(int i=0; i< 4; ++i) {
		d->DrawRectangle(i*2, 10, 1, 118);
	}
	d->DrawRectangle(20, 10, 108, 22);
#endif

#if 1
	d->DrawBitmap(0, 0, GetDial(power));
	d->DrawBitmap(WIDTH - DIAL_WIDTH, 0, GetDial(volume), Renderer::FLIP_X);
	d->DrawStr("P", 23, 12, getGlypth_aurekBesh6);
	d->DrawStr("V", 96, 12, getGlypth_aurekBesh6);

	//d->DrawRectangle(WIDTH / 2 - 1, 2, 3, 10);

	static const int NLINES = 6;
	static const char* lines[NLINES] = {
		"THERE IS NO DISCORD, THERE IS SERENITY.",
		"THERE IS NO THOUGHT, THERE IS PERCEPTION.",
		"THERE IS NO IGNORANCE, THERE IS ATTENTION.",
		"THERE IS NO DIVISION, THERE IS EMPATHY.",
		"THERE IS NO SELF, THERE IS THE FORCE.",
		"MAY THE FORCE BE WITH YOU, ALWAYS."
	};

	if (prevTime) {
		animTime += time - prevTime;
	}
	prevTime = time;
	
	if (restMode) {
		int dx = animTime / 100; // / 80;
		bool render = d->DrawStr(lines[line], WIDTH - DIAL_WIDTH - 1 - dx, 22, getGlypth_aurekBesh6,
								 DIAL_WIDTH, WIDTH - DIAL_WIDTH);
		if (!render) {
			++line;
			if (line == NLINES)
				line = 0;
			animTime = 0;
		}
	}
	else {
		for (int x = INNERX; x < DATA_WIDTH; ++x) {
			//d->DrawRectangle(x, 14 + 9 + 9 * sin(x / 3.0), 1, 1);
			//d->DrawRectangle(x, 11 + 5 + 5 * sin(x / 3.0), 1, 1);
			//d->DrawRectangle(x, 22 + 5 + 5 * sin(0.3 + x / 2.0), 1, 1);
			d->DrawRectangle(x, 12, 1, 18);
		}
	}
	
	// Current Palette
	const int PALETTE = 5;	// 0-based
	for (int i = 0; i <= PALETTE; ++i) {
		int x = i % 4;
		int y = i / 4;
		d->DrawRectangle(INNERX + x*5, y*5, 4, 4);
	}

	// Current blade color
	const uint8_t color[3] = { 0, 255, 100 };
	static const int CSTART = WIDTH / 2 + 6;
	static const int CWIDTH = WIDTH - CSTART - INNERX;
	for (int i = 0; i < 3; ++i) {
		d->DrawRectangle(CSTART, i * 3, 1 + color[i] * CWIDTH / 256, 2);
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
