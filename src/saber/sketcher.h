#ifndef SKETCHER_INCLUDED
#define SKETCHER_INCLUDED

#include <stdint.h>
#include "renderer.h"

#include <DotStar.h>
#include <Grinliz_Arduino_Util.h>

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
    void Draw(DotStar::RGB* uiLedStart, int mode, const UIRenderData* data);
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

    uint8_t data[DATA_WIDTH];

private:
    textureData GetDial(int value);

    uint8_t line = 0;
    uint8_t pos = 0;
    uint32_t animTime = 0;
};

void calcCrystalColor(uint32_t msec, const uint8_t* base, uint8_t* out);


#endif // SKETCHER_INCLUDED
