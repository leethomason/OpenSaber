#pragma once

#include <stdint.h>
#include "rgb.h"

struct Vec2
{
    int x = 0;
    int y = 0;
};

struct Rect
{
    int x0 = 0;
    int y0 = 0;
    int x1 = 0;
    int y1 = 0;

    int CX() const { return x1 - x0; }
    int CY() const { return y1 - y0; }
    int Area() const { return CX() * CY(); }
};

class VRender
{
public:
    enum {
        MODE_RGB_32,
        MODE_RGB_16
    };

    void Attach(void* buffer,
                int mode,
                const Rect& size);
    void SetClip(const Rect& clip);     // And sub-clip to this

    void Clear(const osbr::RGBA rgba);

    // x0, x1 inclusive
    void DrawLine(int x0, int y0, int x1, int y1, const osbr::RGBA& rgba);
    void DrawPoly(const Vec2* points, int n, const osbr::RGBA& rgba);

private:
    // primitive; generally meant for slabs, but get rect efficiency when we can.
    void DrawRect(int x0, int y0, int x1, int y);

    Rect m_bufferSize;
    uint16_t* m_buffer = 0;
    int m_mode;
    Rect m_clip;
};
