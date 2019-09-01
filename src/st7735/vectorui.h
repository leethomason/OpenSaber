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

};
