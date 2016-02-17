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

void Display::DrawBitmap(int x, int y, const uint8_t* bitmap, int w, int h)
{
	const int nRows = height / 8;
	const int bitmapRows = h / 8;

	const int x0 = MAX(0, x);
	const int x1 = MIN(x + w, width);
	const int dx = x1 - x0;

	for (int i = x0; i < x1; ++i) {
		uint8_t* dst = buffer + i * nRows;
		const uint8_t* src = bitmap + (i - x) * bitmapRows + (bitmapRows - 1);
		for (int r = 0; r < bitmapRows; ++r) {
			*dst++ = *src--;
		}
	}
}
