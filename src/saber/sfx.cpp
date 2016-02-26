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
  m_bladeOn = false;
  m_muted = false;
  m_numFonts = 0;
  m_numFilenames = 0;
  m_currentSound = SFX_NONE;
  m_currentFont = -1;
  m_igniteTime = 1000;
  m_retractTime = 1000;

  memset(m_location, 255, sizeof(SFXLocation)*NUM_SFX_TYPES);
}

bool SFX::init()
{
#if SERIAL_DEBUG == 1
  Serial.println(F("SFX::init()"));
#endif
  m_player->init();
  scanFonts();
  m_player->mute(true); // nothing is happening; connect shutdown pin.
  return true;
}

void SFX::filePath(CStr<25>* path, const char* dir, const char* file)
{
  path->clear();
  *path = dir;
  path->append('/');
  path->append(file);  
}

void SFX::filePath(CStr<25>* path, int index) 
{
  path->clear();
  if (m_numFonts > 0 && m_currentFont >= 0) {
    *path = m_dirName[m_currentFont].c_str();
    path->append('/');
  }
  path->append(m_filename[index].c_str());
}
  
void SFX::scanFonts()
{
  m_numFonts = 0;
  Serial.println("scanFonts()");
  File root = SD.open("/");
  while (true) {
    File entry =  root.openNextFile();
    if (!entry) {
      // no more files
      break;
    }
    if (entry.isDirectory()) {
      // Scan for a sound font with a limited, reasonable set of files.
      static const int N = 4;
      const char* NAMES[N] = { "HUM.WAV", "IDLE.WAV", "POWERON.WAV", "IGNITE.WAV" };
      for(int i=0; i<N; ++i) {
        CStr<25> path;
        filePath(&path, entry.name(), NAMES[i]);

        File file = SD.open(path.c_str());
        if (file) {
          m_dirName[m_numFonts++] = entry.name();
          file.close();
          break;
        }
      }
    }
    entry.close();
  }
  root.close();

  combSort(m_dirName, m_numFonts);
  Serial.println("Fonts:");
  for(int i=0; i<m_numFonts; ++i) {
    Serial.print(i); Serial.print(": "); Serial.println(m_dirName[i].c_str());
  }
  Serial.println("");
}

void SFX::scanFiles(uint8_t index)
{
  // First get the files,
  // then sort the files,
  // finally assign location info.
  memset(m_location, 255, sizeof(SFXLocation)*NUM_SFX_TYPES);
  m_numFilenames = 0;

  Serial.print("scanFiles "); Serial.println(index);
  File root = SD.open(m_dirName[index].c_str());
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

  // They often come in weird order, which is a bummer.
  // Simple sort seems fast enough.
  combSort(m_filename, m_numFilenames);

  for (int i = 0; i < m_numFilenames; ++i) {
    addFile(m_filename[i].c_str(), i);
  }

  Serial.print("IDLE      "); Serial.print(m_location[SFX_IDLE].start);      Serial.print(" "); Serial.println(m_location[SFX_IDLE].count);
  Serial.print("MOTION    "); Serial.print(m_location[SFX_MOTION].start);    Serial.print(" "); Serial.println(m_location[SFX_MOTION].count);
  Serial.print("IMPACT    "); Serial.print(m_location[SFX_IMPACT].start);    Serial.print(" "); Serial.println(m_location[SFX_IMPACT].count);
  Serial.print("USER_TAP  "); Serial.print(m_location[SFX_USER_TAP].start);  Serial.print(" "); Serial.println(m_location[SFX_USER_TAP].count);
  Serial.print("USER_HOLD "); Serial.print(m_location[SFX_USER_HOLD].start); Serial.print(" "); Serial.println(m_location[SFX_USER_HOLD].count);
  Serial.print("POWER_ON  "); Serial.print(m_location[SFX_POWER_ON].start);  Serial.print(" "); Serial.println(m_location[SFX_POWER_ON].count);
  Serial.print("POWER_OFF "); Serial.print(m_location[SFX_POWER_OFF].start); Serial.print(" "); Serial.println(m_location[SFX_POWER_OFF].count);

  readIgniteRetract();
}

int SFX::calcSlot(const char* name )
{
  int slot = -1;

  if (strstr(name, "POWERONF")) return -1;

  if      (strStarts(name, "BLDON")   || strStarts(name, "POWERON"))    slot = SFX_POWER_ON;
  else if (strStarts(name, "BLDOFF")  || strStarts(name, "POWEROFF"))   slot = SFX_POWER_OFF;
  else if (strStarts(name, "IDLE")    || strStarts(name, "HUM"))        slot = SFX_IDLE;
  else if (strStarts(name, "IMPACT")  || strStarts(name, "CLASH"))      slot = SFX_IMPACT;
  else if (strStarts(name, "MOTION")  || strStarts(name, "SWING"))      slot = SFX_MOTION;
  else if (strStarts(name, "USRHOLD") || strStarts(name, "LOCKUP"))     slot = SFX_USER_HOLD;
  else if (strStarts(name, "USRTAP")  || strStarts(name, "BLASTER"))    slot = SFX_USER_TAP;

  return slot;
}

void SFX::addFile(const char* name, int index)
{
  int slot = calcSlot(name);
  if (slot == -1) return;

  if (!m_location[slot].InUse()) {
    m_location[slot].start = index;
    m_location[slot].count = 1;
  }
  else {
    m_location[slot].count++;
  }
}

bool SFX::playSound(int sound, int mode)
{
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
    if (m_bladeOn) 
      return false;  // defensive error check.
    m_player->mute(m_muted);
    m_bladeOn = true;
  }
  else if (sound == SFX_POWER_OFF) {
    if (!m_bladeOn) 
      return false;  // defensive error check.
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
    Serial.print(F("SFX play track ")); Serial.print(m_filename[track].c_str()); Serial.print("... ");
#endif
    CStr<25> path;
    if (m_numFonts > 0 && m_currentFont >= 0) {
      filePath(&path, m_dirName[m_currentFont].c_str(), m_filename[track].c_str());
    }
    else {
      path = m_filename[track].c_str();
    }
    m_player->play(path.c_str());
    m_currentSound = sound;
    return true;
  }
  return false;
}

void SFX::process()
{
  // Play the idle sound if the blade is on.
  if (m_bladeOn && !m_player->isPlaying()) {
    playSound(SFX_IDLE, SFX_OVERRIDE);
  }
  if (!m_bladeOn && !m_player->isPlaying()) {
#ifdef SABER_SOUND_SHUTDOWN
    m_player->mute(true);
#endif
  }
}

uint32_t SFX::readU32(File& file, int n) {
  uint32_t v = 0;
  for (int i = 0; i < n; ++i) {
    int b = file.read();
    v += b << (i * 8);
  }
  return v;
}

bool SFX::readHeader(const char* filename, uint8_t* nChannels, uint32_t* nSamplesPerSec, uint32_t* lengthMillis)
{
  File file = SD.open(filename);
  if (file) {
    Serial.println(filename);
    file.seek(22);
    *nChannels = readU32(file, 2);
    Serial.print("channels:        "); Serial.println(*nChannels);
    *nSamplesPerSec = readU32(file, 4);
    Serial.print("nSamplesPerSec:  "); Serial.println(*nSamplesPerSec);
    uint32_t nAvgBytesPerSec = readU32(file, 4);
    Serial.print("nAvgBytesPerSec: "); Serial.println(nAvgBytesPerSec);
    Serial.print("nBlockAlign:     "); Serial.println(readU32(file, 2));
    Serial.print("wBitsPerSample:  "); Serial.println(readU32(file, 2));
    *lengthMillis = (file.size() - 44u) * 1000u / (nAvgBytesPerSec);
    Serial.print("length millis:   "); Serial.println(*lengthMillis);
    file.close();
    return true;
  }
  return false;
}


void SFX::readIgniteRetract()
{
  uint8_t nChannels = 0;
  uint32_t samples = 0;
  CStr<25> path;

  if (m_location[SFX_POWER_ON].InUse()) {
    filePath(&path, m_location[SFX_POWER_ON].start);
    readHeader(path.c_str(), &nChannels, &samples, &m_igniteTime);
  }
  if (m_location[SFX_POWER_OFF].InUse())
    filePath(&path, m_location[SFX_POWER_OFF].start);
    readHeader(path.c_str(), &nChannels, &samples, &m_retractTime);
}


void SFX::mute(bool muted)
{
  // this->m_muted is a little different from m_player->mute()
  // m_player->mute() controls the shutdown pin. We shutdown the
  //   amp for sound quality reasons and power draw when not in
  //   use.
  // this->m_muted is more the "traditional" use of mute. No
  //   sound is being output, but the audio system otherwise
  //   runs and responds normally.
  Serial.print("SFX::mute "); Serial.println(muted ? "true" : "false");
  m_muted = muted;
  if (m_muted) {
    m_player->mute(m_muted);
  }
}

bool SFX::isMuted() const
{
  return m_muted;
}

void SFX::setVolume204(int vol)
{
  vol = constrain(vol, 0, 204);
  if (vol >= 204) {
    m_player->setVolume(1.0f);
  }
  else {
    static const float INV = 0.0049;
    float v = float(vol) * INV;
    m_player->setVolume(v);
  }
}


uint8_t SFX::getVolume204() const
{
  return m_player->volume() * 204.0f + 0.5f;
}


uint8_t SFX::setFont(uint8_t font)
{
  Serial.print("setFont "); Serial.println(font);
  if (m_numFonts) {
    if (font != m_currentFont) {
      m_currentFont = font % m_numFonts;
      scanFiles(m_currentFont);
    }
  }
  else {
    m_currentFont = 0;
  }
  return m_currentFont;
}

const char* SFX::currentFontName() const
{
  if (m_numFonts && m_currentFont >= 0) {
    return m_dirName[m_currentFont].c_str();
  }
  return "<none>";
}

