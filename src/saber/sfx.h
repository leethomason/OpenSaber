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

#include <Arduino.h>
#include "Grinliz_Arduino_Util.h"
#include "Grinliz_Util.h"

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

	void process(int bladeMode, uint32_t delta, bool* still);

	void setVolume(int v);
	int getVolume() const { return m_volume; }

	const uint32_t getIgniteTime() const { return m_igniteTime; }
	const uint32_t getRetractTime() const { return m_retractTime; }

	int setFont(int font);
	int setFont(const char *fontName);
	int currentFont() const { return m_currentFont; }

	bool smoothMode() const { return m_smoothMode; }
	void sm_setSwing(float radPerSec, int blend256) { m_speed = radPerSec; m_blend256 = blend256; }
	void sm_ignite();
	void sm_retract();
	bool sm_playEvent(int sfx);

protected:
	enum {
		CHANNEL_IDLE,
		CHANNEL_MOTION_0,
		CHANNEL_MOTION_1,
		CHANNEL_EVENT
	};

	void readIgniteRetract();
	void scanFiles();
	int calcSlot(const char* name);
	int getTrack(int sound);
	void playMotionTracks();

	static int sm_swingToVolume(float radPerSec);
	int scaleVolume(int v) const;

	I2SAudioDriver *m_driver;
	const Manifest& m_manifest;

	bool m_smoothMode;
	int m_currentSound;
	int m_currentFont;
	uint32_t m_igniteTime;
	uint32_t m_retractTime;
	int m_volume;

	float m_speed;
	int m_blend256;
	int m_swing = 0;		// tracks current swing with decay...lots of noise from mag
	int m_swingTarget = 0;	
	int m_hum = 0;			// also tracks with decay
	StepProp m_swingDecay;	// decay rate is fast; about 1.5/millisecond, so need fixed.
	int m_stillCount = 0;	// how many still frames (motion sound = 0)

	Random m_random;
	AnimateProp volumeEnvelope;

	struct SFXType {
		int start;
		int count;
	};
	SFXType m_sfxType[NUM_SFX_TYPES];

	static SFX *m_instance;
};

#endif // SFX_HEADER
