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

#ifndef RGB_TO_HSV_INCLUDED
#define RGB_TO_HSV_INCLUDED

#include <stdint.h>

// hue:        [0, 180)
// saturation: [0, 255]
// value:      [0, 255]

void rgb2hsv(
    uint8_t src_r, uint8_t src_g, uint8_t src_b,
    uint8_t* dst_h, uint8_t* dst_s, uint8_t* dst_v);

void hsv2rgb(
    uint8_t src_h, uint8_t src_s, uint8_t src_v,
    uint8_t* dst_r, uint8_t* dst_g, uint8_t* dst_b);


#endif // RGB_TO_HSV_INCLUDED

