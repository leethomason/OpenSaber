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
	const int bitmapRows = (h+7) / 8;

	const int x0 = MAX(0, x);
	const int x1 = MIN(x + w, width);
	const int dx = x1 - x0;

	const int r0 = y / 8;
	const int r1 = (y + h) / 8;
	for (int r = r0; r < r1; ++r) {
		mask[r] = 0xff;
	}


	const uint8_t* src = bitmap + (x0-x) * bitmapRows;// +(i - x) * bitmapRows + (bitmapRows - 1);
	for (int i = x0; i < x1; ++i) {
		uint8_t* dst = buffer + i * nRows;
		for (int r = 0; r < bitmapRows; ++r) {
			*dst++ = *src++;
		}
	}
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
