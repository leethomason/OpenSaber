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

#include <SD.h>

#include "sfx.h"
#include "pins.h"
#include "AudioPlayer.h"

// 4: 204
// 3: 180
// 2: 170
// 1: 160

//#define DEBUG_DEEP

#if SERIAL_DEBUG == 1
# define ASSERT(x) if (!(x)) { Serial.print("SFX-ASSERT:"); Serial.print(#x); Serial.print(" "); Serial.print(__LINE__); }
#else
# define ASSERT(x)
#endif

SFX* SFX::m_instance = 0;

SFX::SFX(AudioPlayer* audioPlayer)
{
  m_instance = this;
  m_player = audioPlayer;
  m_numFilenames = 0;
  memset(m_location, 255, sizeof(SFXLocation)*NUM_SFX_TYPES);
}

bool SFX::init()
{
#if SERIAL_DEBUG == 1
  Serial.println(F("SFX::init()"));
#endif
  m_player->init();
  scanFiles();
  return true;
}

void SFX::scanFiles()
{
  // First get the files,
  // then sort the files,
  // finally assign location info.

  Serial.println("scanFiles()");
  File root = SD.open("/");
  while (true) {
    File entry =  root.openNextFile();
    if (!entry) {
      // no more files
      break;
    }
    if (entry.isDirectory()) {
      continue;
    }
    else {
      int slot = calcSlot(entry.name());
      if (slot >= 0 && m_numFilenames < MAX_SFX_FILES) {
        m_filename[m_numFilenames++] = entry.name();
      }
    }
    entry.close();
  }
  root.close();

  combSort(m_filename, m_numFilenames);

  for (int i = 0; i < m_numFilenames; ++i) {
    Serial.println(m_filename[i].c_str());
    addFile(m_filename[i].c_str(), i);
  }

  Serial.print("IDLE      "); Serial.print(m_location[SFX_IDLE].start);      Serial.print(" "); Serial.println(m_location[SFX_IDLE].count);
  Serial.print("MOTION    "); Serial.print(m_location[SFX_MOTION].start);    Serial.print(" "); Serial.println(m_location[SFX_MOTION].count);
  Serial.print("IMPACT    "); Serial.print(m_location[SFX_IMPACT].start);    Serial.print(" "); Serial.println(m_location[SFX_IMPACT].count);
  Serial.print("USER_TAP  "); Serial.print(m_location[SFX_USER_TAP].start);  Serial.print(" "); Serial.println(m_location[SFX_USER_TAP].count);
  Serial.print("USER_HOLD "); Serial.print(m_location[SFX_USER_HOLD].start); Serial.print(" "); Serial.println(m_location[SFX_USER_HOLD].count);
  Serial.print("POWER_ON  "); Serial.print(m_location[SFX_POWER_ON].start);  Serial.print(" "); Serial.println(m_location[SFX_POWER_ON].count);
  Serial.print("POWER_OFF "); Serial.print(m_location[SFX_POWER_OFF].start); Serial.print(" "); Serial.println(m_location[SFX_POWER_OFF].count);
}

int SFX::calcSlot(const char* name )
{
  int slot = -1;

  if (strstr(name, "POWERONF")) return -1;

  if      (strstr(name, "BLDON")   || strstr(name, "POWERON"))    slot = SFX_POWER_ON;
  else if (strstr(name, "BLDOFF")  || strstr(name, "POWEROFF"))   slot = SFX_POWER_OFF;
  else if (strstr(name, "IDLE")    || strstr(name, "HUM"))        slot = SFX_IDLE;
  else if (strstr(name, "IMPACT")  || strstr(name, "CLASH"))      slot = SFX_IMPACT;
  else if (strstr(name, "MOTION")  || strstr(name, "SWING"))      slot = SFX_MOTION;
  else if (strstr(name, "USRHOLD") || strstr(name, "LOCKUP"))     slot = SFX_USER_HOLD;
  else if (strstr(name, "USRTAP")  || strstr(name, "BLASTER"))    slot = SFX_USER_TAP;

  return slot;
}

void SFX::addFile(const char* name, int index)
{
  int slot = calcSlot(name);
  if (slot == -1) return;

  if (m_location[slot].start == 255) {
    m_location[slot].start = index;
    m_location[slot].count = 1;
  }
  else {
    m_location[slot].count++;
  }
}

bool SFX::playSound(int sound, int mode) {
  ASSERT(sound >= 0);
  ASSERT(sound < NUM_SFX_TYPES);
#if SERIAL_DEBUG == 1
#ifdef DEBUG_DEEP
  Serial.print(F("SFX playSound() sound:")); Serial.println(sound);
  Serial.print(F("SFX bladeOn:")); Serial.println(bladeOn);
  Serial.print(F("SFX currentSound:")); Serial.println(currentSound);
#endif
#endif

  if (!m_bladeOn && (sound != SFX_POWER_ON)) {
    return false ; // don't play sound with blade off
  }

  if (sound == SFX_POWER_ON) {
    if (m_bladeOn) return false;  // defensive error check.
    m_bladeOn = true;
  }
  else if (sound == SFX_POWER_OFF) {
    if (!m_bladeOn) return false;  // defensive error check.
    m_bladeOn = false;
  }

  if (!m_player->isPlaying()) {
    m_currentSound = SFX_NONE;
  }

  if (   m_currentSound == SFX_NONE
         || (mode == SFX_OVERRIDE)
         || (mode == SFX_GREATER && sound > m_currentSound)
         || (mode == SFX_GREATER_OR_EQUAL && sound >= m_currentSound))
  {
    int track = m_location[sound].start + random(m_location[sound].count);
    ASSERT(track >= 0);
    ASSERT(track < m_numFilenames);

#if SERIAL_DEBUG == 1
    Serial.print(F("SFX play track ")); Serial.print(m_filename[track].c_str());
#endif
    m_player->play(m_filename[track].c_str());
    m_currentSound = sound;
    return true;
  }
  return false;
}

void SFX::process() {
  // Play the idle sound if the blade is on.
  if (m_bladeOn && !m_player->isPlaying()) {
    playSound(SFX_IDLE, SFX_OVERRIDE);
  }
}


