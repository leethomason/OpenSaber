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

#ifndef CSTR_INCLUDED
#define CSTR_INCLUDED

#include <Arduino.h>
#include "Grinliz_Util.h"

class SPISettings;

class LEDManager;
typedef void (*BlinkHandler)(const LEDManager&);

class LEDManager
{
public:
	enum {
		BLINK_TRAILING,
		BLINK_LEADING,
        BLINK_BREATH
	};

    LEDManager(uint8_t pin, bool on=true);

    void set(bool on);
    bool isOn() const;

    void blink(uint32_t currentTime, uint8_t n, uint32_t cycle, BlinkHandler handler = 0, uint8_t style = BLINK_LEADING);
    int  numBlinks(uint32_t currentTime) const;
    bool blinking() const {
        return m_nBlink > 0;
    }

    void process(uint32_t currentTime);
    uint8_t pin() const { return m_pin; }

private:
    uint8_t  m_pin = 0;
    uint8_t  m_nBlink = 0;
    bool 	 m_on = false;
    uint8_t  m_style = BLINK_LEADING;
    uint32_t m_cycle = 0;
    uint32_t m_startTime = 0;
    BlinkHandler m_handler = 0;
    uint8_t m_nCallbacks = 0;
    bool m_analogMode = true;   // Teensy has a bug; once it goes analog, it STAYS analog
};

// --- Interupts & Time --- //

struct ProfileData
{
    ProfileData(const char* _name) { 
        name = _name;
        next = root;
        root = this; 
    }

    const char* name = 0;
    ProfileData* next = 0;

    uint32_t nCalls;
    uint32_t totalTime;
    uint32_t maxTime;
    uint32_t startTime;

    static ProfileData* root;
};

void ProfileStart(ProfileData* data);
void ProfileEnd(ProfileData* data);
void DumpProfile();

class ProfileBlock
{
public:
    ProfileBlock(ProfileData* data) {
        m_data = data;
        ProfileStart(m_data);
    }

    ~ProfileBlock() {
        ProfileEnd(m_data);
    }

private:
    ProfileData* m_data;
};

#endif // CSTR_INCLUDED
