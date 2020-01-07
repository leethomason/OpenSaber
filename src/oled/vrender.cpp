#include "vrender.h"
#include "fixed.h"
#include "Grinliz_Util.h"
#include <stdio.h>

#ifndef _MSC_VER
#include "Grinliz_Arduino_Util.h" // profiling
#endif

VRender::VRender()
{
    ClearTransform();
    Clear();
}

void VRender::Clear() 
{
    m_nActive = 0;
    m_layer = 0;
    m_nColor = 0;
    m_nPool = 0;
    ASSERT(Y_HASH >= m_size.CY());
    memset(m_rootHash, 0, sizeof(ActiveEdge*)*Y_HASH);
}

void VRender::ClearTransform()
{
    m_rot = 0;
    m_transX = 0;
    m_transY = 0;
    m_scaleX = 1;
    m_scaleY = 1;
}


void VRender::DrawRect(int x0, int y0, int w, int h, const osbr::RGBA& rgba, int outline)
{
    Rect in(x0, y0, w + x0, h + y0);
    ASSERT(in.y0 <= in.y1);
    ASSERT(in.x0 <= in.x1);
    ASSERT(m_layer < 127);

    if (m_immediate && m_rot == 0 && outline == 0) {
        Fixed115 xf = x0 * m_scaleX + m_transX;
        Fixed115 yf = y0 * m_scaleY + m_transY;
        Fixed115 wf = w * m_scaleX;
        Fixed115 hf = h * m_scaleY;

        Rect r(xf.getInt(), yf.getInt(), (xf + wf).getInt(), (yf + hf).getInt());
        r = TransformCam(r);

        r.Intersect(m_clip);
        if (!r.Empty()) {
            for (int y = r.y0; y < r.y1; ++y) {
                BlockDrawChunk chunk;
#ifdef VECTOR_MONO
                chunk.rgb = rgba.rgb().get() ? 1 : 0;
#else
                chunk.rgb = rgba.rgb();
#endif
                chunk.x0 = r.x0;
                chunk.x1 = r.x1;
                chunk.y = y;
                m_blockDraw(&chunk, 1);
            }
        }
        return;
    }

    if (!m_layerFixed)
        m_layer++;

#ifdef VECTOR_MONO
    ColorRGBA c = rgba.rgb().get() ? 1 : 0;
#else
    ColorRGBA c = rgba;
#endif

    CreateActiveEdge(in.x0, in.y0, in.x1, in.y0, c);
    CreateActiveEdge(in.x1, in.y0, in.x1, in.y1, c);
    CreateActiveEdge(in.x1, in.y1, in.x0, in.y1, c);
    CreateActiveEdge(in.x0, in.y1, in.x0, in.y0, c);

    if (outline) {
        CreateActiveEdge(in.x0 + outline, in.y0 + outline, in.x1 - outline, in.y0 + outline, c);
        CreateActiveEdge(in.x1 - outline, in.y0 + outline, in.x1 - outline, in.y1 - outline, c);
        CreateActiveEdge(in.x1 - outline, in.y1 - outline, in.x0 + outline, in.y1 - outline, c);
        CreateActiveEdge(in.x0 + outline, in.y1 - outline, in.x0 + outline, in.y0 + outline, c);
    }
}

void VRender::DrawPoly(const Vec2* points, int n, const osbr::RGBA& rgba)
{
#ifdef VECTOR_MONO
    ColorRGBA c = rgba.rgb().get() ? 1 : 0;
#else
    ColorRGBA c = rgba;
#endif

    if (!m_layerFixed)
        m_layer++;

    for (int i = 1; i < n; ++i) {
        CreateActiveEdge(points[i - 1].x, points[i - 1].y, points[i].x, points[i].y, c);
    }
    if (points[n - 1] != points[0]) {
        CreateActiveEdge(points[n - 1].x, points[n - 1].y, points[0].x, points[0].y, c);
    }
}


void VRender::DrawPoly(const Vec2I8* points, int n, const osbr::RGBA& rgba)
{
#ifdef VECTOR_MONO
    ColorRGBA c = rgba.rgb().get() ? 1 : 0;
#else
    ColorRGBA c = rgba;
#endif

    if (!m_layerFixed)
        m_layer++;

    for (int i = 1; i < n; ++i) {
        CreateActiveEdge(points[i - 1].x, points[i - 1].y, points[i].x, points[i].y, c);
    }
    if (points[n - 1] != points[0]) {
        CreateActiveEdge(points[n - 1].x, points[n - 1].y, points[0].x, points[0].y, c);
    }
}


void VRender::Transform4(Fixed115* e, int x0, int y0, int x1, int y1)
{
    Fixed115 camTX = camTrans.x;
    Fixed115 camTY = camTrans.y;
    Fixed115 camSX = camScale.x;
    Fixed115 camSY = camScale.y;

    if (m_rot != 0) {
        FixedNorm s = iSin(m_rot);
        FixedNorm c = iCos(m_rot);

        Fixed115 xp0 = x0 * m_scaleX;
        Fixed115 yp0 = y0 * m_scaleY;
        Fixed115 xp1 = x1 * m_scaleX;
        Fixed115 yp1 = y1 * m_scaleY;

        e[0] = (xp0 * c - yp0 * s + m_transX) * camSX + camTX;
        e[1] = (xp0 * s + yp0 * c + m_transY) * camSY + camTY;
        e[2] = (xp1 * c - yp1 * s + m_transX) * camSX + camTX;
        e[3] = (xp1 * s + yp1 * c + m_transY) * camSY + camTY;
    }
    else if (m_scaleX != 1 || m_scaleY != 1) {
        e[0] = (x0 * m_scaleX + m_transX) * camSX + camTX;
        e[1] = (y0 * m_scaleY + m_transY) * camSY + camTY;
        e[2] = (x1 * m_scaleX + m_transX) * camSX + camTX;
        e[3] = (y1 * m_scaleY + m_transY) * camSY + camTY;
    }
    else {
        e[0] = (x0 + m_transX) * camSX + camTX;
        e[1] = (y0 + m_transY) * camSY + camTY;
        e[2] = (x1 + m_transX) * camSX + camTX;
        e[3] = (y1 + m_transY) * camSY + camTY;
    }
}

void VRender::CreateActiveEdge(int x0, int y0, int x1, int y1, ColorRGBA c)
{
    Fixed115 p[4];

    Transform4(p, x0, y0, x1, y1);

    if (p[3] < p[1]) {
        glSwap(p[0], p[2]);
        glSwap(p[1], p[3]);
    }

    InnerCreateActiveEdge(p[0], p[1], p[2], p[3], c);
}


void VRender::InnerCreateActiveEdge(Fixed115 x0, Fixed115 y0, Fixed115 x1, Fixed115 y1, ColorRGBA c)
{
    ASSERT(m_nPool < MAX_EDGES);
    ActiveEdge* ae = &m_edgePool[m_nPool];
    ae->color = c;
    ae->layer = m_layer;

    // If horizontal, doesn't render.
    if (y0.getInt() == y1.getInt())
        return;
    int yAdd = y0.getInt();
    if (yAdd >= Y_HASH)
        return;

    m_nPool++;
    ae->yEnd = y1.getInt();
    // For small values, this can go wrong. Need some thinging about
    // almost flat lines. Use pixel coordinates? Not sure. Causes strange
    // long line artifacts.
    ae->slope = (x1 - x0) / (y1 - y0);

    ae->x = x0;
    if (y0 > yAdd) {
        ae->x += ae->slope * (y0 - yAdd);
    }
    ASSERT(Y_HASH >= m_size.CY());
    if (yAdd < 0) 
        yAdd = 0;
    ae->next = m_rootHash[yAdd];
    m_rootHash[yAdd] = ae;
}


void VRender::Render()
{
    ClearTransform();
    m_immediate = false;

    Rect clip = Rect::Intersect(m_size, m_clip);
    if (clip.Empty())
        return;
    int savedLayer = m_layer;
    m_layer = LAYER_BACKGROUND;

    InnerCreateActiveEdge(Fixed115(m_size.x0), Fixed115(m_size.y0), Fixed115(m_size.x0), Fixed115(m_size.y1), 0);
    InnerCreateActiveEdge(Fixed115(m_size.x1), Fixed115(m_size.y0), Fixed115(m_size.x1), Fixed115(m_size.y1), 0);

    m_layer = savedLayer;

    Rasterize();
}


void VRender::IncrementActiveEdges(int y)
{
    int n = m_nActive;
    ActiveEdge** ae = m_activeEdges;
    for (int i = 0; i < n; ++i) {
        // Set the new edge x value, cull the no longer active.
        // Will sort int the next stage.
        if (y >= m_activeEdges[i]->yEnd) {
            // Do nothing; will get culled because ae isn't incremented.
        }
        else {
            *ae = m_activeEdges[i];
            (*ae)->x += (*ae)->slope;
            ++ae;
        }
    }
    m_nActive = ae - m_activeEdges;
}

void VRender::AddStartingEdges(int y)
{
    ASSERT(Y_HASH >= m_size.CY());
    for (ActiveEdge* e = m_rootHash[y]; e; e = e->next) {
        if (e->yEnd > y) {
            // Things that are added before 0 have yAdd of 0.
            // For everything else it is a hash.
            m_activeEdges[m_nActive++] = e;
        }
    }
}


void VRender::SortActiveEdges()
{
    for (int i = 0; i < m_nActive; ++i) {
        int j = i;
        while (j > 0 && m_activeEdges[j - 1]->x > m_activeEdges[j]->x) {
            glSwap(m_activeEdges[j - 1], m_activeEdges[j]);
            j--;
        }
    }
#ifdef _DEBUG
    for (int i = 1; i < m_nActive; ++i) {
        ASSERT(m_activeEdges[i]->x >= m_activeEdges[i - 1]->x);
    }
#endif
}


ColorRGB VRender::AddToColorStack(int layer, ColorRGBA color)
{
    for (int i = m_nColor; i >= 0; --i) {
        // Even-Odd rule. The layer toggles itself on and off.
        if (i > 0 && m_colorStack[i - 1].layer == layer) {
            for (int j = i; j < m_nColor; ++j) {
                m_colorStack[j - 1] = m_colorStack[j];
            }
            --m_nColor;
            break;
        }
        else if (i == 0 || m_colorStack[i - 1].layer < layer) {
            ASSERT(m_nColor < MAX_COLOR_STACK);
            // Scoot up higher entries.
            for (int j = m_nColor; j > i; --j) {
                m_colorStack[j] = m_colorStack[j - 1];
            }
            m_colorStack[i].Set(layer, color);
            ++m_nColor;
            break;
        }
    }
#ifdef _DEBUG
    for (int i = 1; i < m_nColor; ++i) {
        ASSERT(m_colorStack[i-1].layer < m_colorStack[i].layer);
    }
#endif // _DEBUG

    #ifdef VECTOR_MONO
    if (m_nColor)
        return m_colorStack[m_nColor - 1].color;
    return 0;
    #else
    int start = m_nColor - 1;
    for (; start > 0; start--) {
        if (m_colorStack[start].color == 255)
            break;
    }

    osbr::RGB rgb = m_colorStack[start].color.rgb();
    for (int i = start+1; i < m_nColor; ++i) {
        // Everything left is translucent.
        // ARM M0+ doesn't have division.
        osbr::RGBA c = m_colorStack[i].color;
        rgb.r = (c.r * c.a + rgb.r * (255 - c.a)) >> 8;
        rgb.g = (c.g * c.a + rgb.g * (255 - c.a)) >> 8;
        rgb.b = (c.b * c.a + rgb.b * (255 - c.a)) >> 8;
    }
    return rgb;
    #endif
}

void VRender::RasterizeLine(int y, const Rect& clip)
{
    ASSERT(m_nColor == 0);
    static const int CACHE = 8;
    BlockDrawChunk cache[CACHE];
    int nCache = 0;

    if (y < clip.y0 || y >= clip.y1)
        return;

    ASSERT(m_nActive >= 2);
    ASSERT(m_nActive % 2 == 0);

    // Edges are sorted. Walk right to left.
    int x0 = m_activeEdges[0]->x.getInt();
    ColorRGB rgb(0);
    int clipX0 = clip.x0;
    int clipX1 = clip.x1;

    // Edges can go away under the current active, or abut.
    // Intentionally trying to keep this loop simple without look-ahead, etc.
    for(int i=0; i<m_nActive; ++i) {
        ActiveEdge* ae = m_activeEdges[i];
        // FIXME
        // If the color stack doesn't change, we don't need to draw. But be
        // wary of the boundary condition where it's a black bacground,
        // which is why the empty is detected, else it will never draw.
        int x1 = ae->x.getInt();

        // Rasterize previous chunk.
        if (x1 > x0) {
            int subClipX0 = glMax(x0, clipX0);
            int subClipX1 = glMin(x1, clipX1);
            if (subClipX1 > subClipX0) {

                cache[nCache].x0 = subClipX0;
                cache[nCache].x1 = subClipX1;
                cache[nCache].y = y;
                cache[nCache].rgb = rgb;
                nCache++;

                if (nCache == CACHE) {
                    m_blockDraw(cache, nCache);
                    nCache = 0;
                }
            }
        }
        rgb = AddToColorStack(ae->layer, ae->color);
        x0 = x1;
    }
    ASSERT(m_nColor == 0);
    m_nColor = 0;
    if (nCache) {
        m_blockDraw(cache, nCache);
    }
}

void VRender::Rasterize()
{
#ifndef _MSC_VER
    static ProfileData data("VRender::Rasterize");
    ProfileBlock block(&data);
#endif

    Rect clip = Rect::Intersect(m_size, m_clip);

    for (int j = 0; j < clip.y1; ++j) {
        IncrementActiveEdges(j);
        AddStartingEdges(j);
        SortActiveEdges();
        RasterizeLine(j, clip);
    }
    ClearTransform();
}
