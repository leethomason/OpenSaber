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
        const UIRenderData* data);

private:
    static void Segment(VRender* ren, int width, int s, int num, osbr::RGBA rgba);
    // 1000-100-10-1
    static void NumToDigit(int num, int* digits);

};
