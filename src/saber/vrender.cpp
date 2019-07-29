#include "vrender.h"
#include "fixed.h"
#include "Grinliz_Util.h"
#include <stdio.h>

VRender::VRender()
{
    ClearTransform();
    Clear();
}

void VRender::Clear() 
{
    m_nEdge = 0;
    m_nActive = 0;
    m_layer = 0;
    m_nColor = 1;
    m_colorStack[0].Set(LAYER_BACKGROUND, osbr::RGBA(0, 0, 0, 255));
}

void VRender::Edge::Clear()
{
    this->layer = layer;
    this->yAdd = 0;
    this->nextStart = 0;
}

void VRender::Edge::Init(int x0, int y0, int x1, int y1, int layer, const osbr::RGBA& rgba) 
{
    Clear();

    this->x0 = x0;
    this->y0 = y0;
    this->x1 = x1;
    this->y1 = y1;
    this->layer = layer;

    this->color.r = rgba.r;
    this->color.g = rgba.g;
    this->color.b = rgba.b;
    this->color.a = rgba.a;
}


void VRender::DrawRect(int x0, int y0, int w, int h, const osbr::RGBA& rgba, int outline)
{
    Rect in(x0, y0, w + x0, h + y0);
    ASSERT(m_nEdge + 4 <= MAX_EDGES);
    ASSERT(in.y0 <= in.y1);
    ASSERT(w > 0);
    ASSERT(h > 0);
    ASSERT(m_layer < 127);

    if (!m_layerFixed)
        m_layer++;
    StartEdges();
    if (m_rot != 0)
        m_edge[m_nEdge++].Init(in.x0, in.y0, in.x1, in.y0, m_layer, rgba);
    m_edge[m_nEdge++].Init(in.x1, in.y0, in.x1, in.y1, m_layer, rgba);
    if (m_rot != 0)
        m_edge[m_nEdge++].Init(in.x1, in.y1, in.x0, in.y1, m_layer, rgba);
    m_edge[m_nEdge++].Init(in.x0, in.y1, in.x0, in.y0, m_layer, rgba);

    if (outline) {
        if (m_rot != 0)
            m_edge[m_nEdge++].Init(in.x0 + outline, in.y0 + outline, in.x1 - outline, in.y0 + outline, m_layer, rgba);
        m_edge[m_nEdge++].Init(in.x1 - outline, in.y0 + outline, in.x1 - outline, in.y1 - outline, m_layer, rgba);
        if (m_rot != 0)
            m_edge[m_nEdge++].Init(in.x1 - outline, in.y1 - outline, in.x0 + outline, in.y1 - outline, m_layer, rgba);
        m_edge[m_nEdge++].Init(in.x0 + outline, in.y1 - outline, in.x0 + outline, in.y0 + outline, m_layer, rgba);
    }

    EndEdges();
}

void VRender::DrawPoly(const Vec2* points, int n, const osbr::RGBA& rgba)
{
    if (!m_layerFixed)
        m_layer++;
    StartEdges();
    for (int i = 1; i < n; ++i) {
        ASSERT(m_nEdge < MAX_EDGES);
        m_edge[m_nEdge++].Init(points[i - 1].x, points[i - 1].y, points[i].x, points[i].y, m_layer, rgba);
    }
    if (points[n - 1] != points[0]) {
        ASSERT(m_nEdge < MAX_EDGES);
        m_edge[m_nEdge++].Init(points[n - 1].x, points[n - 1].y, points[0].x, points[0].y, m_layer, rgba);
    }
    EndEdges();
}

void VRender::StartEdges()
{
    m_start = m_nEdge;
    m_end = m_nEdge;
}

void VRender::EndEdges()
{
    m_end = m_nEdge;

    if (m_transX != 0 || m_transY != 0 || m_rot != 0) {
        FixedNorm s = iSin(m_rot);
        FixedNorm c = iCos(m_rot);

        for (int i = m_start; i < m_end; i++) {
            Fixed115 x0 = m_edge[i].x0;
            Fixed115 y0 = m_edge[i].y0;
            Fixed115 x1 = m_edge[i].x1;
            Fixed115 y1 = m_edge[i].y1;

            m_edge[i].x0 = x0 * c - y0 * s + m_transX;
            m_edge[i].y0 = x0 * s + y0 * c + m_transY;

            m_edge[i].x1 = x1 * c - y1 * s + m_transX;
            m_edge[i].y1 = x1 * s + y1 * c + m_transY;

            m_edge[i].Align();
        }
    }
    else {
        for (int i = m_start; i < m_end; i++) {
            m_edge[i].Align();
        }
    }
}


void VRender::Render()
{
    ClearTransform();
    Rect clip = m_size.Intersect(m_clip);
    ASSERT(m_nEdge + 1 < MAX_EDGES);

    SortToStart();
    Rasterize();
}

void VRender::SortToStart()
{
    memset(m_rootHash, 0, sizeof(Edge*)*Y_HASH);

    for (int i = 0; i < m_nEdge; ++i) {
        Edge* e = m_edge + i;
        if (e->Horizontal()) 
            continue;
        ASSERT(e->y0 <= e->y1);   // Should be sorted by the Start/End

        int yAdd = 0;
#if defined(_MSC_VER) && defined(_DEBUG)
        /*  FIXME
            The patching works. (Adding 1 or subtracting 1.) Which implies
            there is a way to do the division so it always ends up at the correct result.
            But haven't worked it out yet.

        int x1Slope = (e->x0 + e->slope * (e->y1 - e->y0)).getInt();
        int x1Literal = e->x1.getInt();
        if (x1Slope < x1Literal) e->slope.setRaw(e->slope.getRaw() + 1);
        if (x1Slope > x1Literal) e->slope.setRaw(e->slope.getRaw() - 1);
        int x1SlopeP = (e->x0 + e->slope * (e->y1 - e->y0)).getInt();
        int x1LiteralP = e->x1.getInt();
        ASSERT(x1SlopeP == x1LiteralP);
        */
#endif

        if (e->y0 < 0) {
            yAdd = 0;
        }
        else if (e->y0.getDec() == 0) {
            yAdd = e->y0.getInt();
        }
        else {
            yAdd = e->y0.getInt() + 1;
        }
        e->yAdd = (uint8_t)yAdd;
        int yHash = yAdd % Y_HASH;
        m_edge[i].nextStart = m_rootHash[yHash];
        m_rootHash[yHash] = e;

#if defined(_MSC_VER) && defined(_DEBUG)
        //printf("Sorting start=%d (%.2f,%.2f)-(%.2f,%.2f)\n",
        //    e->layer,
        //    e->x0.toFloat(), e->y0.toFloat(), e->x1.toFloat(), e->y1.toFloat());
#endif
    }
}


void VRender::IncrementActiveEdges(int y)
{
    int n = m_nActive;
    ActiveEdge* ae = m_activeEdges;
    for(int i=0; i<n; ++i) {
        // Set the new edge x value, cull the no longer active.
        // Will sort int the next stage.
        if (y >= m_activeEdges[i].yEnd) {
            // Do nothing; will get culled.
        }
        else {
            *ae = m_activeEdges[i];
            ae->x += ae->slope;   
            ++ae;
        }
    }
    m_nActive = ae - m_activeEdges;
}

void VRender::AddStartingEdges(int y)
{
    for (Edge* e = m_rootHash[y % Y_HASH]; e; e = e->nextStart) {
        // Things that are added before 0 have yAdd of 0.
        // For everything else it is a hash.
        if (e->yAdd == y) {
            if (e->Horizontal()) 
                continue;
            if (e->y1 < y) 
                continue;

            ASSERT(m_nActive < MAX_ACTIVE);
            ActiveEdge* ae = m_activeEdges + m_nActive;
            m_nActive++;
            ASSERT(e->y1 >= e->y0);
            ae->slope = (e->x1 - e->x0) / (e->y1 - e->y0);
            ae->x = (Fixed115(y) - e->y0) * ae->slope + e->x0;
            ae->yEnd = e->y1.getDec() ? (e->y1.getInt() + 1) : e->y1.getInt();
            ae->layer = e->layer;
            ae->color = e->color;

#if defined(_MSC_VER) && defined(_DEBUG)
            //printf("Adding start=%d (%.2f,%.2f)-(%.2f,%.2f)\n",
            //    e->layer,
            //    e->x0.toFloat(), e->y0.toFloat(), e->x1.toFloat(), e->y1.toFloat());
#endif
        }
    }
}


void VRender::SortActiveEdges()
{
    for (int i = 1; i < m_nActive; ++i) {
        int j = i;
        while (j > 0 && m_activeEdges[j - 1].x > m_activeEdges[j].x) {
            SwapAE(&m_activeEdges[j - 1], &m_activeEdges[j]);
            j--;
        }
    }
}


osbr::RGB VRender::AddToColorStack(int layer, const osbr::RGBA& color, bool* empty)
{
    // Toggles layers. (even-odd rule)
    ASSERT(m_nColor > 0);   // should be a black background, always
    ASSERT(layer > LAYER_BACKGROUND);
    ASSERT(m_colorStack[0].layer == LAYER_BACKGROUND);

    for (int i = m_nColor; i > 0; --i) {
        if (m_colorStack[i - 1].layer == layer) {
            for (int j = i; j < m_nColor; ++j) {
                m_colorStack[j - 1] = m_colorStack[j];
            }
            --m_nColor;
            break;
        }
        else if (m_colorStack[i-1].layer < layer) {
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

    osbr::RGB rgb(0);
    int start = m_nColor - 1;
    for (; start >= 0; start--) {
        if (m_colorStack[start].color.a == 255)
            break;
    }
    for (int i = start; i < m_nColor; ++i) {
        osbr::RGBA c = m_colorStack[i].color;
        // ARM M0+ doesn't have division.
        if (c.a == 255) {
            rgb.r = c.r;
            rgb.g = c.g;
            rgb.b = c.b;
        }
        else {
            rgb.r = (c.r * c.a + rgb.r * (255 - c.a)) >> 8;
            rgb.g = (c.g * c.a + rgb.g * (255 - c.a)) >> 8;
            rgb.b = (c.b * c.a + rgb.b * (255 - c.a)) >> 8;
        }
    }
    *empty = (m_nColor == 0);
    return rgb;
}

void VRender::RasterizeLine(int y, const Rect& clip)
{
    static const int CACHE = 8;
    BlockDrawChunk cache[CACHE];
    int nCache = 0;

    if (m_nActive == 0)
        return;
    if (y < clip.y0 || y >= clip.y1)
        return;

    // Edges are sorted. Walk right to left.
    int x0 = m_activeEdges[0].x.getInt();
    osbr::RGB rgb(0);
    int clipX0 = clip.x0;
    int clipX1 = clip.x1;

    // Edges can go away under the current active, or abut.
    // Intentionally trying to keep this loop simple without look-ahead, etc.
    ActiveEdge* ae = m_activeEdges;
    for(int i=0; i<m_nActive; ++i, ++ae) {
        // If the color stack doesn't change, we don't need to draw. But be
        // wary of the boundary condition where it's a black bacground,
        // which is why the empty is detected, else it will never draw.
        bool empty = false;
        osbr::RGB newRGB = AddToColorStack(ae->layer, ae->color, &empty);
        if (newRGB != rgb || empty) {
            int x1 = ae->x.getInt();

            // Rasterize previous chunk.
            if (x1 > x0) {
                int subClipX0 = Max(x0, clipX0);
                int subClipX1 = Min(x1, clipX1);
                if (subClipX1 > subClipX0) {

                    cache[nCache].x0 = subClipX0;
                    cache[nCache].x1 = subClipX1;
                    cache[nCache].rgb = rgb;
                    nCache++;

                    if (nCache == CACHE) {
                        m_blockDraw(cache, y, nCache);
                        nCache = 0;
                    }
                }
            }
            x0 = x1;
            rgb = newRGB;
        }
    }
    if (nCache) {
        m_blockDraw(cache, y, nCache);
    }
}

void VRender::Rasterize()
{
    Rect clip = m_size.Intersect(m_clip);

    for (int j = 0; j < clip.y1; ++j) {
        IncrementActiveEdges(j);
        AddStartingEdges(j);
        SortActiveEdges();
        RasterizeLine(j, clip);
#if defined(_MSC_VER) && defined(_DEBUG)
        if (m_nColor != 1) {
            printf("ASSERTION\n");
            for (int i = 0; i < m_nColor; ++i) {
                printf("Color: [%d] layer=%d (%d,%d,%d)\n", i, m_colorStack[i].layer,
                    m_colorStack[i].color.r, m_colorStack[i].color.g, m_colorStack[i].color.b);
            }
        }
#endif
        ASSERT(m_nColor == 1);  // black background always there
    }
    ClearTransform();
}


#define FONT_BIT_SET(arr, x, y) (arr[x] & (1<<y))

void VRenderUtil::DrawStr(VRender* ren, const char* str, int x, int y, GlyphMetrics metrics, const osbr::RGBA& rgba)
{
    ren->PushLayer();
    int cx = x;
    while (str && *str) {
        int advance = 0, texW = 0, texRows = 0;
        const uint8_t* mem = metrics(*str, &advance, &texW, &texRows);

        int i0 = 0;
        while(i0 < texW) {
            int j0 = 0;
            uint8_t c = mem[i0];
            int i1 = i0 + 1;
            while (i1 < texW && mem[i1] == c)
                ++i1;

            while (j0 < 8) {
                int j1 = j0;
                while (c & (1 << j1) && j1 < 8) {
                    j1++;
                }
                if (j1 > j0) {
                    ren->DrawRect(cx + i0, y + j0, i1-i0, j1 - j0, rgba);
                    j0 = j1;
                }
                j0++;
            }
            i0 = i1;
        }
        cx += advance;
        ++str;
    }
    ren->PopLayer();
}

