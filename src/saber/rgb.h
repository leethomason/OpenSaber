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

#ifndef GRINLIZ_SABER_RGB_INCLUDED
#define GRINLIZ_SABER_RGB_INCLUDED

#include <stdint.h>

namespace osbr {

struct RGB {
	enum {
        // channels
        RED = 0,
        GREEN = 1,
        BLUE = 2,

        // 32 bit color
		BLACK = 0
	};

    RGB() {}
    RGB(const RGB& other) { r = other.r; g = other.g; b = other.b; }
    RGB(uint8_t _r, uint8_t _g, uint8_t _b) {
        r = _r; g = _g; b = _b;
    }
    RGB(uint32_t c) { set(c); }
    RGB& operator=(const RGB& rhs) = default;

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

	void set(uint8_t _r, uint8_t _g, uint8_t _b) {
		r = _r; g = _g; b = _b;
	}

    void setWhite(uint8_t _w) {
        r = g = b = _w;
    }

	void set(uint32_t c) {
		r = (c & 0xff0000) >> 16;
		g = (c & 0xff00) >> 8;
		b = c & 0xff;
	}

	uint32_t get() const {
		return (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
	}

    uint8_t average() const {
        return (int(r) + int(g) + int(b)) / 3;
    }

    int32_t sum() const { return int32_t(r) + int32_t(g) + int32_t(b);}

	// From 0-256
    void scale(uint16_t s) {
        r = (uint16_t(r) * s) >> 8;
        g = (uint16_t(g) * s) >> 8;
        b = (uint16_t(b) * s) >> 8;
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

    int size() const { return 3; } // number of components
};

struct RGBA {
    enum {
        // channels
        RED = 0,
        GREEN = 1,
        BLUE = 2,
        ALPHA = 3,

        // 32 bit color
		BLACK = 0
    };

    RGBA() {}
    RGBA(const RGBA& other) { r = other.r; g = other.g; b = other.b; a = other.a; }
    RGBA(const RGB& other, uint8_t alpha) { r = other.r; g = other.g; b = other.b; a = alpha; }
    RGBA(uint32_t c) { set(c); }
    RGBA(int r, int g, int b, int a = 255) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
    RGBA& operator=(const RGBA& rhs) = default; 

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;

    void set(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) {
        r = _r; g = _g; b = _b; a = _a;
    }

    void set(uint32_t c) {
        r = (c & 0xff0000) >> 16;
        g = (c & 0xff00) >> 8;
        b = c & 0xff;
        a = (c & 0xff000000) >> 24;
    }

    void set(const RGB& rgb, uint8_t alpha) {
        r = rgb.r;
        g = rgb.g;
        b = rgb.b;
        a = alpha;
    }

    uint32_t get() const {
        return (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b) | (uint32_t(a) << 24);
    }

    RGB rgb() const {
        RGB c(r, g, b);
        return c;
    }

    // From 0-256
    void scale(uint16_t s) {
        r = (uint16_t(r) * s) >> 8;
        g = (uint16_t(g) * s) >> 8;
        b = (uint16_t(b) * s) >> 8;
    }

    uint8_t operator[](const int index) const {
        return *(&r + index);
    }

    uint8_t& operator[](const int index) {
        return *(&r + index);
    }

    bool operator==(const RGBA& rhs) const {
        return (r == rhs.r) && (g == rhs.g) && (b == rhs.b) && (a == rhs.a);
    }

    bool operator!=(const RGBA& rhs) const {
        return !(rhs == *this);
    }

    int size() const { return 4; } // number of components
};


static_assert(sizeof(RGB) <= 4, "RGB be 3 or 4 bytes.");
static_assert(sizeof(RGBA) == 4, "RGBA should be 4 bytes.");


}	// namespace osbr
#endif