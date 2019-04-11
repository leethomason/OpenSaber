#pragma once

#include <stdint.h>
#include "rgb.h"
/*
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

// Renders to a 16bit RGB buffer.
class VRender16
{
public:
    void Set(int width, int height);
    void Attach(uint16_t* buffer,       // Memory for the RGB data
        const Rect& bufferRect,         // Defines the bounds of the buffer 
        const Rect& clip);              // And sub-clip to this

    void Clear(const osbr::RGBA rgba);
    void DrawLine(int x0, int y0, int x1, int y1, const osbr::RGBA& rgba);
    //void DrawRectangle(const Rect& rectangle, const osbr::RGBA *fill, const osbr::RGBA* border);

private:
    uint16_t ToU16(const osbr::RGBA& c) {
        return ((c.r >> 3) << 11) | ((c.g >> 2) << 5) | (c.b >> 3); // fixme
    }

    int m_width = 0, m_height = 0;
    uint16_t* m_buffer = 0;
    Rect m_bufferRect;
    Rect m_clip;
};

*/
