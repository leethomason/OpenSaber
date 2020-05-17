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

#ifndef GRINLIZ_DOTSTAR_INCLUDED
#define GRINLIZ_DOTSTAR_INCLUDED

#include <stdint.h>

#include "rgb.h"

class DotStar
{
public:
	DotStar();

	void beginSPI(uint8_t enablePin);
	void beginSW(uint8_t clockPin, uint8_t dataPin);

	// Brightness is global; 0-256/255
	void display(const osbr::RGB* leds, int nLEDs, uint16_t brightness);
	// Brightness in the alpha channel.
	void display(const osbr::RGBA* leds, int nLEDs);

	bool swMode() const { return m_clockPin || m_dataPin; }
	uint8_t swClockPin() const { return m_clockPin; }
	uint8_t swDataPin() const { return m_dataPin; }

private:
	void swOut(uint8_t n);

  void begin();
  void transfer(uint8_t data);
  void end();

	uint8_t 	m_enable = 0;
	uint8_t		m_clockPin = 0;
	uint8_t		m_dataPin = 0;
};

#endif // GRINLIZ_DOTSTAR_INCLUDED
