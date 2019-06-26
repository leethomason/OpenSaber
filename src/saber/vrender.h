#pragma once

#include <stdint.h>
#include "rgb.h"

typedef void (*BlockDraw)(int x0, int y0, int x1, int y1, const osbr::RGBA& rgba);

class VRender
{
public:
    template<class T> 
    static T Min(T a, T b) { return a < b ? a : b; }
    template<class T> 
    static T Max(T a, T b) { return a > b ? a : b; }

    struct Vec2
    {
        int x = 0;
        int y = 0;
    };

    struct Rect
    {
        Rect() {}
        Rect(int x0, int y0, int x1, int y1) {
            this->x0 = x0;
            this->y0 = y0;
            this->x1 = x1;
            this->y1 = y1;
        }

        int x0 = 0;
        int y0 = 0;
        int x1 = 0;
        int y1 = 0;

        int CX() const { return x1 - x0; }
        int CY() const { return y1 - y0; }
        int Area() const { return CX() * CY(); }
        bool Empty() const { return x1 <= x0 || y1 <= y0; }

        Rect Intersect(const Rect& rhs) const {
            Rect r = *this;
            r.x0 = Max(this->x0, rhs.x0);
            r.y0 = Max(this->y0, rhs.y0);
            r.x1 = Min(this->x1, rhs.x1);
            r.y1 = Min(this->y1, rhs.y1);
            return r;
        }
    };


    void Attach(BlockDraw blockDraw) { m_blockDraw = blockDraw; }
    void SetSize(int w, int h) { m_size = Rect(0, 0, w, h); }
    void SetClip(const Rect& clip) { m_clip = clip; }
    void ClearClip() { m_clip = m_size; }

    // Respects clip
    void Clear(const osbr::RGBA rgba);
    void DrawRect(int x0, int y0, int width, int height, const osbr::RGBA& rgba);

    // x0, x1 inclusive
    void DrawLine(int x0, int y0, int x1, int y1, const osbr::RGBA& rgba);
    void DrawPoly(const Vec2* points, int n, const osbr::RGBA& rgba);

private:
    struct Segment {
        Vec2 p0;
        Vec2 p1;
    };

    BlockDraw m_blockDraw = 0;
    Rect m_size;
    Rect m_clip;
};
