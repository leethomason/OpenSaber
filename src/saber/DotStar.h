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
