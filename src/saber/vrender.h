#pragma once

#include <stdint.h>
#include "rgb.h"
#include "fixed.h"

typedef void (*BlockDraw)(int x0, int y0, int x1, int y1, const osbr::RGB& rgb);

class VRender
{
public:
    static const int MAX_EDGES = 200;   // defines memory use.
    static const int MAX_ACTIVE = 16;
    static const int Y_HASH = 32;

    template<class T> 
    static T Min(T a, T b) { return a < b ? a : b; }
    template<class T> 
    static T Max(T a, T b) { return a > b ? a : b; }
    template<class T>
    static void Swap(T& a, T& b) {
        T temp = a;
        a = b;
        b = temp;
    }

    struct Vec2
    {
        int x = 0;
        int y = 0;

        const bool operator== (const Vec2& rhs) const {
            return rhs.x == x && rhs.y == y;
        }

        const bool operator!= (const Vec2& rhs) const {
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

    VRender();
    void Attach(BlockDraw blockDraw) { m_blockDraw = blockDraw; }
    void SetSize(int w, int h) { m_size = Rect(0, 0, w, h); }
    void SetClip(const Rect& clip) { m_clip = clip; }
    void ClearClip() { m_clip = m_size; }

    void Render();

    // Respects clip
    void Clear(const osbr::RGB rgb);
    void DrawRect(int x0, int y0, int width, int height, const osbr::RGBA& rgba);
    void DrawPoly(const Vec2* points, int n, const osbr::RGBA& rgba);

    void SetTransform(FixedNorm rotation, int x, int y) {
        m_rot = rotation;
        m_trans.x = x;
        m_trans.y = y;
    }

    void ClearTransform()
    {
        m_rot = 0;
        m_trans.x = m_trans.y = 0;
    }

    struct Edge {
        enum {
            LAYER_BACKGROUND = -1
        };
        uint8_t layer;
        uint8_t yAdd;
        Fixed115 x0, y0;
        Fixed115 x1, y1;
        Fixed115 x;
        Fixed115 slope;
        osbr::RGBA color;
        Edge* nextStart = 0;
        Edge* nextActive = 0;

        void Init(int x0, int y0, int x1, int y1, int layer, const osbr::RGBA& rgba) {
            this->x0 = x0;
            this->y0 = y0;
            this->x1 = x1;
            this->y1 = y1;
            this->layer = layer;
            this->color = rgba;
            this->nextStart = 0;
        }
        
        void Align() {
            if (y0 > y1) {
                Swap(y0, y1);
                Swap(x0, x1);
                slope = -slope;
            }
        }

        bool Horizontal() const { return y0 == y1; }
    };
    private:

    struct ColorEntry
    {
        int layer;
        osbr::RGBA color;
        void Set(int layer, osbr::RGBA color) {
            this->layer = layer;
            this->color = color;
        }
    };

    void StartEdges();
    void EndEdges();

    void SortToStart();
    void Rasterize();
    void RasterizeLine(int y, const Rect&);
    osbr::RGB AddToColorStack(int layer, const osbr::RGBA&);

    void IncrementActiveEdges(int y);
    void AddStartingEdges(int y);
    void SortActiveEdges();

    static const int MAX_COLOR_STACK = 8;
    static const int MAX_MATRIX_STACK = 4;

    BlockDraw m_blockDraw = 0;
    Rect m_size;
    Rect m_clip;
    int m_nEdge = 0;
    int m_layer = 0;
    int m_nColor = 0;
    bool m_matrixDirty = true;
    int m_start;
    int m_end;
    Edge* m_activeRoot = 0;
    FixedNorm m_rot;
    Vec2 m_trans;

    ColorEntry m_colorStack[MAX_COLOR_STACK];
    Edge m_edge[MAX_EDGES];
    Edge* m_rootHash[Y_HASH];
};
