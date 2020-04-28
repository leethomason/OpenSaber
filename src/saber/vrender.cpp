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


void VRender::DrawRect(int x0, int y0, int w, int h, int c, int outline)
{
    Rect in(x0, y0, w + x0, h + y0);
    ASSERT(in.y0 <= in.y1);
    ASSERT(in.x0 <= in.x1);

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
    Rasterize();
}

void VRender::DrawPoly(const Vec2* points, int n, int color)
{
    for (int i = 1; i < n; ++i) {
        CreateActiveEdge(points[i - 1].x, points[i - 1].y, points[i].x, points[i].y, color);
    }
    if (points[n - 1] != points[0]) {
        CreateActiveEdge(points[n - 1].x, points[n - 1].y, points[0].x, points[0].y, color);
    }
    Rasterize();
}


void VRender::DrawPoly(const Vec2I8* points, int n, int color)
{
    for (int i = 1; i < n; ++i) {
        CreateActiveEdge(points[i - 1].x, points[i - 1].y, points[i].x, points[i].y, color);
    }
    if (points[n - 1] != points[0]) {
        CreateActiveEdge(points[n - 1].x, points[n - 1].y, points[0].x, points[0].y, color);
    }
    Rasterize();
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

void VRender::CreateActiveEdge(int x0, int y0, int x1, int y1, int c)
{
    Fixed115 p[4];

    Transform4(p, x0, y0, x1, y1);

    if (p[3] < p[1]) {
        glSwap(p[0], p[2]);
        glSwap(p[1], p[3]);
    }

    InnerCreateActiveEdge(p[0], p[1], p[2], p[3], c);
}


void VRender::InnerCreateActiveEdge(Fixed115 x0, Fixed115 y0, Fixed115 x1, Fixed115 y1, int c)
{
    ASSERT(m_nPool < MAX_EDGES);
    ActiveEdge* ae = &m_edgePool[m_nPool];
    ae->color = c;

    // If horizontal, doesn't render.
    //if (y0.getInt() == y1.getInt())
    //    return;

    // yAdd will be the scan line after...unless it is zero.
    int yAdd = 0;
    if (y0 < 0) {
        yAdd = 0;
    }
    else if (y0.getDec() == 0) {
        yAdd = y0.getInt();
    }
    else {
        yAdd = y0.getInt() + 1;
    }
    if (yAdd >= Y_HASH) {
        //printf("Discard: yAdd=%d\n", yAdd);
        return;
    }

    ASSERT(yAdd >= 0 && yAdd < Y_HASH);
    ASSERT(yAdd >= y0);

    m_nPool++;

    // yEnd is similarily exacting.
    if (y1.getDec() == 0)
        ae->yEnd = y1.getInt();
    else
        ae->yEnd = y1.getInt() + 1;

    ae->slope16 = ((x1.x - x0.x) << 16) / (y1.x - y0.x);
    int32_t dy16 = (yAdd << 16) - y0.convertRaw(16);
    ae->x16 = x0.convertRaw(16) + (ae->slope16 >> 8) * (dy16 >> 8);

    ASSERT(Y_HASH >= m_size.CY());
    ae->next = m_rootHash[yAdd];
    m_rootHash[yAdd] = ae;

    static const float INV = 1.0f / 65536.0f;
    //printf("Added: x=%f slope=%f yAdd=%d\n", ae->x16 * INV, ae->slope16 * INV, yAdd);
}


void VRender::Render()
{
    ClearTransform();
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
            (*ae)->x16 += (*ae)->slope16;
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
        while (j > 0 && m_activeEdges[j - 1]->x16 > m_activeEdges[j]->x16) {
            glSwap(m_activeEdges[j - 1], m_activeEdges[j]);
            j--;
        }
    }
#ifdef _DEBUG
    for (int i = 1; i < m_nActive; ++i) {
        ASSERT(m_activeEdges[i]->x16 >= m_activeEdges[i - 1]->x16);
    }
#endif
}

void VRender::RasterizeLine(int y, const Rect& clip)
{
    if (m_nActive == 0)
        return;
    if (y < clip.y0 || y >= clip.y1)
        return;

    ASSERT(m_nColor == 0);
    static const int CACHE = 16;
    BlockDrawChunk cache[CACHE];
    int nCache = 0;

    //ASSERT(m_nActive % 2 == 0);
    if (m_nActive % 2)
        return;

    // Edges are sorted. Walk right to left.
    int clipX0 = clip.x0;
    int clipX1 = clip.x1;

    // Intentionally trying to keep this loop simple without look-ahead, etc.
    for(int i=0; i<m_nActive; i+=2) {
        ActiveEdge* left = m_activeEdges[i];
        ActiveEdge* right = m_activeEdges[i + 1];

        int x0 = left->x16 >> 16;
        int x1 = right->x16 >> 16;

        // Rasterize previous chunk.
        if (x1 > x0) {
            int subClipX0 = glMax(x0, clipX0);
            int subClipX1 = glMin(x1, clipX1);
            if (subClipX1 > subClipX0) {
                ASSERT(nCache < CACHE);
                cache[nCache].x0 = subClipX0;
                cache[nCache].x1 = subClipX1;
                cache[nCache].y = y;
                cache[nCache].color = 1;
                nCache++;
            }
        }
    }
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
}
