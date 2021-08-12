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

#include <Arduino.h>
#include "saberdb.h"
#include "sfx.h" // bug fix; generally don't want a dependency on sfx

using namespace osbr;

SaberDB::SaberDB()
{
    setPaletteFromDirHash(0);
}

void SaberDB::setPaletteFromDirHash(uint32_t h)
{
    switch (h)
    {
    case 0xfe1a3638:
    {
        Log.p("Bespin2/Jaina/Vader").eol();
        static const int BESPIN2 = 0;
        static const int JAINA = 1;
        static const int VADER = 2;

        palette[0].set(0x00ff00, 0x00ffa0, BESPIN2); // green
        palette[1].set(0x0000ff, 0x00c8ff, BESPIN2); // blue
        palette[2].set(0x00ffff, 0x00a0ff, BESPIN2); // cyan
        palette[3].set(0xff0000, 0xa08000, VADER);   // red
        palette[4].set(0xff6000, 0x808000, JAINA);   // orange
        palette[5].set(0x0044ff, 0x00ccff, JAINA);   // blue-green
        palette[6].set(0x508080, 0x30a0a0, JAINA);   // white
        palette[7].set(0x00ff44, 0x00ffaa, JAINA);   // green-blue
    }
    break;

    case 0x3f14313c:
    {
        Log.p("Bespin2/Jaina/SmoothJedi").eol();
        static const int BESPIN2 = 0;
        static const int JAINA = 1;
        static const int SMOOTH = 2;

        palette[0].set(0x00ff00, 0x00ffa0, BESPIN2); // green
        palette[1].set(0x0000ff, 0x00c8ff, SMOOTH);  // blue
        palette[2].set(0x00ffff, 0x00a0ff, BESPIN2); // cyan
        palette[3].set(0xff0000, 0xa08000, SMOOTH);   // red
        palette[4].set(0xff6000, 0x808000, JAINA);   // orange
        palette[5].set(0x0044ff, 0x00ccff, JAINA);   // blue-green
        palette[6].set(0x508080, 0x30a0a0, JAINA);   // white
        palette[7].set(0x00ff44, 0x00ffaa, JAINA);   // green-blue
    }
    break;

    case 0xd9a8993d:
    {
        Log.p("Hero - Jaina").eol();
        static const int HERO = 0;
        static const int JAINA = 1;

        palette[0].set(0x00ff00, 0x00ffa0, HERO); // green
        palette[1].set(0x0000ff, 0x00c8ff, HERO); // blue
        palette[2].set(0x00ffff, 0x00a0ff, HERO); // cyan
        palette[3].set(0xff0000, 0xa08000, JAINA);   // red
        palette[4].set(0xff6000, 0x808000, JAINA);   // orange
        palette[5].set(0x0044ff, 0x00ccff, JAINA);   // blue-green
        palette[6].set(0x508080, 0x30a0a0, JAINA);   // white
        palette[7].set(0x00ff44, 0x00ffaa, JAINA);   // green-blue
    }
    break;

    case 0xe13817f5:
    {
        Log.p("Fulcrum - TFA Graflex: NOT TUNED").eol();
        static const int FULCRUM = 0;
        static const int GRAFLEX = 1;

        palette[0].set(0x00ff00, 0x00ffa0, FULCRUM); // green
        palette[1].set(0x0000ff, 0x00c8ff, FULCRUM); // blue
        palette[2].set(0x00ffff, 0x00a0ff, FULCRUM); // cyan
        palette[3].set(0xff0000, 0xa08000, GRAFLEX);   // red
        palette[4].set(0xff6000, 0x808000, GRAFLEX);   // orange
        palette[5].set(0x0044ff, 0x00ccff, GRAFLEX);   // blue-green
        palette[6].set(0x508080, 0x30a0a0, GRAFLEX);   // white
        palette[7].set(0x00ff44, 0x00ffaa, GRAFLEX);   // green-blue
    }
    break;

    case 0xdf451848:
    {
        Log.p("Temple - Jaina").eol();
        static const int JAINA = 0;
        static const int TEMPLE = 1;

        palette[0].set(0x0044ff, 0x00ccff, JAINA);     // blue
        palette[1].set(0x00ff00, 0x00ffa0, TEMPLE);    // green
        palette[2].set(0x00ffff, 0x00a0ff, TEMPLE);    // cyan
        palette[3].set(0xff0000, 0xa08000, TEMPLE);    // red
        palette[4].set(0xff6000, 0x808000, JAINA);     // orange
        palette[5].set(0xffcc00, 0x808080, TEMPLE);    // yellow
        palette[6].set(0x808080, 0x30a0a0, JAINA);     // white
        palette[7].set(0x00ff44, 0x00ffaa, JAINA);     // green-blue
    }
    break;

    // Duchess / dutchess
    case 0x1deb9709:
    {
        Log.p("Jaina - BoldOne").eol();
        static const int JAINA = 0;
        static const int BOLD = 1;

        // FIXME: boost values are not tested!
        // And currently turned off.
        palette[0].set(0x0088ff, 0x44ccff, BOLD, 290, 290);     // blue
        palette[1].set(0x00ff00, 0x00ffa0, BOLD, 290, 290);     // green
        palette[2].set(0xC000FF, 0x80A080, JAINA);              // purple
        palette[3].set(0xff0000, 0xa08000, JAINA);              // red
        palette[4].set(0xff6000, 0x808000, JAINA);              // orange
        palette[5].set(0xffff00, 0x00FF88, BOLD, 290, 290);     // yellow
        palette[6].set(0x80A080, 0x30a0a0, JAINA);              // white
        palette[7].set(0x00ff44, 0x00ffaa, BOLD, 290, 290);     // green-blue
    }
    break;

    // Sisters.
    case 0x74288e03:
    case 0x66b046f6:
    {
        Log.p("Fulcrum/Graflex - Nightsister").eol();
        static const int FULCRUM = 0;   // Luna
        // static const int GRAFLEX = 0;   // Celestia
        static const int NIGHTSISTER = 1;
        palette[0].set(0x00ff00, 0x00ffa0, FULCRUM);              // green
        palette[1].set(0x0088ff, 0x80A080, FULCRUM);              // blue
        palette[2].set(0xCC00FF, 0x80A080, NIGHTSISTER);          // purple
        palette[3].set(0xff0000, 0xa08000, NIGHTSISTER);          // red
        palette[4].set(0xff6000, 0x808000, NIGHTSISTER);          // orange
        palette[5].set(0xffff00, 0x00FF88, FULCRUM);              // yellow
        palette[6].set(0x80A080, 0x30a0a0, FULCRUM);              // white
        palette[7].set(0x00ff44, 0x00ffaa, NIGHTSISTER);          // green-blue
    }
    break;

    // Protection
    case 0xade1fec7:
    {
        Log.p("Graflex/Bold").eol();
        static const int GRAFLEX = 0;
        static const int BOLD = 1;
        palette[0].set(0x00ff00, 0x00ffa0, GRAFLEX);       // green
        palette[1].set(0x0088ff, 0x80A080, BOLD);          // blue
        palette[2].set(0x00ffff, 0x00a0ff, GRAFLEX);       // cyan
        palette[3].set(0xff0000, 0xa08000, GRAFLEX);       // red
        palette[4].set(0xff7700, 0x808000, GRAFLEX);       // orange
        palette[5].set(0xCC00FF, 0x80A080, BOLD);          // purple
        palette[6].set(0x80A080, 0x30a0a0, BOLD);          // white
        palette[7].set(0x00ff44, 0x00ffaa, BOLD);          // green-blue
    }
    break;

    case 0xd447a9d5:    // corelln (Corellian SS) test font.
    default:
    {
        Log.p("corelln (Corellian SS) test font").eol();

        palette[0].set(0x00ff00, 0x00ffa0, 0);   // green
        palette[1].set(0x0000ff, 0x00c8ff, 0);   // blue
        palette[2].set(0x00ffff, 0x00a0ff, 0);   // cyan
        palette[3].set(0xff0000, 0xa08000, 0);   // red
        palette[4].set(0xff6000, 0x808000, 0);   // orange
        palette[5].set(0x0044ff, 0x00ccff, 0);   // blue-green
        palette[6].set(0x508080, 0x30a0a0, 0);   // white
        palette[7].set(0x00ff44, 0x00ffaa, 0);   // green-blue
    }
    break;
    }
}

void SaberDB::nextPalette()
{
    setPalette(m_currentPalette + 1);
}

void SaberDB::setPalette(int n)
{
#ifdef SABER_SOUND_ON
    // The SD card memory streaming and the use of the SD
    // file system lead to crashes. (Grr.) Stop the sound
    // here to see if it cleans up the problem.
    SFX::instance()->stopSound();
#endif

    m_currentPalette = abs(n) % NUM_PALETTES;
}

const SaberDB::Palette* SaberDB::getPalette(int i) const
{
    i = glClamp(i, 0, NUM_PALETTES - 1);
    return &palette[i];
}

void SaberDB::setVolume(int v) 
{
    m_volume = glClamp(v, 0, 512);  // can go over 256; but cap at something not crazy.
    Log.p("setVolume=").p(v).eol();
}

void SaberDB::setVolume4(int v)
{
    switch (v)
    {
    case 0:
        setVolume(0);
        break;
    case 1:
        setVolume(VOLUME_1);
        break;
    case 2:
        setVolume(VOLUME_2);
        break;
    case 3:
        setVolume(VOLUME_3);
        break;
    case 4:
        setVolume(VOLUME_4);
        break;
    }
}

uint8_t SaberDB::volume4() const
{
    if (!soundOn() || m_volume == 0)
    {
        return 0;
    }
    if (m_volume >= VOLUME_4) {
        return 4;
    }
    if (m_volume >= VOLUME_3) {
        return 3;
    }
    if (m_volume >= VOLUME_2) {
        return 2;
    }
    return 1;
}

void SaberDB::setMotion(float v)
{
    v = constrain(v, 1.1, 10.0);
    m_motion = v;
}

void SaberDB::setImpact(float v)
{
    v = constrain(v, 1.1, 10.0);
    m_impact = v;
}

void SaberDB::setBladeColor(const RGB &color)
{
    palette[m_currentPalette].bladeColor = color;
}

void SaberDB::setImpactColor(const RGB &color)
{
    palette[m_currentPalette].impactColor = color;
}

void SaberDB::setSoundFont(int f)
{
    palette[m_currentPalette].soundFont = glClamp(f, 0, 3); // fixme: use constants
}
