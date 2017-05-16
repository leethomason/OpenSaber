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
#include <EEPROM.h>
#include "Grinliz_Arduino_Util.h"
#include "saberdb.h"
#include "sfx.h"  // bug fix; generally don't want a dependency on sfx


SaberDB::SaberDB() {
    STATIC_ASSERT(BASE_ADDR + NUM_PALETTES * sizeof(Palette) + sizeof(DataHeader) < EEPROM_SIZE);
}

bool SaberDB::writeDefaults()
{
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

    Log.p("Writing EEPROM default.").eol();

    DataHeader dataHeaderDefault;
    EEPROM.put(headerAddr(), dataHeaderDefault);

    for (int i = 0; i < NUM_PALETTES; ++i) {
        Palette p = defPalette[i];
#if SABER_SOUND_ON == SABER_SOUND_FLASH
        p.soundFont = "<flash>";
#else
        p.soundFont = defNames[i];
#endif
        EEPROM.put(paletteAddr(i), p);
    }
    readData();
    Log.p("EEPROM default complete.").eol();
    return true;
}


bool SaberDB::readData() {

    EEPROM.get(headerAddr(), dataHeader);
    EEPROM.get(paletteAddr(dataHeader.currentPalette), palette);
    /*
    Serial.print(F("Data header. currentPalette=")); Serial.print(dataHeader.currentPalette);
    Serial.print(F(" soundOn=")); Serial.println(dataHeader.soundOn);
    Serial.print(F("RGB ")); Serial.print(palette.bladeColor[0]); Serial.print(" "); Serial.print(palette.bladeColor[1]); Serial.print(" "); Serial.println(palette.bladeColor[2]);
    */
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
    Log.event("[PALETTE]", dataHeader.currentPalette);
    //Log.p("Switch Palette to: ").p(dataHeader.currentPalette).eol();

    EEPROM.put(headerAddr(), dataHeader);
    EEPROM.get(paletteAddr(dataHeader.currentPalette), palette);
}

void SaberDB::setupInit()
{
    dataHeader.nSetup = dataHeader.nSetup + 1;
    EEPROM.put(headerAddr(), dataHeader);
}

bool SaberDB::setSoundOn(bool on) {
    dataHeader.soundOn = on;
    Log.p("setSoundOn sound: ").p(dataHeader.soundOn).eol();

    EEPROM.put(headerAddr(), dataHeader);
    return true;
}

void SaberDB::setVolume(int v) {
    v = constrain(v, 0, 204);
    dataHeader.volume = v;
    EEPROM.put(headerAddr(), dataHeader);
}

void SaberDB::setVolume4(int v)
{
    switch (v) {
    case 0:
        setSoundOn(false);
        break;
    case 1:
        setVolume(VOLUME_1);
        setSoundOn(true);
        break;
    case 2:
        setVolume(VOLUME_2);
        setSoundOn(true);
        break;
    case 3:
        setVolume(VOLUME_3);
        setSoundOn(true);
        break;
    case 4:
        setVolume(VOLUME_4);
        setSoundOn(true);
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
    EEPROM.put(headerAddr(), dataHeader);
}

void SaberDB::setImpact(float v) {
    v = constrain(v, 1.1, 10.0);
    dataHeader.impact = v;
    EEPROM.put(headerAddr(), dataHeader);
}

void SaberDB::setBladeColor(const RGB& color) {
    palette.bladeColor = color;
    EEPROM.put(paletteAddr(dataHeader.currentPalette), palette);
}

void SaberDB::setImpactColor(const RGB& color) {
    palette.impactColor = color;
    EEPROM.put(paletteAddr(dataHeader.currentPalette), palette);
}

void SaberDB::setSoundFont(const char* v) {
    palette.soundFont = v;
    EEPROM.put(paletteAddr(dataHeader.currentPalette), palette);
}

