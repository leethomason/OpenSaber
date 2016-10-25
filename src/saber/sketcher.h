#ifndef SKETCHER_INCLUDED
#define SKETCHER_INCLUDED

#include <stdint.h>
#include "renderer.h"

struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;

	void set(uint8_t _r, uint8_t _g, uint8_t _b) {
		r = _r; g = _g; b = _b;
	}
	void set(uint32_t c) {
		r = (c & 0xff0000) >> 16;
		g = (c & 0xff00) >> 8;
		b = c & 0xff;
	}
};

/**
Sin wave.
Input: 0-255 (range will be clipped correctly.
Output:: -256 - 256
*/
int16_t isin(uint16_t x);

struct UIRenderData
{
    uint8_t power   = 0;
    uint8_t volume  = 0;
    uint8_t palette = 0;
    uint8_t color[3];

	uint32_t mVolts = 0;
    const char* fontName = 0;

    UIRenderData() {
        color[0] = color[1] = color[2] = 0;
    }
};

class DotStarUI
{
public:
    void Draw(RGB* uiLedStart, int mode, const UIRenderData* data);
};

class Sketcher
{
public:
    enum {
        WIDTH = 128,
        HEIGHT = 32,
        DATA_WIDTH = 52,
        DIAL_WIDTH = 28,
        INNERX = 38,

        REST_MODE = 0,
        BLADE_ON_MODE,
        PALETTE_MODE,
        VOLUME_MODE,
        NUM_MODES
    };

    Sketcher();
    void Draw(Renderer* d, uint32_t time, int mode, const UIRenderData* data);
    void Push(uint8_t value);

    uint8_t data[DATA_WIDTH];

private:
    textureData GetDial(int value);

    uint8_t line = 0;
    uint8_t pos = 0;
    uint32_t animTime = 0;
};

void calcCrystalColor(uint32_t msec, const uint8_t* base, uint8_t* out);


#endif // SKETCHER_INCLUDED
