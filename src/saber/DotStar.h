#ifndef GRINLIZ_DOTSTAR_INCLUDED
#define GRINLIZ_DOTSTAR_INCLUDED

#include <stdint.h>

struct RGB {
	enum {
		BLACK = 0
	};

	RGB() {}
	RGB(uint32_t c) { set(c); }
	RGB(uint8_t r, uint8_t g, uint8_t b) { set(r, g, b); }

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

	void set(uint8_t _r, uint8_t _g, uint8_t _b) {
		r = _r; g = _g; b = _b;
	}

	void set(uint32_t c) {
		r = (c & 0xff0000) >> 16;
		g = (c & 0xff00) >> 8;
		b = c & 0xff;
	}

	void set(int index, uint8_t c) {
		*(&r + index) = c;
	}

	uint32_t get() const {
		return (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
	}
	uint8_t get(int index) const {
		return *(&r + index);
	}

    void scale(uint16_t s) {
        r = (uint16_t(r) * uint16_t(s)) >> 8;
        g = (uint16_t(g) * uint16_t(s)) >> 8;
        b = (uint16_t(b) * uint16_t(s)) >> 8;
    }

	uint8_t operator[](const int index) const {
		return *(&r + index);
	}

    uint8_t& operator[](const int index) {
        return *(&r + index);
    }

	bool operator==(const RGB& rhs) const {
		return (r == rhs.r) && (g == rhs.g) && (b == rhs.b);
	}
};


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
