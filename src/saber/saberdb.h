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

class SaberDB
{
public:
    SaberDB();

    void nextPalette();
    int paletteIndex() const      {
        return m_currentPalette;
    }
    void setPalette(int n);
    void setPaletteFromDirHash(uint32_t h);

    bool soundOn() const          {
        return m_volume > 0;
    }

    uint8_t volume()              {
        return m_volume;
    }
    void setVolume(int v);

    uint8_t volume4() const; // an approximation
    // Turns sonud on/off, and/or sets volume.
    void setVolume4(int vol);

    // Palette
    const osbr::RGB& bladeColor() const   {
        return palette[m_currentPalette].bladeColor;
    }
    void setBladeColor(const osbr::RGB& color);

    const osbr::RGB& impactColor() const  {
        return palette[m_currentPalette].impactColor;
    }
    void setImpactColor(const osbr::RGB& color);

    int soundFont() const {
        return palette[m_currentPalette].soundFont;
    }
    void setSoundFont(int f);

    static const int NUM_PALETTES = 8;

    struct Palette {
        osbr::RGB bladeColor;
        osbr::RGB impactColor;
        int soundFont;

        static const int NAUDIO = 4;
        int channelBoost[NAUDIO];

        void set(const osbr::RGB& blade, const osbr::RGB& impact, int font, 
                 int humBoost=256, int swingBoost=256, int eventBoost=256 ) 
        {
            bladeColor = blade;
            impactColor = impact;
            soundFont = font;
            channelBoost[0] = humBoost;
            channelBoost[1] = swingBoost;
            channelBoost[2] = swingBoost;
            channelBoost[3] = eventBoost;
        }
    };

    const SaberDB::Palette* getPalette(int i) const;
    const SaberDB::Palette* getPalette() const { return &palette[m_currentPalette]; }

private:
    int m_currentPalette = 0;
    int m_volume = 40;
    float   m_motion = DEFAULT_G_FORCE_MOTION;
    float   m_impact = DEFAULT_G_FORCE_IMPACT;
    Palette palette[NUM_PALETTES];
};


#endif // SABER_DB_INCLUDED
