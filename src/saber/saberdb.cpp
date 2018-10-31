/*
Copyright (c) 2016 Lee Thomason, Grinning Lizard Software

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
#include "sfx.h"  // bug fix; generally don't want a dependency on sfx
#include "vprom.h"

using namespace osbr;

SaberDB::SaberDB() {
    STATIC_ASSERT(BASE_ADDR + NUM_PALETTES * sizeof(Palette) + sizeof(DataHeader) < EEPROM_SIZE);
}

bool SaberDB::writeDefaults()
{
    #if (defined(LED_TYPE) && (LED_TYPE == LED_TYPE_BBG))
        static Palette defPalette[NUM_PALETTES] = {
                                              // assume 400ma/channel 
            { 0x00ff00,  0x00ffa0,    0 },    // green         400         
            { 0x0000ff,  0x00c8ff,    0 },    // blue          400
            { 0x00ffff,  0x00a0ff,    0 },    // cyan          800
            { 0x00ff44,  0x00ffaa,    0 },    // off-green     500

            { 0xffff00,  0x44ffa0,    0 },    // green-green   800
            { 0xa0a000,  0x44ffa0,    0 },    // green-green   500
            { 0x8080ff,  0x00a0ff,    0 },    // cyan          800
            { 0xffff44,  0xaaffaa,    0 },    // off-green     900
        };

        static const char* defNames[NUM_PALETTES] = {
            "BESPIN2",
            "OBIWAN",
            "FATES",
            "JAINA",
            "BESPIN2",
            "BESPIN2",
            "JAINA",
            "JAINA",
        };
    #elif (defined(LED_TYPE) && (LED_TYPE == LED_TYPE_WWG))
        static Palette defPalette[NUM_PALETTES] = {
            { 0x440044,  0x444400,    0 }, 
            { 0x880088,  0x888800,    0 }, 
            { 0xbb00bb,  0xbbbb00,    0 }, 
            { 0xff00ff,  0xffff00,    0 }, 

            { 0x004400,  0x440044,    0 }, 
            { 0x008800,  0x880088,    0 }, 
            { 0x00bb00,  0xbb00bb,    0 }, 
            { 0x00ff00,  0xff00ff,    0 }, 
        };

        static const char* defNames[NUM_PALETTES] = {
            "JAINA",
            "JAINA",
            "JAINA",
            "JAINA",
            "JAINA",
            "JAINA",
            "JAINA",
            "JAINA",
        };        
    #elif (defined(LED_TYPE) && (LED_TYPE == LED_TYPE_GGC))
        static Palette defPalette[NUM_PALETTES] = {
            { 0x444400,  0x440044,    0 }, 
            { 0x888800,  0x880088,    0 }, 
            { 0xbbbb00,  0xbb00bb,    0 }, 
            { 0xffff00,  0xff00ff,    0 }, 

            { 0x440044,  0x444400,    0 }, 
            { 0x880088,  0x888800,    0 }, 
            { 0xbb00bb,  0xbbbb00,    0 }, 
            { 0x00ff00,  0xffff00,    0 }, 
        };

        static const char* defNames[NUM_PALETTES] = {
            "BESPIN2",
            "BESPIN2",
            "BESPIN2",
            "BESPIN2",
            "BESPIN2",
            "BESPIN2",
            "BESPIN2",
            "BESPIN2",
        };        
    #elif (SABER_MODEL == SABER_MODEL_SISTER)
        #if SABER_SUB_MODEL == SABER_SUB_MODEL_CELESTIA
            static Palette defPalette[NUM_PALETTES] = {
                { 0x00ff00,  0x00ffa0,    0 },    // green-green
                { 0x0000ff,  0x00c8ff,    0 },    // blue-blue
                { 0x00ffff,  0x00a0ff,    0 },    // cyan-cyan
                { 0xff0000,  0xa08000,    0 },    // red-red
                { 0x00ff00,  0x00c8ff,    0 },    // green-blue
                { 0x00ff00,  0x00ccff,    0 },    // green-cyan
                { 0x9000ff,  0x9064ff,    0 },    // purple-purple
                { 0xffff00,  0x44ff00,    0 }     // yellow-yellow
            };

        #elif SABER_SUB_MODEL == SABER_SUB_MODEL_LUNA
            static Palette defPalette[NUM_PALETTES] = {
                { 0x00ff00,  0x00ffa0,    0 },    // green-green
                { 0x0000ff,  0x00c8ff,    0 },    // blue-blue
                { 0x00ffff,  0x00a0ff,    0 },    // cyan-cyan
                { 0xff0000,  0xa08000,    0 },    // red-red
                { 0x0000ff,  0x00c8ff,    0 },    // green-blue
                { 0x00ffff,  0x00ccff,    0 },    // green-cyan
                { 0x9000ff,  0x9064ff,    0 },    // purple-purple
                { 0xffff00,  0x44ff00,    0 },    // yellow-yellow
            };
        #else 
            #error SUB_MODEL not defined.
        #endif
        static const char* defNames[NUM_PALETTES] = {
            "BESPIN2",
            "JAINA",
            "FATES",
            "REVENGE",
            "FATES",
            "JAINA",
            "JAINA",
            "ROGUE"
        };        
    #else
        // STANDARD
        static Palette defPalette[NUM_PALETTES] = {
            { 0x00ff00,  0x00ffa0,    0 },    // green
            { 0x0000ff,  0x00c8ff,    0 },    // blue
            { 0x00ffff,  0x00a0ff,    0 },    // cyan
            { 0xff0000,  0xa08000,    0 },    // red
            { 0xff4000,  0x80ff00,    0 },
            { 0x0044ff,  0x00ccff,    0 },
            { 0x9000ff,  0x9064ff,    0 },
            { 0x00ff44,  0x00ffaa,    0 }
        };

        #ifdef CORE_TEENSY
        static const char* defNames[NUM_PALETTES] = {
            "BESPIN2",
            "OBIWAN",
            "FATES",
            "VADER",
            "ROGUE",
            "GRAFLEX",
            "JAINA",
            "ROGUE"
        };
        #else
        static const char* defNames[NUM_PALETTES] = {
            "bespin2",
            "bespin2",
            "bespin2",
            "jaina",
            "jaina",
            "jaina",
            "jaina",
            "jaina"
        };
        #endif
    #endif

    Log.p("Writing EEPROM default.").eol();

    DataHeader dataHeaderDefault;
    vpromPut(headerAddr(), dataHeaderDefault);

    for (int i = 0; i < NUM_PALETTES; ++i) {
        Palette p = defPalette[i];
        p.soundFont = defNames[i];
        vpromPut(paletteAddr(i), p);
    }
    readData();
    Log.p("EEPROM default complete.").eol();
    return true;
}


bool SaberDB::readData() {

    vpromGet(headerAddr(), dataHeader);
    vpromGet(paletteAddr(dataHeader.currentPalette), palette);
    setupInit();
    return true;
}


void SaberDB::nextPalette() {
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
    EventQ.event("[PALETTE]", dataHeader.currentPalette);
    //Log.p("Switch Palette to: ").p(dataHeader.currentPalette).eol();

    vpromPut(headerAddr(), dataHeader);
    vpromGet(paletteAddr(dataHeader.currentPalette), palette);
}


void SaberDB::getPalette(int i, Palette* pal)
{
    vpromGet(paletteAddr(i), *pal);
}


void SaberDB::setupInit()
{
    dataHeader.nSetup = dataHeader.nSetup + 1;
    vpromPut(headerAddr(), dataHeader);
}

void SaberDB::setVolume(int v) {
    v = constrain(v, 0, 204);
    dataHeader.volume = v;
    vpromPut(headerAddr(), dataHeader);
}

void SaberDB::setVolume4(int v)
{
    switch (v) {
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
    if (!soundOn() || dataHeader.volume == 0) {
        return 0;
    }
    if (dataHeader.volume == VOLUME_4) return 4;
    if (dataHeader.volume >= VOLUME_3) return 3;
    if (dataHeader.volume >= VOLUME_2) return 2;
    return 1;
}

void SaberDB::setMotion(float v) {
    v = constrain(v, 1.1, 10.0);
    dataHeader.motion = v;
    vpromPut(headerAddr(), dataHeader);
}

void SaberDB::setImpact(float v) {
    v = constrain(v, 1.1, 10.0);
    dataHeader.impact = v;
    vpromPut(headerAddr(), dataHeader);
}

void SaberDB::setBladeColor(const RGB& color) {
    palette.bladeColor = color;
    vpromPut(paletteAddr(dataHeader.currentPalette), palette);
}

void SaberDB::setImpactColor(const RGB& color) {
    palette.impactColor = color;
    vpromPut(paletteAddr(dataHeader.currentPalette), palette);
}

void SaberDB::setSoundFont(const char* v) {
    palette.soundFont = v;
    vpromPut(paletteAddr(dataHeader.currentPalette), palette);
}

