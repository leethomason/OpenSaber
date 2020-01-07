#include "bladeflash.h"
#include "Grinliz_Util.h"
#include "rgb2hsv.h"
#include "fixed.h"

void BladeFlash::tick(uint32_t msec)
{
    if (msec - tStart >= LEN) {
        color = bladeColor;
    }
    else {
#if false
        int t = int(msec - tStart) * 1024 / LEN;
#else
        FixedNorm fn(msec - tStart, LEN*4);
        FixedNorm sn = iSin(fn);
        int t = sn.scale(1024);
#endif
        color.r = lerp1024<int>(impactColor.r, bladeColor.r, t);
        color.g = lerp1024<int>(impactColor.g, bladeColor.g, t);
        color.b = lerp1024<int>(impactColor.b, bladeColor.b, t);
    }
}


void BladeFlash::setImpactInverse()
{
    uint8_t h, s, v;
    rgb2hsv(bladeColor.r, bladeColor.g, bladeColor.b, &h, &s, &v);
    h += 90;
    hsv2rgb(h, s, v, &impactColor.r, &impactColor.g, &impactColor.b);
}

