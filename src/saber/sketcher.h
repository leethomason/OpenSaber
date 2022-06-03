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

#include "rgb.h"

void calcCrystalColorHSV(uint32_t msec, const osbr::RGB& base, osbr::RGB* out);

#if false

#include <stdint.h>
#include "renderer.h"
#include "./src/util/Grinliz_Util.h"
#include "saberUtil.h"
#include "uirenderdata.h"

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

#endif

#endif // SKETCHER_INCLUDED
