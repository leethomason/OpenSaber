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

#include "dotstar.h"
#include <Arduino.h>
#include <SPI.h>

using namespace osbr;

// 8 MHz seems to work for the prop shield.
// With a level shifter, about 1 MHz is needed. (Possibly faster with testing.)
static const SPISettings dotstarSettings(8000000, MSBFIRST, SPI_MODE0);

DotStar::DotStar()
{
}

void DotStar::beginSPI(uint8_t enable)
{
	m_enable = enable;
	pinMode(m_enable, OUTPUT);
	digitalWrite(m_enable, LOW);
	SPI.begin();
}

void DotStar::beginSW(uint8_t clockPin, uint8_t dataPin)
{
	m_clockPin = clockPin;
	m_dataPin = dataPin;

	pinMode(m_dataPin , OUTPUT);
	pinMode(m_clockPin, OUTPUT);
	digitalWrite(m_dataPin , LOW);
	digitalWrite(m_clockPin, LOW);	
}

void DotStar::begin() const
{
    if (swMode()) {
    }
    else {
        SPI.beginTransaction(dotstarSettings);
    }
}


void DotStar::transfer(uint8_t data) const
{
    if (swMode()) swOut(data);
    else SPI.transfer(data);
}

void DotStar::transferColor(uint8_t r, uint8_t g, uint8_t b)const
{
    if (m_config == RBG) {
        transfer(r);
        transfer(b);
        transfer(g);
    }
    else {
        transfer(b);
        transfer(g);
        transfer(r);
    }
}

void DotStar::end() const
{
    if (swMode()) {
    }
    else {
        SPI.endTransaction();
    }
}


void DotStar::swOut(uint8_t n) const
{
	for(uint8_t i=8; i--; n <<= 1) {
    	if(n & 0x80) 
    		digitalWrite(m_dataPin, HIGH);
    	else         
    		digitalWrite(m_dataPin, LOW);

    	digitalWrite(m_clockPin, HIGH);
    	digitalWrite(m_clockPin, LOW);
	}
}

void DotStar::display(const osbr::RGB* led, int nLEDs, uint16_t brightness) const
{
    begin();
	if (m_enable != 255)
		digitalWrite(m_enable, HIGH);

	// Frame marker.
	for(int i=0; i<4; ++i) {
        transfer(0);
	}

    // RGB, global brightness.
    // Brightness is 5 bits; 0-31
    // Will be fully on at either 255 or 256
    uint8_t bright = (brightness > 255) ? 31 : (brightness >> 3);
    bright |= 0xe0;

    for (int i = 0; i < nLEDs; ++i, ++led) {
        // Brightness
        transfer(bright);

        // Color
        transferColor(led->r, led->g, led->b);
    }
	
    // End frame.
	for(int i=0; i<4; ++i) {
		transfer(0xff);
	}

	if (m_enable != 255)
		digitalWrite(m_enable, LOW);
    end();
}


void DotStar::display(const osbr::RGBA* led, int nLEDs) const
{
    begin();
	if (m_enable != 255)
		digitalWrite(m_enable, HIGH);

	// Frame marker.
	for(int i=0; i<4; ++i) {
        transfer(0);
	}

    // RGBA, per-LED brightness.
    for (int i = 0; i < nLEDs; ++i, ++led) {
        // Brightness is 5 bits; 0-31 with high bits always set.
        uint8_t bright = (led->a >> 3) | 0xe0;
        transfer(bright);
        transferColor(led->r, led->g, led->b);
    }

	// End frame.
	for(int i=0; i<4; ++i) {
		transfer(0xff);
	}

	if (m_enable != 255)
		digitalWrite(m_enable, LOW);
    end();
}
