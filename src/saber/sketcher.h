#ifndef SKETCHER_INCLUDED
#define SKETCHER_INCLUDED

#include <stdint.h>
#include <DotStar.h>
#include "renderer.h"

struct UIRenderData
{
    uint8_t power   = 0;
    uint8_t volume  = 0;
    uint8_t palette = 0;
    uint8_t color[3];
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

#endif // SKETCHER_INCLUDED
