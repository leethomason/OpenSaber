#pragma once

#include "rgb.h"

class BladeFlash
{
public:
    BladeFlash() {}

    void setBladeColor(const osbr::RGB& rgb) { bladeColor = rgb;  }
    void setImpactColor(const osbr::RGB& rgb) { impactColor = rgb; }
    void setImpactInverse();

    void tick(uint32_t msec);
    void doFlash(uint32_t msec) { tStart = msec; }

    osbr::RGB getColor() const { return color;  }

private:
    static const uint32_t LEN = 500;
    uint32_t tStart = 0;
    osbr::RGB bladeColor;
    osbr::RGB impactColor;
    osbr::RGB color;
};