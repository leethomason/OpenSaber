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

#ifndef SABER_DB_INCLUDED
#define SABER_DB_INCLUDED

#include <stdint.h>
#include "./src/util/Grinliz_Arduino_Util.h"
#include "pins.h"
#include "rgb.h"

class Manifest;

static constexpr int NUM_PALETTES = 8;
static constexpr int NUM_VOLUMES = 4;

class SaberDB
{
public:
    SaberDB();

    int paletteIndex() const { return m_currentPalette; }
    void setPalette(int n);

    static int toVolume256(int volume4);
    static int toVolume4(int volume256);

    // Palette
    int soundFont() const {
        return palette[m_currentPalette].soundFont;
    }
    void setSoundFont(int f);

    struct Palette {
        int soundFont;
        osbr::RGB bladeColor;
        osbr::RGB impactColor;

        void set(int font, const osbr::RGB& blade, const osbr::RGB& impact) 
        {
            soundFont = font;
            bladeColor = blade;
            impactColor = impact;
        }
    };

    void setPalette(int i, const Palette& palette);
    const SaberDB::Palette* getPalette(int i) const;
    const SaberDB::Palette* getPalette() const { return &palette[m_currentPalette]; }

private:
    int m_currentPalette = 0;
    float m_motion = DEFAULT_G_FORCE_MOTION;
    float m_impact = DEFAULT_G_FORCE_IMPACT;
    Palette palette[NUM_PALETTES];
};


#endif // SABER_DB_INCLUDED
