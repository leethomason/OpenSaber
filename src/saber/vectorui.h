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
#include "modes.h"
#include "uirenderdata.h"

class VRender;
class Renderer;

class VectorUI
{
public:
    VectorUI();

    void Draw(
        VRender* vRender,   // vector renderer
        Renderer* bRender,  // bitmap renderer
        uint32_t time, 
        UIMode mode, 
        bool bladeIgnited, 
        const UIRenderData* data);

private:
    static const int BAR_W = 16;
    static const int W = 128;
    static const int H = 32;
    static const int TEXT = 5;

    uint32_t lastTime = 0;

    AnimateProp colorProp[3];
    int hProp = 0;
    int hTime = 0;

    static void Segment(VRender* ren, int width, int s, int num, int rgba);
    static void DrawBar(VRender* ren, int x, int y, int width, int color, int fraction);
    static void DrawMultiBar(VRender* ren, int x, bool flip, int yCutoff);
    static void DrawColorHSV(VRender* ren, int x, int h);
    static void DrawBank(VRender* ren, int x, int y, int state);    // 0 off, 100 on, 200 off
};
