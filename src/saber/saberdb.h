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
#include <Grinliz_Arduino_Util.h>
#include <DotStar.h>
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
  uint32_t numSetupCalls() const { return dataHeader.nSetup; }

  // Palette
  const RGB& bladeColor() const   { return palette.bladeColor; }
  void setBladeColor(const RGB& color);

  const RGB& impactColor() const  { return palette.impactColor; }
  void setImpactColor(const RGB& color);  

  const char* soundFont() const { return palette.soundFont.c_str(); }
  void setSoundFont(const char*);

  static const int NUM_PALETTES = 8;

  // Testing (not saved)
  void setCrystalColor(const RGB& color) { m_crystalColor = color; }
  const RGB& crystalColor() const { return m_crystalColor; }

private:
  void setupInit();

	static const int BASE_ADDR    = 20;

	struct Palette {
	  RGB bladeColor;
	  RGB impactColor;
    CStr<9> soundFont;
	};

	struct DataHeader {
	  DataHeader() : 
	    currentPalette(0), 
	    soundOn(1), 
	    volume(160), 
	    motion(DEFAULT_G_FORCE_MOTION), 
	    impact(DEFAULT_G_FORCE_IMPACT),
      nSetup(0)
	  {}
	  
	  uint8_t currentPalette;
	  uint8_t soundOn;
    uint8_t volume;
    float   motion;
    float   impact;
    uint32_t nSetup;
	};

  const int headerAddr() const        { return BASE_ADDR; }
  const int paletteAddr(int i) const  { return BASE_ADDR + sizeof(DataHeader) + sizeof(Palette) * i; }

  RGB     m_crystalColor;
	DataHeader	dataHeader;
	Palette 		palette;
};


#endif // SABER_DB_INCLUDED
