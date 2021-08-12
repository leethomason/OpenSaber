/*
  Copyright (c) Lee Thomason, Grinning Lizard Software

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
  of the Software, and to permit persons to whom the Software is furnished to do
  so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "renderer.h"
#include "./src/util/Grinliz_Util.h"
#include <stdint.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

void Renderer::Attach(int w, int h, uint8_t* buf)
{
	ASSERT(buf);

	m_width = w;
	m_height = h;
	m_buffer = buf;
	m_nRows = m_height / 8;

	ASSERT(m_nRows <= MAX_ROWS);
}

bool Renderer::DrawBitmap(int x, int y, TextureData data, int flags, int clip0, int clip1)
{
	int texW = 0;
	int texH = 0;
	const uint8_t* tex = data(&texW, &texH);
	ASSERT(tex);
	return DrawBitmap(x, y, tex, texW, texH, flags, clip0, clip1);
}

bool Renderer::DrawBitmap(int x, int y, const uint8_t* tex, int texW, int texH, int flags, int clip0, int clip1)
{
	if (!tex) return false;
	ASSERT(m_buffer);
	if (!m_buffer) return false;

	const int texRows = (texH + 7) / 8;

	const int x0 = MAX(MAX(clip0, 0), x);
	const int x1 = MIN(x + texW, MIN(m_width, clip1));
	const int dx = x0 - x;
	if (x1 <= x0) return false;

	const int r0 = (y+256) / 8 - 32;
	const int r1 = (y + texH + 7) / 8;
	const unsigned shift = (y+256) - ((y+256) / 8) * 8;
	const unsigned downShift = 8 - shift;

	for (int r = MAX(r0, 0); r < MIN(r1, m_nRows); ++r) {
		m_mask[r] = (flags & NO_MASK) ? 0 : 0xff;
	}

	if (!(flags & NO_MASK)) {
		CalcMask(y, texH, 0, 0);
		if (r0 == r1 && m_mask[r0] == 0)
			return false;
	}

	const uint8_t* src = tex;
	const uint8_t* texEnd = tex + texRows * texW;
	(void)src;

	for (int r = r0; r < r1; ++r) {
		if (r < 0 || r >= m_nRows) continue;

		uint8_t* dst = m_buffer + x0 + r * m_width;
		ASSERT(dst >= m_buffer && dst < m_buffer + m_nRows * m_width);
		const uint8_t* src = tex + (r - r0) * texW + dx;
		int bias = 1;
		if (flags & FLIP_X) {
			dst = m_buffer + (x1 - 1) + r * m_width;
			bias = -1;
		}

		for (int i = x0; i < x1; ++i) {

			*dst = *dst & (~m_mask[r]);
			if (src < texEnd) {
				ASSERT(src >= tex && src < tex + texRows * texW);
				ASSERT(dst >= m_buffer && dst < m_buffer + m_nRows * m_width);
				*dst |= *src << shift;
			}
			if (r > r0) {
				ASSERT((src - texW) >= tex && (src - texW) < tex + texRows * texW);
				ASSERT(dst >= m_buffer && dst < m_buffer + m_nRows * m_width);
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
	int x1 = MIN(x + w, m_width);

	for (int r = r0; r < r1; ++r) {
		if (r < 0) continue;
		if (r >= m_nRows) continue;

		uint8_t* dst = m_buffer + x0 + r * m_width;
		for (int x = x0; x < x1; ++x) {
			ASSERT(dst >= m_buffer && dst < m_buffer + m_nRows * m_width);
			*dst++ |= m_mask[r];
		}
	}
}


void Renderer::CalcMask(int y, int h, int* pr0, int* pr1)
{
	static const uint8_t LOWBYTES[8] = {
		0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80
	};
	static const uint8_t HIGHBYTES[8] = {
		0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x7, 0x3, 0x1
	};

	const int r0 = (y+256) / 8 - 32;
	const int r1 = (y + h + 7) / 8;

	if (pr0) *pr0 = r0;
	if (pr1) *pr1 = r1;

	ASSERT(m_nRows <= MAX_ROWS);
	for (int r = MAX(r0, 0); r < MIN(r1, m_nRows); ++r) {
		m_mask[r] = 0xff;
	}

	if (r0 >= 0) {
		int trim = y - r0 * 8;
		m_mask[r0] &= LOWBYTES[trim];
	}
	if (r1 <= m_nRows)  {
		int trim = r1 * 8 - (y + h);
		m_mask[r1 - 1] &= HIGHBYTES[trim];
	}
}

bool Renderer::DrawStr(const char* str, int x, int y, GlyphMetrics metrics, int clip0, int clip1)
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


int Renderer::StrWidth(const char* str, GlyphMetrics metrics)
{
	int w = 0;
	int d0 = 0, d1 = 0;
	while (str && *str) {
		int advance = 0;
		metrics(*str, &advance, &d1, &d0);
		w += advance;
		++str;
	}
	return w;
}


void Renderer::Fill(int c)
{
	if (m_buffer) {
		const int size = m_nRows * m_width;
		const uint8_t val = c ? 0xff : 0;

		for (int i = 0; i < size; ++i) {
			m_buffer[i] = val;
		}
	}
}
