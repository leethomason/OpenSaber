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

	void attachLEDs(const RGB* leds, int nLEDs) {
		m_leds = leds;
		m_nLEDs = nLEDs;
	}

	void setBrightness(uint16_t value256)	{ m_brightness = value256; }
	uint16_t brightness() const				{ return m_brightness; }

	void display();

	bool swMode() const { return m_clockPin || m_dataPin; }
	uint8_t swClockPin() const { return m_clockPin; }
	uint8_t swDataPin() const { return m_dataPin; }

private:
	void displaySPI();
	void displaySW();
	void swOut(uint8_t n);

	uint8_t 	m_enable = 0;
	uint8_t		m_clockPin = 0;
	uint8_t		m_dataPin = 0;
	const RGB* 	m_leds = 0;
	int 		m_nLEDs = 0;
	uint16_t 	m_brightness = 256;
};

#endif // GRINLIZ_DOTSTAR_INCLUDED
