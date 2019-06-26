#include "vrender.h"
#include "fixed.h"
#include "Grinliz_Util.h"

void VRender::Clear(const osbr::RGBA rgba) 
{
    Rect clip = m_size.Intersect(m_clip);
    if (!clip.Empty()) {
        m_blockDraw(clip.x0, clip.y0, clip.x1, clip.y1, rgba);
    }
}

void VRender::DrawRect(int x0, int y0, int w, int h, const osbr::RGBA& rgba)
{
    Rect in(x0, y0, w + x0, h + y0);
    Rect r = m_size.Intersect(m_clip).Intersect(in);
    if (!r.Empty()) {
        m_blockDraw(r.x0, r.y0, r.x1, r.y1, rgba);
    }
}



/*
void VRender16::Set(int w, int h)
{
    m_width = w;
    m_height = h;
}

void VRender16::Attach(uint16_t* buffer,       // Memory for the RGB data
    const Rect& bufferRect,         // Defines the bounds of the buffer 
    const Rect& clip)
{
    m_buffer = buffer;
    m_bufferRect = bufferRect;
    m_clip = clip;
}

void VRender16::Clear(const osbr::RGBA rgba)
{
    uint16_t c = ToU16(rgba);
    const int area = m_bufferRect.Area();
    for (int i = 0; i < area; ++i) {
        m_buffer[i] = c;
    }
}


void VRender16::DrawLine(int x0, int y0, int x1, int y1, const osbr::RGBA& rgba)
{
    Fixed115 step;
    uint16_t c = ToU16(rgba);
    int pitch = m_bufferRect.CX();

    int dx = x1 - x0 + 1;
    int dy = y1 - y0 + 1;
    if (glAbs(dx) > glAbs(dy)) {
        if (x0 > x1) {
            Swap(&x0, &x1); Swap(&y0, &y1);
        }
        dx = glAbs(dx);
        step.balancedDiv(dx, dy);
        Fixed115 y(y0);
        for (int i = 0; i < dx; ++i, y += step) {
            m_buffer[(y.getInt() - m_bufferRect.y0)*pitch + x0 - m_bufferRect.x0 + i] = c;
        }
    }
}
*/