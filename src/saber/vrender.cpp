#include "vrender.h"
#include "fixed.h"
#include "Grinliz_Util.h"


VRender::VRender()
{
    m_activeRoot = 0;
}

void VRender::Clear(const osbr::RGB rgb) 
{
    Rect clip = m_size.Intersect(m_clip);
    if (!clip.Empty()) {
        m_blockDraw(clip.x0, clip.y0, clip.x1, clip.y1, rgb);
    }
}

void VRender::DrawRect(int x0, int y0, int w, int h, const osbr::RGBA& rgba)
{
    Rect in(x0, y0, w + x0, h + y0);
    ASSERT(m_nEdge + 1 < MAX_EDGES);
    ASSERT(in.y0 <= in.y1);
    m_layer++;
    m_edge[m_nEdge++].Init(in.x0, in.y0, in.x0, in.y1, m_layer, rgba);
    m_edge[m_nEdge++].Init(in.x1, in.y0, in.x1, in.y1, m_layer, rgba);
}

void VRender::DrawPoly(const Vec2* points, int n, const osbr::RGBA& rgba)
{
    m_layer++;
    for (int i = 1; i < n; ++i) {
        ASSERT(m_nEdge < MAX_EDGES);
        m_edge[m_nEdge++].Init(points[i - 1].x, points[i - 1].y, points[i].x, points[i].y, m_layer, rgba);
    }
    if (points[n - 1] != points[0]) {
        ASSERT(m_nEdge < MAX_EDGES);
        m_edge[m_nEdge++].Init(points[n - 1].x, points[n - 1].y, points[0].x, points[0].y, m_layer, rgba);
    }
}

void VRender::Render()
{
    Rect clip = m_size.Intersect(m_clip);
    ASSERT(m_nEdge + 1 < MAX_EDGES);
    osbr::RGBA rgba(0);
    m_edge[m_nEdge++].Init(clip.x0, clip.y0, clip.x0, clip.y1, Edge::LAYER_BACKGROUND, rgba);
    m_edge[m_nEdge++].Init(clip.x1, clip.y0, clip.x1, clip.y1, Edge::LAYER_BACKGROUND, rgba);

    // Sort the edges by y0? Or bin them. Faster to bin, of course, but a bit more memory.
    SortToStart();
    Rasterize();

    // Delete the background.
    m_nEdge -= 2;
}

void VRender::SortToStart()
{
    memset(m_rootHash, 0, sizeof(Edge*)*Y_HASH);

    for (int i = 0; i < m_nEdge; ++i) {
        m_edge[i].Align();
        Fixed115 y = m_edge[i].y0;
        if (y < 0) y = 0;
        int yHash = y.getInt() % Y_HASH;
        m_edge[i].nextStart = m_rootHash[yHash];
        m_rootHash[yHash] = m_edge + i;
    }
}


void VRender::IncrementActiveEdges(int y)
{
    Edge* e = m_activeRoot;
    Edge* prev = 0;

    // Set the new edge x value, cull the no longer active.
    // Will sort int the next stage.
    while (e) {
        if (y >= e->y1) {
            if (prev) prev->nextActive = e->nextActive;
            else m_activeRoot = e->nextActive;
        }
        else {
            e->x += e->slope;   
            prev = e;
        }
        e = e->nextActive;
    }
}

void VRender::AddStartingEdges(int y)
{
    for (Edge* e = m_rootHash[y % Y_HASH]; e; e = e->nextStart) {
        if (e->y0 == y) {
            if (e->Horizontal()) continue;
            e->nextActive = m_activeRoot;
            m_activeRoot = e;
            e->x = e->x0;
            e->slope = (e->x1 - e->x0) / (e->y1 - e->y0);
        }
    }
}


void VRender::SortActiveEdges()
{
    if (!m_activeRoot || !m_activeRoot->nextActive) return;

    // Resulting list:
    Edge* head = 0;
    while (m_activeRoot) {
        Edge* current = m_activeRoot;
        m_activeRoot = m_activeRoot->nextActive;

        if (!head || current->x < head->x) {
            current->nextActive = head;
            head = current;
        }
        else {
            Edge* p = head;
            while (p) {
                if (!p->nextActive || (current->x < p->nextActive->x)) {
                    current->nextActive = p->nextActive;
                    p->nextActive = current;
                    break;
                }
                p = p->nextActive;
            }
        }
    }
    m_activeRoot = head;
}


osbr::RGB VRender::AddToColorStack(int layer, const osbr::RGBA& color)
{
    // Toggles layers. (even-odd rule)
    bool added = false;
    for (int i = 0; i < m_nColor; ++i) {
        if (m_colorStack[i].layer == layer) {
            // A match, remove.
            for (int j = i + 1; j < m_nColor; ++j) {
                m_colorStack[j - 1] = m_colorStack[j];
            }
            m_nColor--;
            added = true;
            break;
        }
        else if (layer > m_colorStack[i].layer 
            && i < (m_nColor-1) 
            && layer < m_colorStack[i+1].layer)
        {
            // Add after.
            ASSERT(m_nColor < MAX_COLOR_STACK);
            for (int j = m_nColor; j > i; --j) {
                m_colorStack[j] = m_colorStack[j - 1];
            }
            m_colorStack[i].Set(layer, color);
            ++m_nColor;
            added = true;
            break;
        }
    }
    if (!added) {
        ASSERT(m_nColor < MAX_COLOR_STACK);
        m_colorStack[m_nColor++].Set(layer, color);
    }

    osbr::RGB rgb(0);
    int start = m_nColor - 1;
    for (; start > 0; start--) {
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
    return rgb;
}

void VRender::RasterizeLine(int y, const Rect& clip)
{
    // Edges are sorted. Walk right to left.
    int SENTINEL = -1000;
    int x0 = SENTINEL;
    osbr::RGB rgb(0);
    for (Edge* e = m_activeRoot; e; e = e->nextActive) {
        int x1 = e->x.getInt();
        // Rasterize previous chunk.
        if (x0 != SENTINEL) {
            Rect clipped = clip.Intersect(Rect(x0, y, x1, y + 1));
            if (!clipped.Empty())
                m_blockDraw(clipped.x0, y, clipped.x1, y + 1, rgb);
        }
        x0 = x1;
        rgb = AddToColorStack(e->layer, e->color);
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
        ASSERT(m_nColor == 0);
    }
}
