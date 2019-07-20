#include "vectorui.h"
#include "vrender.h"
#include "assets.h"
#include "Grinliz_Util.h"
#include "sketcher.h"

void VectorUI::Draw(VRender* ren,
    uint32_t time,
    UIMode mode,
    bool bladeIgnited,
    const UIRenderData* data)
{
    static const int W = 160;
    static const int H = 80;

    ren->Clear();

    static const osbr::RGBA POWER_ON(43, 163, 255, 200);
    static const osbr::RGBA POWER_OFF(43, 163, 255, 40);
    static const osbr::RGBA POWER_TEXT(43, 163, 255, 200);

    static const osbr::RGBA AUDIO_ON(255, 163, 0, 200);
    static const osbr::RGBA AUDIO_OFF(43, 163, 255, 40);

    static const osbr::RGBA PALETTE_ON = POWER_ON;
    static const osbr::RGBA PALETTE_OFF = POWER_OFF;

    {
        int p = data->powerLevel(8);

        // Power
        VRender::Vec2 P[] = {
            {15, -2}, {15, 2}, {H / 2 - 2, 5}, {H / 2 - 2, -5}
        };
        for (int r = 3; r < 11; ++r) {
            ren->SetTransform(FixedNorm(r*2+1, 28), Fixed115(H / 2), Fixed115(H / 2));
            ren->DrawPoly(P, 4, r-3 <= p ? POWER_ON : POWER_OFF);
        }
    }

    {
        // Audio
        VRender::Vec2 P[] = {
            {15, -2}, {15, 2}, {H / 2 - 2, 5}, {H / 2 - 2, -5}
        };
        for (int r = 4, i=0; r >= -3; --r, ++i) {
            ren->SetTransform(FixedNorm(r*2-1, 28), Fixed115(W - H / 2), Fixed115(H / 2));
            ren->DrawPoly(P, 4, i/2 < data->volume ? AUDIO_ON : AUDIO_OFF);
        }
    }
    ren->ClearTransform();
    {
        // Power text
        CStr<5> volts;
        volts.setFromNum(data->mVolts, true);
        VRenderUtil::DrawStr(ren, volts.c_str(), 50, 70, getGlypth_calibri8, POWER_TEXT);
    }

    {
        // Palette
        for (int i = 0; i < 8; ++i) {
            ren->DrawRect(48 + i * 8, 2, 6, 6, i == data->palette ? PALETTE_ON : PALETTE_OFF);
        }
    }

    {
        // Crystal
        static const int CX = 25;
        static const int CY = 13;
        static const int T = 4;

        static const VRender::Vec2 OUTER[6] = {
            {W / 2 - CX, H / 2 - CY},
            {W / 2 - CX - CY, H / 2},
            {W / 2 - CX, H / 2 + CY},
            {W / 2 + CX, H / 2 + CY},
            {W / 2 + CX + CY, H / 2},
            {W / 2 + CX, H / 2 - CY}
        };
        static const VRender::Vec2 ARROW[4] = {
            {W / 2, H / 2 - CY},
            {W / 2 - CY, H / 2},
            {W / 2, H / 2 + CY},
            {W / 2 + CY, H / 2}
        };
        static const VRender::Vec2 ARROW_INNER[4] = {
            {W / 2, H / 2 - CY + T},
            {W / 2 - CY + T, H / 2},
            {W / 2, H / 2 + CY - T},
            {W / 2 + CY - T, H / 2}
        };
        static const VRender::Vec2 INNER[6] = {
            {W / 2 - CX, H / 2 - CY + T},
            {W / 2 - CX - CY + T, H / 2},
            {W / 2 - CX, H / 2 + CY - T},
            {W / 2 + CX, H / 2 + CY - T},
            {W / 2 + CX + CY - T, H / 2},
            {W / 2 + CX, H / 2 - CY + T}
        };
        static const VRender::Vec2 LEFT[4] = {
            {W / 2 - CX, H / 2 - CY + T},
            {W / 2 - CX - CY + T, H / 2},
            {W / 2 - CX, H / 2 + CY - T},
            {W / 2 - CX + CY - T, H / 2},
        };
        osbr::RGBA bg(255, 255, 255, 120);
        for (int i = 0; i < 8; ++i) {
            if (mode == UIMode::MEDITATION) {
                osbr::RGB medRGB;
                calcCrystalColorHSV(time + i * 200, data->color, &medRGB);
                bg.set(medRGB.r, medRGB.g, medRGB.b, 120);
            }
            ren->DrawRect(W/2 - 32 + i*8 + 1, H/2-16, 6, 32, bg, 1);
        }

        osbr::RGBA rgba(data->color.r, data->color.g, data->color.b, 200);
        ren->PushLayer();
        if (mode == UIMode::NORMAL) {
            ren->DrawPoly(OUTER, 6, rgba);
            ren->DrawPoly(INNER, 6, rgba);
        }
        else if (mode == UIMode::PALETTE) {
            ren->SetTransform(-CX, 0);
            ren->DrawPoly(ARROW, 4, rgba);
            ren->DrawPoly(ARROW_INNER, 4, rgba);
        }
        else if (mode == UIMode::VOLUME) {
            ren->SetTransform(CX, 0);
            ren->DrawPoly(ARROW, 4, rgba);
            ren->DrawPoly(ARROW_INNER, 4, rgba);
        }
        ren->ClearTransform();
        ren->PopLayer();
    }

    ren->Render();
}
