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

#ifndef SKETCHER_INCLUDED
#define SKETCHER_INCLUDED

#include <stdint.h>
#include "renderer.h"
#include "Grinliz_Util.h"
#include "saberUtil.h"
#include "rgb.h"
#include "uirenderdata.h"


/* Renders the UI to RGB LEDs */
class DotStarUI
{
public:
    void Draw(osbr::RGB* uiLedStart,    // target LEDs
              int nLED,                 // must be 4 or 6
              uint32_t time, 
              UIMode mode, bool bladeIgnited, const UIRenderData& data) const;

	static bool Test();

private:
    void DrawVolume(osbr::RGB* led, int nLED, uint32_t time, int vol04) const;
};


/* Renders the UI to a 7x5 dot matrix (90 turn; usually they are 7 high, 
   this uses them as 7 wide.)
 */
class Pixel_7_5_UI
{
public:
    Pixel_7_5_UI();
    void Draw(uint32_t time, UIMode mode, bool bladeIgnited, const UIRenderData* data);

    // Returns columns.
    // So this will a pointer to 7 bytes.
	const uint8_t* Pixels() { return m_col; }

private:
	void DrawBar(int x, int y);
	void DrawDot(int x, int y);

	static const int NROWS = 5;
	static const int NCOLS = 7;
    static const int ALLOCATED = NCOLS + 1;
    uint8_t m_col[ALLOCATED] = { 0 };		// enough memory for writing the glyphs 4+4 bytes
};


/* Renders the UI to a 4 7-segment displays. */
class Digit4UI
{
public:
	Digit4UI() {}

	void Draw(UIMode mode, const UIRenderData* data);
	const CStr<9>& Output() { return m_output; }

private:
	CStr<9> m_output;
};

void calcCrystalColorHSV(uint32_t msec, const osbr::RGB& base, osbr::RGB* out);

#endif // SKETCHER_INCLUDED
