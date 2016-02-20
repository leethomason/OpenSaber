#include "renderer.h"
#include <string.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

Renderer::Renderer()
{
	width = 0;
	height = 0;
	buffer = 0;
}

void Renderer::Attach(int w, int h, uint8_t* buf)
{
	width = w;
	height = h;
	buffer = buf;
}

bool Renderer::DrawBitmap(int x, int y, textureData data, int flags, int clip0, int clip1)
{
	int texW = 0;
	int texH = 0;
	const uint8_t* tex = data(&texW, &texH);
	return DrawBitmap(x, y, tex, texW, texH, flags, clip0, clip1);
}

bool Renderer::DrawBitmap(int x, int y, const uint8_t* bitmap, int w, int h, int flags, int clip0, int clip1)
{
	if (!bitmap) return false;

	const int nRows = height / 8;
	const int texRows = (h + 7) / 8;

	const int x0 = MAX(MAX(clip0, 0), x);
	const int x1 = MIN(x + w, MIN(width, clip1));
	if (x1 <= x0) return false;

	const int r0 = MAX(0, y / 8);
	const int r1 = MIN((y + h + 7) / 8, nRows);	// r1 exclusive; higher.
	const int dr = r1 - r0;
	const unsigned shift = (y+256) - ((y+256) / 8) * 8;
	const unsigned downShift = 8 - shift;
	if (dr <= 0) return false;

	for (int r = r0; r < r1; ++r) {
		mask[r] = (flags & NO_MASK) ? 0 : 0xff;
	}

	if (!(flags & NO_MASK)) {
		CalcMask(y, h, 0, 0);
		if (r0 == r1 && mask[r0] == 0)
			return false;
	}

	const uint8_t* src = bitmap;
	const int maxTexR = (y + h) / 8;

	for (int r = r0; r < r1; ++r) {
		uint8_t* dst = buffer + x0 + r * width;
		for (int i = x0; i < x1; ++i) {
			if (flags & FLIP_X) {
				dst = buffer + (x1 - 1 - (i - x0)) * nRows;
			}
			*dst = *dst & (~mask[r]);
			if (r < maxTexR) {
				*dst |= *src << shift;
			}
			if (r > r0) {
				*dst |= *(src - w) >> downShift;
			}
			dst++;
			src++;
		}
	}
	return true;
}


void Renderer::DrawRectangle(int x, int y, int w, int h)
{
	int r0 = 0, r1 = 0;
	CalcMask(y, h, &r0, &r1);
	int x0 = MAX(x, 0);
	int x1 = MIN(x + w, width);
	const int nRows = height / 8;

	for (int x = x0; x < x1; ++x) {
		uint8_t* dst = buffer + x * nRows + r0;
		for (int r = r0; r < r1; ++r) {
			*dst++ |= mask[r];
		}
	}
}


void Renderer::CalcMask(int y, int h, int* pr0, int* pr1)
{
	static const uint8_t LOW[8] = {
		0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80
	};
	static const uint8_t HIGH[8] = {
		0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x7, 0x3, 0x1
	};

	const int r0 = MAX(0, y / 8);
	const int r1 = MIN((y + h + 7) / 8, height / 8);

	if (pr0) *pr0 = r0;
	if (pr1) *pr1 = r1;

	int trim = y - r0 * 8;
	mask[r0] = LOW[trim];

	trim = (r1)* 8 - (y + h);
	mask[r1 - 1] &= HIGH[trim];
}

bool Renderer::DrawStr(const char* str, int x, int y, glyphMetrics metrics, int clip0, int clip1)
{
	int cx = x;
	bool didRender = false;
	while (str && *str) {
		int advance = 0, texW = 0, texR = 0;
		const uint8_t* mem = metrics(*str, &advance, &texW, &texR);
		int texH = texR * 8;
		bool render = DrawBitmap(cx, y, mem, texW, texH, NO_MASK, clip0, clip1);
		didRender = didRender || render;
		cx += advance;

		++str;
	}
	return didRender;
}

void Renderer::Fill(int c)
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
