#pragma once

#include <stdint.h>
#include "rgb.h"
#include "fixed.h"
#include "Grinliz_Util.h"

#define VECTOR_MONO

#ifdef VECTOR_MONO
typedef uint8_t ColorRGB;
typedef uint8_t ColorRGBA;
#else
typedef osbr::RGB ColorRGB;
typedef osbr::RGBA ColorRGBA;
#endif

struct BlockDrawChunk {
    int x0;
    int x1;
    int y;
    ColorRGB rgb;
};
typedef void (*BlockDraw)(const BlockDrawChunk* chunks, int n);
typedef const uint8_t* (*GlyphMetrics)(int charID, int* advance, int* w, int* rows);

class VRender
{
public:
    // Defines memory use; should probably be passed in. The test UI uses 20-24 edges
    // with immediate mode on, 120 with it off. 
    static const int MAX_EDGES = 120;    
    static const int MAX_ACTIVE = MAX_EDGES / 2;
    // Currently this must be the height or greater, so it isn't really a hash.
    static const int Y_HASH = 32;
    static const int MAX_COLOR_STACK = 8;

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

    void Render();

    void Clear();
    void DrawRect(int x0, int y0, int width, int height, const osbr::RGBA& rgba, int outline=0);

    void DrawPoly(const Vec2* points, int n, const osbr::RGBA& rgba);
    void DrawPoly(const Vec2I8* points, int n, const osbr::RGBA& rgba);
    void PushLayer() { m_layerFixed = true; m_layer++; }
    void PopLayer() { m_layerFixed = false; }
    void SetImmediate(bool val) { m_immediate = val; }

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

    int NumEdges() const {
        return m_nPool;
    }

private:
    enum {
        LAYER_BACKGROUND = -128,
    };

    struct ActiveEdge
    {
        ColorRGBA color;
        int8_t layer;
        int16_t yEnd;
        Fixed115 x;
        Fixed115 slope;
        ActiveEdge* next;
    };

    struct ColorEntry
    {
        int8_t layer;				// actually int8_t in the active edge.
		ColorRGBA color;
        void Set(int layer, ColorRGBA color) {
            this->layer = layer;
            this->color = color;
        }
    };

    void Rasterize();
    void RasterizeLine(int y, const Rect&);
    ColorRGB AddToColorStack(int layer, ColorRGBA color);

    void IncrementActiveEdges(int y);
    void AddStartingEdges(int y);
    void SortActiveEdges();

    void CreateActiveEdge(int x0, int y0, int x1, int y1, ColorRGBA c);
    void InnerCreateActiveEdge(Fixed115 x0, Fixed115 y0, Fixed115 x1, Fixed115 y1, ColorRGBA c);

    void Transform4(Fixed115* e, int x0, int y0, int x1, int y1);
    Rect TransformCam(const Rect& in) {
        Rect r(in.x0 * camScale.x + camTrans.x,
            in.y0 * camScale.y + camTrans.y,
            in.x1 * camScale.x + camTrans.x,
            in.y1 * camScale.y + camTrans.y);
        return r;
    }

    bool m_layerFixed = false;
    BlockDraw m_blockDraw = 0;
    Rect m_size;
    Rect m_clip;
    int m_nActive;
    int m_layer = 0;
    int m_nColor = 0;
    int m_nPool = 0;
    bool m_immediate = false;
    FixedNorm m_rot;
    Fixed115 m_transX, m_transY, m_scaleX, m_scaleY;
    Vec2 camScale = { 1, 1 };
    Vec2 camTrans = { 0, 0 };

    ColorEntry  m_colorStack[MAX_COLOR_STACK];
    ActiveEdge* m_activeEdges[MAX_ACTIVE];
    ActiveEdge  m_edgePool[MAX_EDGES];
    ActiveEdge* m_rootHash[Y_HASH];
};
