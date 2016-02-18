#include "display.h"
#include <string.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

Display::Display(int w, int h)
{
	width = w;
	height = h;
	pixels = new uint32_t[w*h];
	buffer = new uint8_t[w*h];

	memset(pixels, 0, w * h * sizeof(uint32_t));
	memset(buffer, 0, w * h * sizeof(uint8_t));
}

Display::~Display()
{
	delete[] pixels;
	delete[] buffer;
}

void Display::Commit()
{
	int nRows = height / 8;
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			int index = nRows * i + j / 8;
			int shift = j & 7;
			int mask = 1 << shift;
			int bit = buffer[index] & mask;
			pixels[j*width + i] = bit ? 0xffffffff : 0;
		}
	}
}

bool Display::DrawBitmap(int x, int y, const uint8_t* bitmap, int w, int h, bool doMask, int clip0, int clip1)
{
	if (!bitmap) return false;

	const int nRows = height / 8;
	const int texRows = (h + 7) / 8;

	const int x0 = MAX(MAX(clip0, 0), x);
	const int x1 = MIN(x + w, MIN(width, clip1));
	const int dx = x1 - x0;
	if (dx <= 0) return false;

	const int r0 = MAX(0, y / 8);
	const int r1 = MIN((y + h + 7) / 8, height / 8);
	const int dr = r1 - r0;
	const unsigned shift = (y+256) - ((y+256) / 8) * 8;
	const unsigned downShift = 8 - shift;
	if (dr <= 0) return false;

	for (int r = r0; r < r1; ++r) {
		mask[r] = doMask ? 0xff : 0;
	}

	static const uint8_t LOW[8] = {
		0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80
	};
	static const uint8_t HIGH[8] = {
		0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x7, 0x3, 0x1
	};

	if (doMask) {
		int trim = y - r0 * 8;
		mask[r0] = LOW[trim];

		trim = (r1)* 8 - (y + h);
		mask[r1 - 1] &= HIGH[trim];

		if (r0 == r1 && mask[r0] == 0)
			return false;
	}

	const uint8_t* src = bitmap + (x0-x) * texRows;
	for (int i = x0; i < x1; ++i) {
		uint8_t* dst = buffer + i * nRows + r0;
		for (int r = 0; r < dr; ++r) {
			*dst = *dst & (~mask[r + r0]);
			*dst |= *(src + r) << shift;
			if (r) {
				*dst |= *(src + r - 1) >> downShift;
			}
			dst++;
		}
		src += texRows;
	}
	return true;
}


bool Display::DrawStr(const char* str, int x, int y, glyphMetrics metrics, int clip0, int clip1)
{
	int cx = x;
	bool didRender = false;
	while (str && *str) {
		int advance = 0, texW = 0, texR = 0;
		const uint8_t* mem = metrics(*str, &advance, &texW, &texR);
		int texH = texR * 8;
		bool render = DrawBitmap(cx, y, mem, texW, texH, false, clip0, clip1);
		didRender = didRender || render;
		cx += advance;

		++str;
	}
	return didRender;
}

void Display::Fill(int c)
{
	uint8_t* dst = buffer;
	uint8_t value = c ? 0xff : 0;

	const int nRows = height / 8;
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < nRows; ++j) {
			*dst++ = value;
		}
	}
}
