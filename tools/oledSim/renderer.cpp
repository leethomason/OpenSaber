#include "renderer.h"
#include <string.h>

#if defined(_DEBUG) && defined(_MSC_VER)
	#define ASSERT( x )		if ( !(x)) { _asm { int 3 } }
#endif

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

Renderer::Renderer()
{
	width = 0;
	height = 0;
	buffer = 0;
	nRows = 0;
}

void Renderer::Attach(int w, int h, uint8_t* buf)
{
	width = w;
	height = h;
	buffer = buf;
	nRows = height / 8;
}

bool Renderer::DrawBitmap(int x, int y, textureData data, int flags, int clip0, int clip1)
{
	int texW = 0;
	int texH = 0;
	const uint8_t* tex = data(&texW, &texH);
	return DrawBitmap(x, y, tex, texW, texH, flags, clip0, clip1);
}

bool Renderer::DrawBitmap(int x, int y, const uint8_t* tex, int texW, int texH, int flags, int clip0, int clip1)
{
	if (!tex) return false;

	const int texRows = (texH + 7) / 8;

	const int x0 = MAX(MAX(clip0, 0), x);
	const int x1 = MIN(x + texW, MIN(width, clip1));
	const int dx = x0 - x;
	if (x1 <= x0) return false;

	const int r0 = (y+256) / 8 - 32;
	const int r1 = (y + texH + 7) / 8;
	const unsigned shift = (y+256) - ((y+256) / 8) * 8;
	const unsigned downShift = 8 - shift;

	for (int r = MAX(r0, 0); r < MIN(r1, nRows); ++r) {
		mask[r] = (flags & NO_MASK) ? 0 : 0xff;
	}

	if (!(flags & NO_MASK)) {
		CalcMask(y, texH, 0, 0);
		if (r0 == r1 && mask[r0] == 0)
			return false;
	}

	const uint8_t* src = tex;
	const uint8_t* texEnd = tex + texRows * texW;

	for (int r = r0; r < r1; ++r) {
		if (r < 0 || r >= nRows) continue;

		uint8_t* dst = buffer + x0 + r * width;
		ASSERT(dst >= buffer && dst < buffer + nRows * width);
		const uint8_t* src = tex + (r - r0) * texW + dx;
		int bias = 1;
		if (flags & FLIP_X) {
			dst = buffer + (x1 - 1) + r * width;
			bias = -1;
		}

		for (int i = x0; i < x1; ++i) {

			*dst = *dst & (~mask[r]);
			if (src < texEnd) {
				ASSERT(src >= tex && src < tex + texRows * texW);
				ASSERT(dst >= buffer && dst < buffer + nRows * width);
				*dst |= *src << shift;
			}
			if (r > r0) {
				ASSERT((src - texW) >= tex && (src - texW) < tex + texRows * texW);
				ASSERT(dst >= buffer && dst < buffer + nRows * width);
				*dst |= *(src - texW) >> downShift;
			}
			dst += bias;
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

	for (int r = r0; r < r1; ++r) {
		if (r < 0) continue;
		if (r >= nRows) continue;

		uint8_t* dst = buffer + x0 + r * width;
		for (int x = x0; x < x1; ++x) {
			ASSERT(dst >= buffer && dst < buffer + nRows * width);
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

	const int r0 = (y+256) / 8 - 32;
	const int r1 = (y + h + 7) / 8;

	if (pr0) *pr0 = r0;
	if (pr1) *pr1 = r1;

	for (int r = MAX(r0, 0); r < MIN(r1, nRows); ++r) {
		mask[r] = 0xff;
	}

	if (r0 >= 0) {
		int trim = y - r0 * 8;
		mask[r0] &= LOW[trim];
	}
	if (r1 <= nRows)  {
		int trim = r1 * 8 - (y + h);
		mask[r1 - 1] &= HIGH[trim];
	}
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
	memset(buffer, c ? 0xff : 0, nRows * width);
}
