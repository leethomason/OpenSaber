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
#include <SerialFlash.h>

#include "sfx.h"
#include "pins.h"
#include "AudioPlayer.h"
#include "tester.h"

#if SERIAL_DEBUG == 1
#define DEBUG_DEEP
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
    m_lastSFX = SFX_NONE;
    m_enabled = true;

    memset(m_location, 255, sizeof(SFXLocation)*NUM_SFX_TYPES);
}

bool SFX::init()
{
    Log.p("SFX::init()").eol();
    if (m_player)
        m_player->init();
    scanFonts();
    if (m_player)
        m_player->mute(true); // nothing is happening; connect shutdown pin.
    return true;
}

void SFX::filePath(CStr<25>* path, const char* dir, const char* file)
{
    path->clear();
#if SABER_SOUND_ON == SABER_SOUND_SD    
    *path = dir;
    path->append('/');
#endif    
    path->append(file);
}

void SFX::filePath(CStr<25>* path, int index)
{
    path->clear();
#if SABER_SOUND_ON == SABER_SOUND_SD    
    if (m_numFonts > 0 && m_currentFont >= 0) {
        *path = m_dirName[m_currentFont].c_str();
        path->append('/');
    }
#endif
    path->append(m_filename[index].c_str());
}

void SFX::scanFonts()
{
    m_numFonts = 0;
 
    //  Serial.println("scanFonts()");
#if (SABER_SOUND_ON == SABER_SOUND_SD)
    // Scan for a sound font with a limited, reasonable set of files.
    static const int N = 4;
    const char* NAMES[N] = { "HUM.WAV", "IDLE.WAV", "POWERON.WAV", "IGNITE.WAV" };

    File root = SD.open("/");
    while (true) {
        File entry =  root.openNextFile();
        if (!entry) {
            // no more files
            break;
        }
        if (entry.isDirectory()) {
            for (int i = 0; i < N; ++i) {
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
#elif (SABER_SOUND_ON == SABER_SOUND_FLASH)
    m_numFonts = 1;
    m_dirName[0] = "<flash>";
#endif

    combSort(m_dirName, m_numFonts);

    Log.p("Fonts:").eol();
    for (int i = 0; i < m_numFonts; ++i) {
        Log.p(i).p(": ").p(m_dirName[i].c_str()).eol();
    }
}

void SFX::scanFiles(uint8_t index)
{
    // First get the files,
    // then sort the files,
    // finally assign location info.
    memset(m_location, 255, sizeof(SFXLocation)*NUM_SFX_TYPES);
    m_numFilenames = 0;

    //  Serial.print("scanFiles "); Serial.println(index);
#if (SABER_SOUND_ON == SABER_SOUND_SD)
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
#else
    SerialFlash.opendir();
    char filename[64];
    uint32_t filesize;
    while (SerialFlash.readdir(filename, sizeof(filename), filesize)) {
        //Log.p("SF: ").p(filename).eol();
        int slot = calcSlot(filename);
        if (slot >= 0 && m_numFilenames < MAX_SFX_FILES) {
            m_filename[m_numFilenames++] = filename;
        }
    }
#endif
    // They often come in weird order, which is a bummer.
    // Simple sort seems fast enough.
    combSort(m_filename, m_numFilenames);

    for (int i = 0; i < m_numFilenames; ++i) {
        addFile(m_filename[i].c_str(), i);
    }

    static const char* NAMES[NUM_SFX_TYPES] = {
        "Idle        ",
        "Motion      ",
        "Spin        ",
        "Impact      ",
        "User_Tap    ",
        "User_Hold   ",
        "Power_On    ",
        "Power_Off   "
    };
    for(int i=0; i<NUM_SFX_TYPES; ++i) {
        const int id = SFX_IDLE + i;
        Log.p(NAMES[i]).p("start=").p(m_location[id].start).p(" count=").p(m_location[id].count).eol();

        /* really good debug code: move to command window??
        Log.p("  ");
        for(int j=0; j<m_location[ID[i]].count; ++j) {

            uint8_t nChannels = 0;
            uint32_t nSamplesPerSec = 0;
            uint32_t length = 0;

            int track = m_location[ID[i]].start + j;
            ASSERT(track >= 0);
            ASSERT(track < m_numFilenames);

            CStr<25> path;
            filePath(&path, track);

            readHeader(path.c_str(), &nChannels, &nSamplesPerSec, &length, false);
            Log.p(m_filename[track].c_str()).p(":").p(nChannels).p(":").p(nSamplesPerSec).p(":").p(length).p(" ");
            ASSERT(nChannels == 1);
            ASSERT(nSamplesPerSec == 44100);
        }
        Log.eol();
        */
    }
    readIgniteRetract();
}

int SFX::calcSlot(const char* name )
{
    int slot = -1;

    if (strstr(name, "POWERONF")) return -1;

    if      (istrStarts(name, "BLDON")   || istrStarts(name, "POWERON"))    slot = SFX_POWER_ON;
    else if (istrStarts(name, "BLDOFF")  || istrStarts(name, "POWEROFF"))   slot = SFX_POWER_OFF;
    else if (istrStarts(name, "IDLE")    || istrStarts(name, "HUM"))        slot = SFX_IDLE;
    else if (istrStarts(name, "IMPACT")  || istrStarts(name, "CLASH"))      slot = SFX_IMPACT;
    else if (istrStarts(name, "MOTION")  || istrStarts(name, "SWING"))      slot = SFX_MOTION;
    else if (istrStarts(name, "USRHOLD") || istrStarts(name, "LOCKUP"))     slot = SFX_USER_HOLD;
    else if (istrStarts(name, "USRTAP")  || istrStarts(name, "BLASTER"))    slot = SFX_USER_TAP;
    else if (istrStarts(name, "SPIN"))                                      slot = SFX_SPIN;

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

bool SFX::playSound(int sound, int mode, bool playIfOff)
{
    // Flush out tests before switching sounds:
    Tester::instance()->process();

    if (!m_enabled) {
        return false;
    }

    ASSERT(sound >= 0);
    ASSERT(sound < NUM_SFX_TYPES);
    ASSERT(m_player);

    if (!m_player) return false;

#if SERIAL_DEBUG == 1
#ifdef DEBUG_DEEP
    Log.p("SFX playSound() sound: ").p(sound).eol();
    Log.p("SFX m_bladeOn: ").p(m_bladeOn).eol();
    Log.p("SFX m_currentSound: ").p(m_currentSound).eol();
    Log.p("m_muted: ").p(m_muted).eol();
#endif
#endif

    if (!playIfOff) {
        if (!m_bladeOn && (sound != SFX_POWER_ON)) {
            return false ; // don't play sound with blade off
        }
    }

    if (!playIfOff && sound == SFX_POWER_ON) {
        if (m_bladeOn)
            return false;  // defensive error check.
        m_bladeOn = true;
    }
    else if (!playIfOff && sound == SFX_POWER_OFF) {
        if (!m_bladeOn)
            return false;  // defensive error check. BUT gets in the way of meditation playback.
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

        //Log.p("SFX play track ").p(m_filename[track].c_str()).eol();
        Log.event("[SFX play]", m_filename[track].c_str());

        CStr<25> path;
        if (m_numFonts > 0 && m_currentFont >= 0) {
            filePath(&path, m_dirName[m_currentFont].c_str(), m_filename[track].c_str());
        }
        else {
            path = m_filename[track].c_str();
        }
        m_player->mute(m_muted);
        m_player->play(path.c_str());
        m_currentSound = sound;
        m_lastSFX = sound;
        return true;
    }
    return false;
}

bool SFX::playSound(const char* sfx)
{
    if (!m_enabled) {
        return false;
    }
    m_player->mute(m_muted);
    m_player->play(sfx);
    return true;
}

void SFX::stopSound()
{
    if (!m_enabled) {
        return;
    }
    m_player->stop();
    m_currentSound = SFX_NONE;
}

void SFX::process()
{
    if (!m_player) return;
    if (!m_enabled) return;

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

uint32_t SFX::readU32(SerialFlashFile& file, int n) {
    uint32_t v = 0;
    for (int i = 0; i < n; ++i) {
        uint8_t b = 0;
        file.read(&b, 1);
        v += uint32_t(b) << (i * 8);
    }
    return v;
}

bool SFX::readHeader(const char* filename, uint8_t* nChannels, uint32_t* nSamplesPerSec, uint32_t* lengthMillis, bool logToConsole)
{
#if (SABER_SOUND_ON == SABER_SOUND_SD)
    File file = SD.open(filename);
#elif (SABER_SOUND_ON == SABER_SOUND_FLASH)
    SerialFlashFile file = SerialFlash.open(filename);
#else
    File file;
#endif    
    if (file) {
        Log.p(filename).eol();

        file.seek(22);
        *nChannels = readU32(file, 2);
        if (logToConsole) Log.p("channels:        ").p(*nChannels).eol();
        *nSamplesPerSec = readU32(file, 4);
        if (logToConsole) Log.p("nSamplesPerSec:  ").p(*nSamplesPerSec).eol();
        uint32_t nAvgBytesPerSec = readU32(file, 4);
        *lengthMillis = (file.size() - 44u) * 1000u / (nAvgBytesPerSec);

        if (logToConsole) Log.p("length millis:   ").p(*lengthMillis).eol();
        file.close();
        return true;
    }
    return false;
}

const uint32_t SFX::lengthMillis() const
{
    return m_player->lengthMillis();
}

void SFX::readIgniteRetract()
{
    uint8_t nChannels = 0;
    uint32_t samples = 0;
    CStr<25> path;

    if (m_location[SFX_POWER_ON].InUse()) {
        filePath(&path, m_location[SFX_POWER_ON].start);
        readHeader(path.c_str(), &nChannels, &samples, &m_igniteTime, true);
    }
    if (m_location[SFX_POWER_OFF].InUse())
        filePath(&path, m_location[SFX_POWER_OFF].start);
    readHeader(path.c_str(), &nChannels, &samples, &m_retractTime, true);
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
    Log.p("SFX::mute: ").p(muted ? "true" : "false").eol();

    m_muted = muted;
    if (m_player && m_muted) {
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
        if (m_player)
            m_player->setVolume(1.0f);
    }
    else {
        static const float INV = 0.0049;
        float v = float(vol) * INV;
        if (m_player)
            m_player->setVolume(v);
    }
}


uint8_t SFX::getVolume204() const
{
    if (m_player)
        return m_player->volume() * 204.0f + 0.5f;
    return 160;
}


uint8_t SFX::setFont(uint8_t font)
{
    //Serial.print("setFont "); Serial.println(font);
#if SABER_SOUND_ON == SABER_SOUND_FLASH
    font = 0;
#endif

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

uint8_t SFX::setFont(const char* name)
{
    int i=0;
#if SABER_SOUND_ON == SABER_SOUND_SD
    for(; i<m_numFonts; ++i) {
        if (strEqual(name, fontName(i))) {
            break;
        }
    }
#endif
    return setFont(i);
}

const char* SFX::currentFontName() const
{
    if (m_numFonts && m_currentFont >= 0) {
        return m_dirName[m_currentFont].c_str();
    }
    return "<none>";
}

const char* SFX::fontName(uint8_t font) const
{
    if (m_numFonts == 0) return "";
    return m_dirName[font % m_numFonts].c_str();
}

