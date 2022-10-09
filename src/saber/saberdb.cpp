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
#include "src/wav12util/manifest.h"

using namespace osbr;

SaberDB::SaberDB()
{
    //initializeFromDirHash(0);
}

void SaberDB::initFromManifest(const Manifest& manifest)
{
    int dir = -1;
    for (int i = 0; i < 4; ++i) {
        if (strEqual(manifest.getUnit(i).name, "config")) {
            dir = i;
            break;
        }
    }
    if (dir < 0) {
        Log.p("Error finding palette configuration.").eol();
        return;
    }
    int start = manifest.getUnit(dir).offset;
    Log.p("Palette start=").p(start).eol();
    for (int i = 0; i < NUM_PALETTES; ++i) {
        const ConfigUnit &config = manifest.getConfig(start + i);
        osbr::RGB blade = {config.bc_r, config.bc_g, config.bc_b};
        osbr::RGB impact = {config.ic_r, config.ic_g, config.ic_b};
        Log.p("Palette: ")
            .p(i).p(" font=").p(config.soundFont)
            .p(" bc=").v3(blade.r, blade.g, blade.b)
            .p(" ic=").v3(impact.r, impact.g, impact.b)
            .eol();
        palette[i].set(blade, impact, config.soundFont);
    }
}

void SaberDB::setPalette(int n)
{
    m_currentPalette = abs(n) % NUM_PALETTES;
}

const SaberDB::Palette* SaberDB::getPalette(int i) const
{
    i = glClamp(i, 0, NUM_PALETTES - 1);
    return &palette[i];
}

int SaberDB::toVolume256(int volume4)
{
    if (volume4 >= 4) return VOLUME_4;
    if (volume4 <= 0) return 0;

    if (volume4 == 3)
        return VOLUME_3;
    else if (volume4 == 2)
        return VOLUME_2;
    return VOLUME_1;
}

int SaberDB::toVolume4(int volume256)
{
    if (volume256 >= VOLUME_4) {
        return 4;
    }
    if (volume256 >= VOLUME_3) {
        return 3;
    }
    if (volume256 >= VOLUME_2) {
        return 2;
    }
    if (volume256 >= VOLUME_1) {
        return 1;
    }
    return 0;
}

void SaberDB::setSoundFont(int f)
{
    palette[m_currentPalette].soundFont = glClamp(f, 0, 3); // fixme: use constants
}
