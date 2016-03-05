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
     bool bladeOn() const;
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

class LEDManager
{
public:
	LEDManager(uint8_t pin);

	void set(bool on);
	void blink(uint8_t n, uint32_t cycle);
	void process();
	int  numBlinks() const;
	bool blinking() const { return m_nBlink > 0; }

private:
	uint8_t  m_pin = 0;
	uint8_t  m_nBlink = 0;
	bool 	 m_on = false;
	uint32_t m_cycle = 0;
	uint32_t m_startTime = 0;
};

#endif // SABER_UTIL_INCLUDED
