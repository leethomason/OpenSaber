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

#ifndef SFX_HEADER
#define SFX_HEADER

#include "sfxcalc.h"
#include "bladestate.h"

#include <Arduino.h>
#include "./src/util/Grinliz_Arduino_Util.h"
#include "./src/util/Grinliz_Util.h"

class I2SAudioDriver;
class Manifest;

// SFX in priority order!
enum
{			
	SFX_IDLE,
	SFX_MOTION,	
	SFX_MOTION_HIGH,	// if present, a smooth swing font, and nHigh = nLow
	SFX_IMPACT,
	SFX_USER_TAP,
	SFX_POWER_ON,
	SFX_POWER_OFF,

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

	bool playSound(int sfx, int mode, int channel=0);
	bool playSound(const char *sfx);
	void stopSound();

	// Returns "true" for a stillReset - the blade is both still, and sounds need to be reset.
	bool process(BladeState state, uint32_t delta);

	void setVolume(int v256);
	int getVolume() const { return m_volume; }
	void setBoost(int boost, int channel);

	uint32_t getIgniteTime() const { return m_igniteTime; }
	uint32_t getRetractTime() const { return m_retractTime; }

	int setFont(int font);
	int setFont(const char *fontName);
	int currentFont() const { return m_currentFont; }

	bool smoothMode() const { return m_smoothMode; }

	// radPerSec is straightforward how fast the blade is moving
	// blend256 is the blending between sounds, from 0-256. Some interpretation of
	//          a normal. For example: (int)((1.0f + dot)*128.0f)
	void sm_setSwing(float radPerSec, int blend256) { m_radPerSec = radPerSec; m_blend256 = blend256; }
	void sm_ignite();
	void sm_retract();
	bool sm_playEvent(int sfx);

protected:
	enum {
		CHANNEL_IDLE,
		CHANNEL_MOTION_0,
		CHANNEL_MOTION_1,
		CHANNEL_EVENT,
		N_CHANNEL
	};

	void readIgniteRetract();
	void scanFiles();
	int calcSlot(const char* name);
	int getTrack(int sound);
	void playMotionTracks();
	int scaleVolume(int v) const { return v * m_volume / 256; }

	I2SAudioDriver *m_driver;
	const Manifest& m_manifest;

	bool m_smoothMode;
	int m_currentSound;
	int m_currentFont;
	uint32_t m_igniteTime;
	uint32_t m_retractTime;
	int m_volume;				// typically 0-256, does support higher

	float m_radPerSec = 0;		// set by sm_setSwing()
	int m_blend256 = 0;			// set by sm_setSwing()

	int m_boost[N_CHANNEL] = {256, 256, 256, 256};
	Random m_random;
	SFXCalc sfxCalc;

	struct SFXType {
		int start;
		int count;
	};
	SFXType m_sfxType[NUM_SFX_TYPES];


	static SFX *m_instance;
};

#endif // SFX_HEADER
