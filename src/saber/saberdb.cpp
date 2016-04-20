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
#include <Grinliz_Arduino_Util.h>
#include <EEPROM.h>
#include "saberdb.h"


SaberDB::SaberDB() {
  STATIC_ASSERT(BASE_ADDR + NUM_PALETTES * sizeof(Palette) + sizeof(DataHeader) < EEPROM_SIZE);
  STATIC_ASSERT(BASE_ADDR + NUM_PALETTES * sizeof(Palette) + sizeof(DataHeader) + MAX_LOG * LOG_SIZE < EEPROM_SIZE);
  logOffset = -1;
}

bool SaberDB::writeDefaults() {
  Palette defPalette[NUM_PALETTES] = {
    { 0, 0xff, 0,     0, 0xff, 0xa0,    0 },    // green
    { 0, 0, 0xff,     0, 0xc8, 0xff,    0 },    // blue
    { 0, 0xff, 0xff,  0, 0xa0, 0xff,    0 },    // cyan
    { 0xff, 0, 0,     0xa0, 0x80, 0,    0 },    // red
    { 0xff, 0x6a, 0,  0x80, 0xff, 0,    0 },    // amber
    { 0xff, 0xc8, 0,  0xff, 0xff, 0,    0 },    // sand
    { 0x90, 0, 0xff,  0x90, 0x64, 0xff, 0 },    // purple
    { 0xff, 0x40, 0xed, 0x50, 0,  0xff,  0 }    // pink
  };

#if SERIAL_DEBUG == 1
  Serial.println(F("Writing EEPROM default."));
#endif

  DataHeader dataHeaderDefault;
  EEPROM.put(headerAddr(), dataHeaderDefault);

  for (int i = 0; i < NUM_PALETTES; ++i) {
    EEPROM.put(paletteAddr(i), defPalette[i]);
  }
  clearLog();
  readData();
#if SERIAL_DEBUG == 1
  Serial.println(F("EEPROM default complete."));
#endif
  return true;
}


bool SaberDB::readData() {

  EEPROM.get(headerAddr(), dataHeader);
  EEPROM.get(paletteAddr(dataHeader.currentPalette), palette);

#if SERIAL_DEBUG == 1
  Serial.print(F("Data header. currentPalette=")); Serial.print(dataHeader.currentPalette);
  Serial.print(F(" soundOn=")); Serial.println(dataHeader.soundOn);
  Serial.print(F("RGB ")); Serial.print(palette.bladeColor[0]); Serial.print(" "); Serial.print(palette.bladeColor[1]); Serial.print(" "); Serial.println(palette.bladeColor[2]);
#endif
/*
#if SERIAL_DEBUG == 1
  Serial.print( "nlog ");
  int maxLog = (EEPROM_SIZE - (BASE_ADDR + NUM_PALETTES * sizeof(Palette) + sizeof(DataHeader))) / LOG_SIZE;
  Serial.println( maxLog);
#endif
*/
  return true;
}


void SaberDB::nextPalette() {
  setPalette(dataHeader.currentPalette + 1);
}

void SaberDB::setPalette(int n) {
  dataHeader.currentPalette = abs(n) % NUM_PALETTES;
#if SERIAL_DEBUG == 1
  Serial.print(F("Switch Palette to: ")); Serial.println(dataHeader.currentPalette);
#endif

  EEPROM.put(headerAddr(), dataHeader);
  EEPROM.get(paletteAddr(dataHeader.currentPalette), palette);
}

bool SaberDB::setSoundOn(bool on) {
  dataHeader.soundOn = on;
#if SERIAL_DEBUG == 1
  Serial.print(F("setSoundOn sound: ")); Serial.println(dataHeader.soundOn);
#endif

  EEPROM.put(headerAddr(), dataHeader);
  return true;
}

void SaberDB::setVolume(int v) {
  v = constrain(v, 0, 204);
  dataHeader.volume = v;
  EEPROM.put(headerAddr(), dataHeader);
}

static const int VOLUME_1 = 30;
static const int VOLUME_2 = 80;
static const int VOLUME_3 = 120;
static const int VOLUME_4 = 204;

void SaberDB::setVolume4(int v) 
{
  switch (v) {
    case 0: setSoundOn(false); break;
    case 1: setVolume(VOLUME_1); setSoundOn(true); break;
    case 2: setVolume(VOLUME_2); setSoundOn(true); break;
    case 3: setVolume(VOLUME_3); setSoundOn(true); break;
    case 4: setVolume(VOLUME_4); setSoundOn(true); break;
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

void SaberDB::setBladeColor(const uint8_t* color) {
  for (int i = 0; i < NCHANNELS; ++i) {
    palette.bladeColor[i] = color[i];
  }
  EEPROM.put(paletteAddr(dataHeader.currentPalette), palette);
}

void SaberDB::setImpactColor(const uint8_t* color) {
  for (int i = 0; i < NCHANNELS; ++i) {
    palette.impactColor[i] = color[i];
  }
  EEPROM.put(paletteAddr(dataHeader.currentPalette), palette);
}

void SaberDB::setSoundFont(const char* v) {
  strBufCpy(palette.soundFont, 9, v ? v : "");
  EEPROM.put(paletteAddr(dataHeader.currentPalette), palette);
}

void SaberDB::log(const char* v) {
#ifdef SABER_LOGGING
  if (logOffset < 0) {
    logOffset = scanForLogStart();
  }
  if (logOffset == MAX_LOG) {
    clearLog();
    logOffset = 0;
  }
  int addr = logAddr() + LOG_SIZE * logOffset;

  for (int i = 0; i < LOG_SIZE && v[i]; ++i) {
    EEPROM.put(addr + i, v[i]);
  }
  logOffset++;
#endif
}

int SaberDB::scanForLogStart() {
  for(int i=0; i<MAX_LOG; ++i) {
    int addr = logAddr() + i * LOG_SIZE;
    char c = 0;
    EEPROM.get(addr, c);
    if (c == 0) {
      return i;   
    }
  }
  return MAX_LOG;
}

 
void SaberDB::clearLog() {
  for(int i=0; i<MAX_LOG; ++i) {
    int addr = logAddr() + i * LOG_SIZE;
    EEPROM.put(addr, char(0));
  }
}

void SaberDB::dumpLog() {
#ifdef SABER_LOGGING
  int lineBreak = 0;
  for (int i = 0; i < MAX_LOG; ++i) {
    char msg[LOG_SIZE+1] = {0};
    int addr = logAddr() + i * LOG_SIZE;
    for(int k=0; k<LOG_SIZE; ++k) {
      EEPROM.get(addr + k, msg[k]);      
    }
    if (msg[0] == 0) break;
    Serial.print(msg); Serial.print(" "); 
    lineBreak++;
    if (lineBreak == 8) {
      Serial.println(" ");
      lineBreak = 0;
    }
  }
  Serial.println(" ");
#endif
}


