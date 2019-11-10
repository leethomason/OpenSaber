#pragma once

#include <stdint.h>
#include "rgb.h"
#include "fixed.h"

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
    ColorRGB rgb;
};
typedef void (*BlockDraw)(const BlockDrawChunk* chunks, int y, int n);
typedef const uint8_t* (*GlyphMetrics)(int charID, int* advance, int* w, int* rows);

class VRender
{
public:
    // FIXME optimize for edges
    static const int MAX_EDGES = 250;   // defines memory use.
    static const int MAX_ACTIVE = MAX_EDGES;
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
        int x;
        int y;

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

    void Clear();
    void DrawRect(int x0, int y0, int width, int height, const osbr::RGBA& rgba, int outline=0);

    void DrawPoly(const Vec2* points, int n, const osbr::RGBA& rgba);
    void PushLayer() { m_layerFixed = true; m_layer++; }
    void PopLayer() { m_layerFixed = false; }

    void SetTransform(FixedNorm rotation, Fixed115 x, Fixed115 y) {
        m_rot = rotation;
        m_transX = x;
        m_transY = y;
    }

    void SetTransform(int x, int y) { SetTransform(FixedNorm(0), Fixed115(x), Fixed115(y)); }
    void SetScale(Fixed115 sx, Fixed115 sy) { m_scaleX = sx; m_scaleY = sy; }

    void ClearTransform();

    int NumEdges() const {
        return m_nEdge;
    }

private:
    enum {
        LAYER_BACKGROUND = -128,
    };

    struct Edge {
        ColorRGBA color;
        int8_t layer;
        uint8_t yAdd;
        Fixed115 x0, y0;
        Fixed115 x1, y1;
        Edge* nextStart = 0;

        void Clear();
        void Init(int x0, int y0, int x1, int y1, int layer, osbr::RGBA rgba);
        
        void Align() {
            if (y0 > y1) {
                Swap(y0, y1);
                Swap(x0, x1);
            }
        }

        bool Horizontal() const { return y0 == y1; }
    };

    struct ActiveEdge
    {
        int8_t layer;
        ColorRGBA color;
        int16_t yEnd;
        Fixed115 x;
        Fixed115 slope;
    };

    #ifdef VECTOR_MONO
    struct ActiveEdgeProxy { int32_t dummy[2]; };
    #else
    struct ActiveEdgeProxy { int32_t dummy[3]; };
    #endif

    static void SwapAE(ActiveEdge* _a, ActiveEdge* _b) {
        STATIC_ASSERT(sizeof(ActiveEdge) == sizeof(ActiveEdgeProxy));
        ActiveEdgeProxy t = *((ActiveEdgeProxy*)_a);
        *((ActiveEdgeProxy*)_a) = *((ActiveEdgeProxy*)_b);
        *((ActiveEdgeProxy*)_b) = t;
    }

    struct ColorEntry
    {
        int8_t layer;				// actually int8_t in the active edge.
		ColorRGBA color;
        void Set(int layer, ColorRGBA color) {
            this->layer = layer;
            this->color = color;
        }
    };

    void StartEdges();
    void EndEdges();

    void SortToStart();
    void Rasterize();
    void RasterizeLine(int y, const Rect&);
    osbr::RGB AddToColorStack(int layer, ColorRGBA color);

    void IncrementActiveEdges(int y);
    void AddStartingEdges(int y);
    void SortActiveEdges();

	int CalcRootHash(int y) { return ((uint32_t)y) % Y_HASH; }

    static const int MAX_COLOR_STACK = 8;

    bool m_layerFixed = false;
    BlockDraw m_blockDraw = 0;
    Rect m_size;
    Rect m_clip;
    int m_nActive;
    int m_nEdge = 0;
    int m_layer = 0;
    int m_nColor = 0;
    int m_start;
    int m_end;
    FixedNorm m_rot;
    Fixed115 m_transX, m_transY, m_scaleX, m_scaleY;

    ColorEntry m_colorStack[MAX_COLOR_STACK];
    ActiveEdge m_activeEdges[MAX_ACTIVE];
    Edge m_edge[MAX_EDGES];
    Edge* m_rootHash[Y_HASH];
};


class VRenderUtil
{
public:
    static void DrawStr(VRender* vrender, const char* str, int x, int y, GlyphMetrics metrics, const osbr::RGBA&);
};