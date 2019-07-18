#include "vectorui.h"
#include "vrender.h"
#include "assets.h"

void VectorUI::Draw(VRender* ren,
    uint32_t time,
    UIMode mode,
    bool bladeIgnited,
    const UIRenderData* data)
{
    static const int W = 160;
    static const int H = 80;

    ren->Clear();

    {
        // Power
        VRender::Vec2 P[] = {
            {15, -2}, {15, 2}, {H / 2 - 2, 5}, {H / 2 - 2, -5}
        };
        for (int r = 3; r < 11; ++r) {
            ren->SetTransform(FixedNorm(r*2+1, 28), Fixed115(H / 2), Fixed115(H / 2));
            ren->DrawPoly(P, 4, osbr::RGBA(255, 255, 255, 255));
        }
    }

    {
        // Audio
        VRender::Vec2 P[] = {
            {15, -2}, {15, 2}, {H / 2 - 2, 5}, {H / 2 - 2, -5}
        };
        for (int r = -3; r < 5; ++r) {
            ren->SetTransform(FixedNorm(r*2-1, 28), Fixed115(W - H / 2), Fixed115(H / 2));
            ren->DrawPoly(P, 4, osbr::RGBA(255, 255, 255, 255));
        }
    }
    ren->ClearTransform();
    VRenderUtil::DrawStr(ren, "Test 3.128", 38, 20, getGlypth_calibri8, osbr::RGBA(255, 0, 255, 200));
    ren->Render();
}
