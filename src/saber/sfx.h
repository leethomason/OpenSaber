/*
Copyright (c) 2016 Lee Thomason, Grinning Lizard Software

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

#ifndef SFX_HEADER
#define SFX_HEADER

#include <Arduino.h>
#include "Grinliz_Arduino_Util.h"
#include "Grinliz_Util.h"

class I2SAudioDriver;
class Manifest;

// SFX in priority order!
enum
{				//  Max
	SFX_IDLE,   //  1
	SFX_MOTION, //  16
	SFX_IMPACT,   //  16
	SFX_USER_TAP, //  4
	SFX_POWER_ON,  //  4
	SFX_POWER_OFF, //  4

	NUM_SFX_TYPES,
	SFX_NONE = 255
};

enum
{
	SFX_GREATER,
	SFX_GREATER_OR_EQUAL,
	SFX_OVERRIDE
};

class SFX
{
public:
	// AudioPlayer can be null to avoid a bunch of #if/if
	SFX(I2SAudioDriver *driver, const Manifest& manifest);
	static SFX *instance() { return m_instance; }

	bool playSound(int sfx, int mode);
	bool playSound(const char *sfx);
	void stopSound();

	bool bladeOn() const { return m_bladeOn; }
	void process(bool playIdleSound);

	void setVolume(int v) { m_volume = glClamp(v, 0, 256);}
	int getVolume() const { return m_volume; }

	int lastSFX() const { return m_lastSFX; }

	const uint32_t getIgniteTime() const { return m_igniteTime; }
	const uint32_t getRetractTime() const { return m_retractTime; }

	int setFont(int font);
	int setFont(const char *fontName);
	int currentFont() const { return m_currentFont; }

protected:
	void readIgniteRetract();
	void scanFiles();
	int calcSlot(const char* name);

	I2SAudioDriver *m_driver;
	const Manifest& m_manifest;

	bool m_bladeOn;
	int m_currentSound; // For smooth, refers to the current effect sound.
	int m_lastSFX;
	int m_currentFont;
	uint32_t m_igniteTime;
	uint32_t m_retractTime;
	int m_volume;

	Random m_random;

	struct SFXType {
		int start;
		int count;
	};
	SFXType m_sfxType[NUM_SFX_TYPES];

	static SFX *m_instance;
};

#endif // SFX_HEADER
