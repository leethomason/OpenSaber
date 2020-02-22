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
#include "Grinliz_Arduino_Util.h"
#include "saberdb.h"
#include "sfx.h" // bug fix; generally don't want a dependency on sfx
//#include "vprom.h"    // would be nice to get this working.

using namespace osbr;

SaberDB::SaberDB()
{
    setPaletteFromDirHash(0);
}

void SaberDB::setPaletteFromDirHash(uint32_t h)
{
    switch (h)
    {
    case 0xfe1a3638: // bespin2, jaina, vader
    {
        Log.p("setPaletteFromDirHash=0xfe1a3638 Bespin2/Jaina/Vader").eol();
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

    default:
    {
        static const int BESPIN2 = 0;
        static const int ROGUE = 1;

        palette[0].set(0x00ff00, 0x00ffa0, BESPIN2); // green
        palette[1].set(0x0000ff, 0x00c8ff, BESPIN2); // blue
        palette[2].set(0x00ffff, 0x00a0ff, BESPIN2); // cyan
        palette[3].set(0xff0000, 0xa08000, ROGUE);   // red
        palette[4].set(0xff6000, 0x808000, ROGUE);   // orange
        palette[5].set(0x0044ff, 0x00ccff, ROGUE);   // blue-green
        palette[6].set(0x508080, 0x30a0a0, ROGUE);   // white
        palette[7].set(0x00ff44, 0x00ffaa, ROGUE);   // green-blue
    }
    break;
    }
}

void SaberDB::nextPalette()
{
    setPalette(dataHeader.currentPalette + 1);
}

void SaberDB::setPalette(int n)
{
#ifdef SABER_SOUND_ON
    // The SD card memory streaming and the use of the SD
    // file system lead to crashes. (Grr.) Stop the sound
    // here to see if it cleans up the problem.
    SFX::instance()->stopSound();
#endif

    dataHeader.currentPalette = abs(n) % NUM_PALETTES;
}

void SaberDB::getPalette(int i, Palette* pal)
{
    i = glClamp(i, 0, NUM_PALETTES - 1);
    *pal = palette[i];
}

void SaberDB::setVolume(int v) 
{
    dataHeader.volume = glClamp(v, 0, 256);
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
    if (!soundOn() || dataHeader.volume == 0)
    {
        return 0;
    }
    if (dataHeader.volume == VOLUME_4)
        return 4;
    if (dataHeader.volume >= VOLUME_3)
        return 3;
    if (dataHeader.volume >= VOLUME_2)
        return 2;
    return 1;
}

void SaberDB::setMotion(float v)
{
    v = constrain(v, 1.1, 10.0);
    dataHeader.motion = v;
}

void SaberDB::setImpact(float v)
{
    v = constrain(v, 1.1, 10.0);
    dataHeader.impact = v;
}

void SaberDB::setBladeColor(const RGB &color)
{
    palette[dataHeader.currentPalette].bladeColor = color;
}

void SaberDB::setImpactColor(const RGB &color)
{
    palette[dataHeader.currentPalette].impactColor = color;
}

void SaberDB::setSoundFont(int f)
{
    palette[dataHeader.currentPalette].soundFont = glClamp(f, 0, 3); // fixme: use constants
}
