#ifndef SABER_UTIL_INCLUDED
#define SABER_UTIL_INCLUDED

#include <stdint.h>

enum {  
	BLADE_OFF,
    BLADE_IGNITE,
    BLADE_ON,
    BLADE_FLASH,
    BLADE_RETRACT
 };

class BladeState
{
public:
	BladeState() {}

     void change(uint8_t newState);
     const int state() const { return m_currentState; }

     // Any of the blade-on states, not just the BLADE_ON idles state.
     bool bladeOn() const;
     bool bladeOff() const { return m_currentState == BLADE_OFF; }

     const uint32_t startTime() const { return m_startTime; }

private:
	uint8_t  m_currentState = BLADE_OFF;
	uint32_t m_startTime = 0;
};

enum {
	BUTTON_MODE_NORMAL,
	BUTTON_MODE_PALETTE,
	BUTTON_MODE_VOLUME,
	NUM_BUTTON_MODES
};

class ButtonMode
{
public:
	ButtonMode() {}

	void nextMode();
	const int mode() const { return m_mode; }

private:
	uint8_t m_mode = BUTTON_MODE_NORMAL;
};


#endif // SABER_UTIL_INCLUDED
