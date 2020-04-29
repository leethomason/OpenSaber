/*
  Copyright (c) Lee Thomason, Grinning Lizard Software

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
  of the Software, and to permit persons to whom the Software is furnished to do
  so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#pragma once

#include <stdint.h>
#include "rgb.h"
#include "fixed.h"
#include "Grinliz_Util.h"

struct BlockDrawChunk {
    int x0;
    int x1;
    int y;
    int color;
};
typedef void (*BlockDraw)(const BlockDrawChunk* chunks, int n);
typedef const uint8_t* (*GlyphMetrics)(int charID, int* advance, int* w, int* rows);

class VRender
{
public:
    static const int MAX_EDGES = 32;    
    static const int MAX_ACTIVE = 32;
    // Currently this must be the height or greater, so it isn't really a hash.
    static const int Y_HASH = 32;

    struct Vec2
    {
        int x;
        int y;

        const bool operator== (const Vec2& rhs) const {
            return rhs.x == x && rhs.y == y;
        }

        const bool operator!= (const Vec2& rhs) const {
            return rhs.x != x || rhs.y != y;
        }
    };

    struct Vec2I8
    {
        int8_t x;
        int8_t y;

        const bool operator== (const Vec2I8& rhs) const {
            return rhs.x == x && rhs.y == y;
        }

        const bool operator!= (const Vec2I8& rhs) const {
            return rhs.x != x || rhs.y != y;
        }
    };


    struct Rect
    {
        Rect() {}
        Rect(int x0, int y0, int x1, int y1) {
            this->x0 = x0;
            this->y0 = y0;
            this->x1 = x1;
            this->y1 = y1;

            if (this->x0 > this->x1) glSwap(this->x0, this->x1);
            if (this->y0 > this->y1) glSwap(this->y0, this->y1);
        }

        int x0 = 0;
        int y0 = 0;
        int x1 = 0;
        int y1 = 0;

        int CX() const { return x1 - x0; }
        int CY() const { return y1 - y0; }
        int Area() const { return CX() * CY(); }
        bool Empty() const { return x1 <= x0 || y1 <= y0; }

        void Intersect(const Rect& rhs) {
            this->x0 = glMax(this->x0, rhs.x0);
            this->y0 = glMax(this->y0, rhs.y0);
            this->x1 = glMin(this->x1, rhs.x1);
            this->y1 = glMin(this->y1, rhs.y1);
        }

        static Rect Intersect(const Rect& a, const Rect& b) {
            Rect r;
            r.x0 = glMax(a.x0, b.x0);
            r.x1 = glMin(a.x1, b.x1);
            r.y0 = glMax(a.y0, b.y0);
            r.y1 = glMin(a.y1, b.y1);
            return r;
        }
    };

    VRender();
    void Attach(BlockDraw blockDraw) { m_blockDraw = blockDraw; }
    void SetSize(int w, int h) { m_size = Rect(0, 0, w, h); }
    
    void SetClip(const Rect& clip) { m_clip = clip; }
    void ClearClip() { m_clip = m_size; }

    void Clear();
    void DrawRect(int x0, int y0, int width, int height, int color, int outline=0);
    void DrawPoly(const Vec2* points, int n, int rgba);

    void SetTransform(FixedNorm rotation, Fixed115 x, Fixed115 y) {
        m_rot = rotation;
        m_transX = x;
        m_transY = y;
    }

    void SetTransform(int x, int y) { SetTransform(FixedNorm(0), Fixed115(x), Fixed115(y)); }
    void SetScale(Fixed115 sx, Fixed115 sy) { m_scaleX = sx; m_scaleY = sy; }
    void SetCamera(int tx, int ty, int sx, int sy) {
        ASSERT(sx != 0 && sy != 0);
        camTrans.x = tx;
        camTrans.y = ty;
        camScale.x = sx;
        camScale.y = sy;
    }

    void ClearTransform();
private:
    struct ActiveEdge
    {
        int16_t color;
        int16_t yEnd;
        int32_t x16;        // x in 16.16
        int32_t slope16;    // step in slope; 64k is one pixel
        ActiveEdge* next;
    };

    void Rasterize();
    void RasterizeLine(int y, const Rect&);

    void IncrementActiveEdges(int y);
    void AddStartingEdges(int y);
    void SortActiveEdges();

    void CreateActiveEdge(int x0, int y0, int x1, int y1, int c);
    void InnerCreateActiveEdge(Fixed115 x0, Fixed115 y0, Fixed115 x1, Fixed115 y1, int c);

    void Transform4(Fixed115* e, int x0, int y0, int x1, int y1);
    Rect TransformCam(const Rect& in) {
        Rect r(in.x0 * camScale.x + camTrans.x,
            in.y0 * camScale.y + camTrans.y,
            in.x1 * camScale.x + camTrans.x,
            in.y1 * camScale.y + camTrans.y);
        return r;
    }

    int m_nActive;
    int m_nPool = 0;
    FixedNorm m_rot;
    Fixed115 m_transX, m_transY, m_scaleX, m_scaleY;
    BlockDraw m_blockDraw = 0;
    Vec2 camScale = { 1, 1 };
    Vec2 camTrans = { 0, 0 };
    Rect m_size;
    Rect m_clip;

    ActiveEdge* m_activeEdges[MAX_ACTIVE];
    ActiveEdge  m_edgePool[MAX_EDGES];
    ActiveEdge* m_rootHash[Y_HASH];
};
