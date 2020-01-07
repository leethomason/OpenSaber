#pragma once

#include <stdint.h>
#include "modes.h"
#include "uirenderdata.h"

class VRender;

class VectorUI
{
public:
    static void Draw(VRender* r, 
        uint32_t time, 
        UIMode mode, 
        bool bladeIgnited, 
        const UIRenderData* data,
        uint8_t* buffer=0);      // buffer is used to immediate draw (without renderer)

private:
    static void Segment(VRender* ren, int width, int s, int num, osbr::RGBA rgba);
    // 1000-100-10-1
    static void DrawBar(VRender* ren, int x, int y, int width, const osbr::RGBA& color, int fraction);
};
