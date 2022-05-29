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
#include <stdio.h>

#ifndef _WIN32
#include "./src/util/Grinliz_Arduino_Util.h" // profiling
#endif

const VRender::Vec2 VRender::VECEND(-111, -222);

VRender::VRender()
{
    ClearTransform();
    Clear();
}

void VRender::Clear() 
{
    m_nActive = 0;
    m_nPool = 0;
    ASSERT(Y_HASH >= m_size.CY());
    memset(m_rootHash, 0, sizeof(ActiveEdge*)*Y_HASH);
}

void VRender::ClearTransform()
{
    m_rot = FixedNorm{ 0 };
    m_transX = Fixed115{ 0 };
    m_transY = Fixed115{ 0 };
    m_scaleX = Fixed115{ 1 };
    m_scaleY = Fixed115{ 1 } ;
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

void VRender::DrawPoly(const Vec2* points[], int n, int color)
{
    for (int i = 0; i < n; ++i) {
        int j = 0;
        const Vec2* p = points[i];

        while (p[j+1] != VECEND) {
            CreateActiveEdge(p[j].x, p[j].y, p[j+1].x, p[j+1].y, color);
            ++j;
        }
        CreateActiveEdge(p[j].x, p[j].y, p[0].x, p[0].y, color);
    }
    Rasterize();
}


void VRender::Transform4(Fixed115* e, int x0, int y0, int x1, int y1)
{
    Fixed115 camTX = Fixed115{ camTrans.x };
    Fixed115 camTY = Fixed115{ camTrans.y };
    Fixed115 camSX = Fixed115{ camScale.x };
    Fixed115 camSY = Fixed115{ camScale.y };

    if (m_rot != FixedNorm{ 0 }) {
        Fixed115 s = Fixed115{ sin(m_rot) };
        Fixed115 c = Fixed115{ cos(m_rot) };

        Fixed115 xp0 = x0 * m_scaleX;
        Fixed115 yp0 = y0 * m_scaleY;
        Fixed115 xp1 = x1 * m_scaleX;
        Fixed115 yp1 = y1 * m_scaleY;

        e[0] = (xp0 * c - yp0 * s + m_transX) * camSX + camTX;
        e[1] = (xp0 * s + yp0 * c + m_transY) * camSY + camTY;
        e[2] = (xp1 * c - yp1 * s + m_transX) * camSX + camTX;
        e[3] = (xp1 * s + yp1 * c + m_transY) * camSY + camTY;
    }
    else if (m_scaleX != Fixed115{ 1 } || m_scaleY != Fixed115{ 1 }) {
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

    // If horizontal (to the pixel( doesn't render, but that will be filtered by the line walking.
    // If the same value, pull out so it doesn't cause a divide by zero.
    if (y0 == y1) return;

    // yAdd and yEnd are fiddly.
    // yAdd will be the scan line after...unless it is zero.
    int yAdd = 0;
    if (y0 < kZero_Fixed115) {
        yAdd = 0;
    }
    else if (isInt(y0)) {
        yAdd = static_cast<int>(y0);
    }
    else {
        yAdd = static_cast<int>(y0) + 1;
    }
    if (yAdd >= Y_HASH) {
        //printf("Discard: yAdd=%d\n", yAdd);
        return;
    }

    ASSERT(yAdd >= 0 && yAdd < Y_HASH);
    ASSERT(Fixed115{ yAdd } >= y0);

    m_nPool++;

    // yEnd is similarily exacting.
    if (isInt(y1))
        ae->yEnd = static_cast<int>(y1);
    else
        ae->yEnd = static_cast<int>(y1) + 1;

    ae->slope = Fixed1616{ x1 - x0 } / Fixed1616{ y1 - y0 };
    ae->x = Fixed1616{ x0 } + ae->slope * (Fixed1616{ yAdd } - Fixed1616{ y0 });

    ASSERT(Y_HASH >= m_size.CY());
    ae->next = m_rootHash[yAdd];
    m_rootHash[yAdd] = ae;

    //static const float INV = 1.0f / 65536.0f;
    //printf("Added: x=%f slope=%f yAdd=%d\n", ae->x16 * INV, ae->slope16 * INV, yAdd);
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

void VRender::RasterizeLine(int y, const Rect& clip)
{
    if (m_nActive == 0)
        return;
    if (y < clip.y0 || y >= clip.y1)
        return;

    static const int CACHE = 16;
    BlockDrawChunk cache[CACHE];
    int nCache = 0;

    ASSERT(m_nActive % 2 == 0);
    if (m_nActive % 2)
        return;

    // Edges are sorted. Walk right to left.
    int clipX0 = clip.x0;
    int clipX1 = clip.x1;

    // Intentionally trying to keep this loop simple without look-ahead, etc.
    for(int i=0; i<m_nActive; i+=2) {
        ActiveEdge* left = m_activeEdges[i];
        ActiveEdge* right = m_activeEdges[i + 1];

        int x0 = static_cast<int>(left->x);
        int x1 = static_cast<int>(right->x);

        // Rasterize previous chunk.
        if (x1 > x0) {
            int subClipX0 = glMax(x0, clipX0);
            int subClipX1 = glMin(x1, clipX1);
            if (subClipX1 > subClipX0) {
                ASSERT(nCache < CACHE);
                cache[nCache].x0 = subClipX0;
                cache[nCache].x1 = subClipX1;
                cache[nCache].y = y;
                cache[nCache].color = left->color;
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
#ifndef _WIN32
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
    Clear();
}
