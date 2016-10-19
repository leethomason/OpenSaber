#ifndef SKETCHER_INCLUDED
#define SKETCHER_INCLUDED

#include <stdint.h>
#include "renderer.h"

class Sketcher
{
public:
	enum {
		WIDTH		= 128,
		HEIGHT		= 32,

		TWEAK_X0	= 2,
		TWEAK_X1    = 0,
		X0			= 0 + TWEAK_X0,
		X1			= WIDTH - TWEAK_X1,
		CENTER		= (X0 + X1) / 2,

		DIAL_WIDTH	= 28,
		DATA_WIDTH	= WIDTH - DIAL_WIDTH * 2 - 20,
		BAR_WIDTH   = 38 - TWEAK_X0 - TWEAK_X1,

		REST_MODE = 0,
		BLADE_ON_MODE,
		PALETTE_MODE,
		VOLUME_MODE,
		NUM_MODES
	};

	Sketcher();
	void Draw(Renderer* d, uint32_t time, int mode);
	void Push(uint8_t value);

	uint8_t volume = 0;
    uint8_t palette = 0;
	uint8_t color[3];
	uint8_t data[DATA_WIDTH];
	uint8_t power;
	uint32_t mVolts = 0;
	const char* fontName = 0;

private:
	textureData GetDial(int value);

	uint8_t line = 0;
	uint8_t pos = 0;
	uint32_t animTime = 0;
};

#endif // SKETCHER_INCLUDED
