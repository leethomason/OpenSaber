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

#ifndef DISPLAY_INCLUDED
#define DISPLAY_INCLUDED

#include <stdint.h>
#include "rgb.h"

typedef const uint8_t* (*GlyphMetrics)(int charID, int* advance, int* w, int* rows);
typedef const uint8_t* (*TextureData)(int* w, int* h);

// Renders to an OLED mono compatible buffer.
// Doesn't own the buffer - just writes to it.
class Renderer
{
public:
	Renderer() {}
	void Attach(int w, int h, uint8_t* buffer);

	enum {
		NO_MASK = 0x01,
		FLIP_X = 0x02
	};
	bool DrawBitmap(int x, int y, const uint8_t* bitmap, int w, int h,
					int flags = 0, int clip0 = 0, int clip1 = 256);
	bool DrawBitmap(int x, int y, TextureData data, 
					int flags = 0, int clip0 = 0, int clip1 = 256);
	bool DrawStr(const char* str, int x, int y, GlyphMetrics metrics, 
				 int clip0 = 0, int clip1 = 256);
	int StrWidth(const char* str, GlyphMetrics metrics);
	void DrawRectangle(int x, int y, int w, int h);

	void Fill(int c);

    // The buffer attached to this OLED renderer
	uint8_t* Buffer() { return m_buffer;  }

private:
	void CalcMask(int y, int h, int* r0, int* r1);

	int m_width	= 0;
	int m_height	= 0;
	int m_nRows	= 0;
	uint8_t* m_buffer = 0;

	enum { MAX_ROWS = 4};
	uint8_t m_mask[MAX_ROWS];
};


#endif // DISPLAY_INCLUDED
