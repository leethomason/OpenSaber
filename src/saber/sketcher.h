#ifndef SKETCHER_INCLUDED
#define SKETCHER_INCLUDED

#include <stdint.h>
#include "renderer.h"
#include <DotStar.h>

/**
Sin wave.
Input: 0-255 (range will be clipped correctly.
Output:: -256 - 256
*/
int16_t isin(uint16_t x);

struct UIRenderData
{
    uint8_t power	= 0;
    uint8_t volume  = 0;
    uint8_t palette = 0;
	uint32_t mVolts = 0;
    const char* fontName = 0;
	RGB color;

    UIRenderData() {}
};

class DotStarUI
{
public:
    void Draw(RGB* uiLedStart, int mode, const UIRenderData& data);

	void SetBrightness(uint16_t v) { m_brightness = v; }
	uint16_t Brightness() const { return m_brightness; }

	static bool Test();

private:
	uint16_t m_brightness = 256;
};

class Sketcher
{
public:
    enum {
        WIDTH = 128,
        HEIGHT = 32,

		TWEAK_X0	= 2,
		TWEAK_X1    = 0,
		X0			= 0 + TWEAK_X0,
		X1			= WIDTH - TWEAK_X1,
		CENTER		= (X0 + X1) / 2,

        DIAL_WIDTH = 28,
		DATA_WIDTH	= WIDTH - DIAL_WIDTH * 2 - 20,
		BAR_WIDTH   = 38 - TWEAK_X0 - TWEAK_X1,

        REST_MODE = 0,
        BLADE_ON_MODE,
        PALETTE_MODE,
        VOLUME_MODE,
        NUM_MODES
    };

    Sketcher();
    void Draw(Renderer* d, uint32_t time, int mode, const UIRenderData* data);
    void Push(uint8_t value);

private:
    textureData GetDial(int value);

    uint8_t line = 0;
    uint8_t pos = 0;
    uint32_t animTime = 0;
	uint8_t data[DATA_WIDTH];
};

void calcCrystalColor(uint32_t msec, int32_t lowVariation, int32_t highVariation, const RGB& base, RGB* out);

bool TestCrystalColor();

#endif // SKETCHER_INCLUDED
