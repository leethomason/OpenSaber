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

#ifndef SABER_DB_INCLUDED
#define SABER_DB_INCLUDED

#include <stdint.h>
#include "pins.h"

class SaberDB
{
public:
	SaberDB();

	bool writeDefaults();
	bool readData();

  // Global
	void nextPalette();
  int paletteIndex() const      { return dataHeader.currentPalette; }
  void setPalette(int n);

  bool soundOn() const          { return dataHeader.soundOn != 0; }
  bool setSoundOn(bool on);

  uint8_t volume()              { return dataHeader.volume; }
  void setVolume(int v);

  uint8_t volume4() const; // an approximation
  // Turns sonud on/off, and/or sets volume.
  void setVolume4(int vol);

  float motion() const          { return dataHeader.motion; }
  void setMotion(float motion);
  float impact() const          { return dataHeader.impact; }
  void setImpact(float impact);

  // Palette
  const uint8_t* bladeColor() const   { return palette.bladeColor; }
  void setBladeColor(const uint8_t* color);

  const uint8_t* impactColor() const  { return palette.impactColor; }
  void setImpactColor(const uint8_t* color);  

  uint8_t soundFont() const { return palette.sound; }
  void setSoundFont(int v);

  void log(const char* log);  // 4 chars or less
  void dumpLog();

  static const int NUM_PALETTES = 8;

private:
  int scanForLogStart();
  void clearLog();

	static const int BASE_ADDR    = 20;
  static const int NUM_SOUND_FONTS = 4;
  static const int LOG_SIZE = 3;
  static const int MAX_LOG = 64;

	struct Palette {
	  uint8_t bladeColor[NCHANNELS];
	  uint8_t impactColor[NCHANNELS];
	  uint8_t sound;
	};

	struct DataHeader {
	  DataHeader() : 
	    currentPalette(0), 
	    soundOn(1), 
	    volume(160), 
	    motion(DEFAULT_G_FORCE_MOTION), 
	    impact(DEFAULT_G_FORCE_IMPACT) 
	  {}
	  
	  uint8_t currentPalette;
	  uint8_t soundOn;
    uint8_t volume;
    float   motion;
    float   impact;
	};

  const int headerAddr() const        { return BASE_ADDR; }
  const int paletteAddr(int i) const  { return BASE_ADDR + sizeof(DataHeader) + sizeof(Palette) * i; }
  const int logAddr() const           { return BASE_ADDR + sizeof(DataHeader) + sizeof(Palette) * NUM_PALETTES; }

  int         logOffset;
	DataHeader	dataHeader;
	Palette 		palette;
};


#endif // SABER_DB_INCLUDED
