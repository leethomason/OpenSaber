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

#include "pins.h"

#if SABER_SOUND_ON == SABER_SOUND_SD
#include <SD.h>
#include <SerialFlash.h>
#elif SABER_SOUND_ON == SABER_SOUND_FLASH
#include "mcmemimage.h"
#endif

#include "sfx.h"
#include "tester.h"

#if SERIAL_DEBUG == 1
//#define DEBUG_DEEP
#endif

SFX* SFX::m_instance = 0;

SFX::SFX(IAudio* audioPlayer)
{
    m_instance = this;

    m_player = audioPlayer;
    m_bladeOn = false;
    m_numFonts = 0;
    m_numFilenames = 0;
    m_currentSound = SFX_NONE;
    m_currentFont = -1;
    m_igniteTime = 1000;
    m_retractTime = 1000;
    m_lastSFX = SFX_NONE;

    memset(m_location, 255, sizeof(SFXLocation)*NUM_SFX_TYPES);
}

bool SFX::init()
{
    Log.p("SFX::init()").eol();
    scanFonts();
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
    m_numFonts = MemImage.numDir();
    for(int i=0; i<m_numFonts; ++i) {
        MemUnit dir;
        MemImage.readDir(i, &dir);
        m_dirName[i] = dir.name;
    }
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
#elif SABER_SOUND_ON == SABER_SOUND_FLASH
    MemUnit dir;
    MemImage.readDir(m_dirName[index].c_str(), &dir);
    for(unsigned i=0; i<dir.size; i++) {
        MemUnit file;
        MemImage.readFile(i + dir.offset, &file);
        CStr<13> name;
        name = file.name;   // explicity call operator=

        int slot = calcSlot(name.c_str());
        if (slot >= 0 && m_numFilenames < MAX_SFX_FILES) {
            m_filename[m_numFilenames++] = name;
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
        //"Spin        ",
        "Impact      ",
        "User_Tap    ",
        //"User_Hold   ",
        "Power_On    ",
        "Power_Off   "
    };
    for(int i=0; i<NUM_SFX_TYPES; ++i) {
        const int id = SFX_IDLE + i;
        Log.p(NAMES[i]).p("start=").p(m_location[id].start).p(" count=").p(m_location[id].count).eol();

        #if 0
        Log.p("  ");
        for(int j=0; j<m_location[id].count; ++j) {
            uint32_t length = 0;

            int track = m_location[id].start + j;
            ASSERT(track >= 0);
            ASSERT(track < m_numFilenames);

            CStr<25> path;
            filePath(&path, track);

            readHeader(path.c_str(), &length);
            Log.p(m_filename[track].c_str()).p(":").p(length).p(" ");
        }
        Log.eol();
        #endif
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
    //else if (istrStarts(name, "USRHOLD") || istrStarts(name, "LOCKUP"))     slot = SFX_USER_HOLD;
    else if (istrStarts(name, "USRTAP")  || istrStarts(name, "BLASTER"))    slot = SFX_USER_TAP;
    //else if (istrStarts(name, "SPIN"))                                      slot = SFX_SPIN;

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
    if (!m_player) return false;

    ASSERT(sound >= 0);
    ASSERT(sound < NUM_SFX_TYPES);
    ASSERT(m_player);

#if SERIAL_DEBUG == 1
#ifdef DEBUG_DEEP
    Log.p("SFX playSound() sound: ").p(sound).eol();
    Log.p("SFX m_bladeOn: ").p(m_bladeOn).eol();
    Log.p("SFX m_currentSound: ").p(m_currentSound).eol();
#endif
#endif

    if (!m_bladeOn && (sound != SFX_POWER_ON)) {
        return false ; // don't play sound with blade off
    }

    if (sound == SFX_POWER_ON) {
        if (m_bladeOn)
            return false;  // defensive error check.
        m_bladeOn = true;
    }
    else if (sound == SFX_POWER_OFF) {
        if (!m_bladeOn)
            return false;  // defensive error check. BUT gets in the way of meditation playback.
        m_bladeOn = false;
    }

    if (!m_player->isPlaying(0)) {
        m_currentSound = SFX_NONE;
    }

    if (   m_currentSound == SFX_NONE
            || (mode == SFX_OVERRIDE)
            || (mode == SFX_GREATER && sound > m_currentSound)
            || (mode == SFX_GREATER_OR_EQUAL && sound >= m_currentSound))
    {
        int track = m_location[sound].start + m_random.rand(m_location[sound].count);
        ASSERT(track >= 0);
        ASSERT(track < m_numFilenames);

        //Log.p("SFX play track ").p(m_filename[track].c_str()).eol();
        EventQ.event("[SFX play]", sound);

        CStr<25> path;
        if (m_numFonts > 0 && m_currentFont >= 0) {
            filePath(&path, m_dirName[m_currentFont].c_str(), m_filename[track].c_str());
        }
        else {
            path = m_filename[track].c_str();
        }
        if (m_savedVolume >= 0) {
            m_player->setVolume(m_savedVolume, 0);
            m_savedVolume = -1;
        }
        m_player->play(path.c_str(), sound == SFX_IDLE, 0);
        m_currentSound = sound;
        m_lastSFX = sound;
        return true;
    }
    return false;
}

bool SFX::playUISound(int n, bool prepend)
{
    CStr<10> str;
    switch(n) {
        case 0: str = "zero"; break;
        case 1: str = "one"; break;
        case 2: str = "two"; break;
        case 3: str = "three"; break;
        case 4: str = "four"; break;
        case 5: str = "five"; break;
        case 6: str = "six"; break;
        case 7: str = "seven"; break;
        default: ASSERT(false); break;
    }
    return playUISound(str.c_str(), prepend);
}

bool SFX::playUISound(const char* name, bool prepend)
{
    if (!m_player) return false;

    // Overrides the volume so the UI is at
    // a consistent volume. Volume will be restored
    // when the sound playing is done.
    CStr<24> path;
    if (prepend)
        path.append("ui/");
    path.append(name);
    path.append(".wav");

#if SERIAL_DEBUG == 1
#ifdef DEBUG_DEEP
    Log.p("SFX::playUISound sound: ").p(path.c_str()).eol();
    /*
    uint8_t nChannels = 0;
    uint32_t nSamplesPerSec = 0;
    uint32_t length = 0;

    readHeader(path.c_str(), &nChannels, &nSamplesPerSec, &length, false);
    Log.p(path.c_str()).p(":").p(nChannels).p(":").p(nSamplesPerSec).p(":").p(length).p(" ");
    */
#endif
#endif
    
    if (m_savedVolume < 0) {
        m_savedVolume = m_player->volume(0);
    }

    m_player->setVolume(128, 0);
    return m_player->play(path.c_str(), false, 0);
}

bool SFX::playSound(const char* sfx)
{
    if (!m_player) return false;

    if (m_savedVolume >= 0) {
        m_player->setVolume(m_savedVolume, 0);
        m_savedVolume = -1;
    }
    m_player->play(sfx, false, 0);
    return true;
}

void SFX::stopSound()
{
    if (!m_player) return;

    m_player->stop(0);
    m_currentSound = SFX_NONE;
}

void SFX::process()
{
    if (!m_player) return;

    if (!m_player->isPlaying(0)) {
        if (m_savedVolume >= 0) {
            //Log.p("restoring volume=").p(m_savedVolume).eol();
            m_player->setVolume(m_savedVolume, 0);
            m_savedVolume = -1;
        }
        // Play the idle sound if the blade is on.
        if (m_bladeOn) {
            playSound(SFX_IDLE, SFX_OVERRIDE);
        }
    }
    // Basically sets the enable line; do this 
    // last so enable isn't cycled without need.
    m_player->process();
}


#if (SABER_SOUND_ON == SABER_SOUND_SD)
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

bool SFX::readHeader(const char* filename, uint32_t* lengthMillis)
{
    File file = SD.open(filename);
    if (file) {
        Log.p(filename).eol();

        file.seek(22);
        uint32_t nAvgBytesPerSec = readU32(file, 4);
        *lengthMillis = (file.size() - 44u) * 1000u / (nAvgBytesPerSec);
        file.close();
        return true;
    }
    return false;
}
#elif SABER_SOUND_ON == SABER_SOUND_FLASH
bool SFX::readHeader(const char* filename, uint32_t* lengthMillis)
{
    MemUnit file;
    int index = MemImage.lookup(filename);
    if (index >= 0) {
        MemImage.readFile(index, &file);
        uint32_t addr = 0;
        wav12::Wav12Header header;
        MemImage.readAudioInfo(index, &header, &addr);
        *lengthMillis = 1000U * header.nSamples / 22050U;
        return true;
    }
    return false;
}
#else
bool SFX::readHeader(const char* filename, uint32_t* lengthMillis)
{
    *lengthMillis = 1000;
    return true;
}
#endif

void SFX::readIgniteRetract()
{
    CStr<25> path;

    if (m_location[SFX_POWER_ON].InUse()) {
        filePath(&path, m_location[SFX_POWER_ON].start);
        readHeader(path.c_str(), &m_igniteTime);
    }
    if (m_location[SFX_POWER_OFF].InUse()) {
        filePath(&path, m_location[SFX_POWER_OFF].start);
        readHeader(path.c_str(), &m_retractTime);
    }
}


void SFX::setVolume204(int vol)
{
    if (!m_player) return;
    vol = constrain(vol, 0, 204);
    if (vol >= 204) {
        if (m_player)
            m_player->setVolume(256, 0);
    }
    else {
        if (m_player)
            m_player->setVolume(vol * 256 / 204, 0);
    }
}


uint8_t SFX::getVolume204() const
{
    if (m_player)
        return m_player->volume(0) * 204 / 256;
    return 160;
}


uint8_t SFX::setFont(uint8_t font)
{
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
    for(; i<m_numFonts; ++i) {
        if (strEqual(name, fontName(i))) {
            break;
        }
    }
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
