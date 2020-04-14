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

#ifndef DRAW_INCLUDED
#define DRAW_INCLUDED

#include <stdint.h>

const uint8_t* get_jBird(int* width, int* height);

// Created using the DotFactory.
// Fonts:
//		Y: fixed
//		X: tightest
//		Bit layout: Column Major
//		Bit order: MSB first
// Images:
//		Bit layout: Column Major
//		Bit order: MSB first

const uint8_t* getGlypth_aurekBesh6(int charID, int* advance, int* w, int* rows);
const uint8_t* getGlypth_calibri8(int charID, int* advance, int* w, int* rows);

void getGlypth_tomThumb5(int charID, uint8_t* glyph, int* advance=0, int* w=0, int* rows=0);
const uint8_t* getGlypth_tomThumb5(int charID, int* advance, int* w, int* rows);

#endif // DRAW_INCLUDED
