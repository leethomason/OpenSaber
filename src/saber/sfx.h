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

#ifndef SFX_HEADER
#define SFX_HEADER

#include <Arduino.h>
#include <SD.h>

#include "Grinliz_Arduino_Util.h"

// SFX in priority order!
enum {            //  Max
  SFX_IDLE,       //  1
  SFX_MOTION,     //  16
  SFX_SPIN,       //  4  although not currently used
  SFX_IMPACT,     //  16
  SFX_USER_TAP,   //  4
  SFX_USER_HOLD,  //  1
  SFX_POWER_ON,   //  4
  SFX_POWER_OFF,  //  4

  NUM_SFX_TYPES,
  MAX_SFX_FILES = 64,
  MAX_FONTS = 10,
  SFX_NONE = 255
};

enum {
  SFX_GREATER,
  SFX_GREATER_OR_EQUAL,
  SFX_OVERRIDE
};

class AudioPlayer;
class SerialFlashFile;

class SFX
{
public:
  // AudioPlayer can be null to avoid a bunch of #if/if
  SFX(AudioPlayer* audioPlayer);
  static SFX* instance() { return m_instance; }

  bool init();
  void scanFiles(uint8_t font);

  bool playSound(int sfx, int mode, bool playIfOff=false);
  bool playSound(const char* sfx);
  bool playUISound(const char* name);
  bool playUISound(int n);
  void stopSound();

  // The class of SFX (MOTION, IMPACT, etc) last successfully played.
  int lastSFX() const                     { return m_lastSFX; }
  
  bool bladeOn() const                    { return m_bladeOn; }
  void process();

  const uint32_t getIgniteTime() const    { return m_igniteTime; }
  const uint32_t getRetractTime() const   { return m_retractTime; }
  // Get the length of the currently playing file. Will
  // return 0 if the file header isn't read yet.
  const uint32_t lengthMillis() const;

  void setVolume204(int vol);
  uint8_t getVolume204() const;

  uint8_t setFont(uint8_t font);
  uint8_t setFont(const char* fontName);
  const char* fontName(uint8_t font) const;
  int numFonts() const { return m_numFonts; }
  const char* currentFontName() const;

  bool readHeader(const char* filename, uint8_t* nChannels, uint32_t* nSamplesPerSec, uint32_t* lengthMillis, bool logToConsole);

  // testing
  uint16_t nEnabled() const;
  uint16_t nDisabled() const;


private:
  void filePath(CStr<25>* str, int id);
  void filePath(CStr<25>* str, const char* dir, const char* file);
  void scanFonts();

  void addFile(const char* filename, int index);
  int calcSlot(const char* filename); // -1 if not a supported file
  void readIgniteRetract();
  uint32_t readU32(File& file, int n);
  uint32_t readU32(SerialFlashFile& file, int n);

  // note: initialize to 255
  struct SFXLocation {
    uint8_t start;
    uint8_t count;
  
    const bool InUse() const { return start < 255 && count < 255; }
  };

  AudioPlayer* m_player;
  bool         m_bladeOn;
  uint8_t      m_numFonts;
  int8_t       m_numFilenames;
  int8_t       m_currentSound;
  int8_t       m_lastSFX;
  uint8_t      m_currentFont;
  uint32_t     m_igniteTime;
  uint32_t     m_retractTime;
  float        m_savedVolume = -1.f;  // negative means not in use.

  SFXLocation  m_location[NUM_SFX_TYPES];
  CStr<13>     m_filename[MAX_SFX_FILES];
  CStr<9>      m_dirName[MAX_FONTS];
  
  static SFX*  m_instance;
};

#endif // SFX_HEADER
