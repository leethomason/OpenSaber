#include "uiPixel75.h"

#include "./assets.h"

/*
    .x.X.x.
    x..X..x
    Xx.X.xX
    XX.X.XX
    .XXXXX.

    cols: 01110 0x0c
          11101 0x19
          10000 0x10
          11111 0x1f
*/
void Pixel75UI::Process(UIMode mode, 
                        const UIRenderData& data)
{
    for (int i = 0; i < ALLOCATED; ++i)
        m_col[i] = 0;

    switch (mode)
    {
    case UIMode::NORMAL:
        getGlypth_tomThumb5('0' + data.palette, m_col + 0);
        DrawBar(5, UIRenderData::powerLevel(data.mVolts, 4));
        DrawBar(6, data.volume);
        break;

    case UIMode::PALETTE:
        getGlypth_tomThumb5('0' + data.palette, m_col + 4);
        DrawBar(0, data.color.r / 50);
        DrawBar(1, data.color.g / 50);
        DrawBar(2, data.color.b / 50);
        break;

    case UIMode::VOLUME:
        getGlypth_tomThumb5('V', m_col);
        DrawBar(4, data.volume);
        DrawBar(5, data.volume);
        DrawBar(6, data.volume);
        break;

    case UIMode::LOCK:
        getGlypth_tomThumb5((int)(data.locked ? "U" : "L"), m_col);
        break;

    case UIMode::MEDITATION:
        m_col[0] = m_col[6] = 0x0e;
        m_col[1] = m_col[5] = 0x19;
        m_col[2] = m_col[4] = 0x10;
        m_col[3] = 0x1f;
        break;

    default:
        break;
    }
}

void Pixel75UI::DrawBar(int x, int y)
{
    uint8_t *c = m_col + x;
    *c = 0;
    for (int i = 0; i < y; ++i)
    {
        *c |= (16 >> i);
    }
}

void Pixel75UI::DrawDot(int x, int y)
{
    m_col[x] |= (16 >> y);
}