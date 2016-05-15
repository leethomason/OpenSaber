#ifndef GRINLIZ_DOTSTAR_INCLUDED
#define GRINLIZ_DOTSTAR_INCLUDED

#include <stdint.h>

class DotStar
{
public:
	struct RGB {
		uint8_t red;
		uint8_t green;
		uint8_t blue;

		void set(uint32_t c) {
			red = (c>>16) & 0xff;
			green = (c>>8) & 0xff;
			blue = c & 0xff;
		}
	};

	DotStar(uint8_t enablePin);
	void begin();

	void attachLEDs(const RGB* leds, int nLEDs) {
		m_leds = leds;
		m_nLEDs = nLEDs;
	}

	void setBrightness(uint16_t value256)	{ m_brightness = value256; }
	uint16_t brightness() const				{ return m_brightness; }

	void display();

private:
	uint8_t 	m_enable = 0;
	const RGB* 	m_leds = 0;
	int 		m_nLEDs = 0;
	uint16_t 	m_brightness = 256;
};

#endif // GRINLIZ_DOTSTAR_INCLUDED
