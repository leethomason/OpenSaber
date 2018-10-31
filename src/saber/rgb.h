#ifndef GRINLIZ_SABER_RGB_INCLUDED
#define GRINLIZ_SABER_RGB_INCLUDED

#include <stdint.h>

namespace osbr {

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

    uint8_t average() const {
        return (int(r) + int(g) + int(b)) / 3;
    }

	// From 0-256
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

	bool operator!=(const RGB& rhs) const {
		return !(rhs == *this);
	}

	int size() const { return 3; }
};
}	// namespace osbr
#endif