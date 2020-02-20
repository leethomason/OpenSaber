/*
  Copyright (c) Lee Thomason, Grinning Lizard Software

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

#include "manifest.h"
#include "sfx.h"
#include "tester.h"
#include "i2saudiodrv.h"

SFX* SFX::m_instance = 0;

SFX::SFX(I2SAudioDriver *driver, const Manifest& manifest) :
    m_driver(driver),
    m_manifest(manifest)
{
    m_instance = this;
    m_currentSound = SFX_NONE;
    m_currentFont = 0;
    m_igniteTime = 1000;
    m_retractTime = 1000;
    m_volume = 64;
    m_smoothMode = false;

    scanFiles();
}

void SFX::scanFiles()
{
    ASSERT(m_currentFont >= 0 && m_currentFont < MEM_IMAGE_NUM_DIR);
    const MemUnit& dirUnit = m_manifest.getUnit(m_currentFont);
    int start = dirUnit.offset;
    int count = dirUnit.size;
    Log.p("scanning: ").p(dirUnit.getName().c_str()).p(" start=").p(start).p(" count=").p(count).eol();

    // Group the files; assume they are already
    // in swing / clash / etc. groups.
    for(int i=0; i<NUM_SFX_TYPES; ++i) {
        m_sfxType[i].start = 0;
        m_sfxType[i].count = 0;
    }

    m_smoothMode = false;
    for(int i=start; i < start + count; ++i) {
        const MemUnit& memUnit = m_manifest.getUnit(i);
        CStr<9> name;
        memUnit.name.toStr(&name);
        int slot = calcSlot(name.c_str());
        if (slot == SFX_MOTION_HIGH)
            m_smoothMode = true;

        Log.p("slot name=").pt(name).p(" slot=").p(slot).eol();

        if (slot >= 0) {
            if (m_sfxType[slot].count == 0) {
                m_sfxType[slot].start = i;
                m_sfxType[slot].count = 1;
            }
            else {
                m_sfxType[slot].count++;
            }
        }
    }

    static const char* NAMES[NUM_SFX_TYPES] = {
        "Idle        ",
        "Motion      ",
        "Motion-high ",
        "Impact      ",
        "User_Tap    ",
        "Power_On    ",
        "Power_Off   "
    };
    for(int i=0; i<NUM_SFX_TYPES; ++i) {
        Log.p(NAMES[i]).p("start=").p(m_sfxType[i].start).p(" count=").p(m_sfxType[i].count).eol();
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
    else if (istrStarts(name, "USRTAP")  || istrStarts(name, "BLASTER"))    slot = SFX_USER_TAP;
    else if (istrStarts(name, "swingL"))                                    slot = SFX_MOTION;
    else if (istrStarts(name, "swingH"))                                    slot = SFX_MOTION_HIGH;
    else if (istrStarts(name, "MOTION")  || istrStarts(name, "SWING"))      slot = SFX_MOTION;

    return slot;
}

bool SFX::playSound(int sound, int mode, int channel)
{
    if (!m_driver) return false;

    ASSERT(sound >= 0);
    ASSERT(sound < NUM_SFX_TYPES);

    if (!m_driver->isPlaying(0)) {
        m_currentSound = SFX_NONE;
    }

    if (   m_currentSound == SFX_NONE
            || (mode == SFX_OVERRIDE)
            || (mode == SFX_GREATER && sound > m_currentSound)
            || (mode == SFX_GREATER_OR_EQUAL && sound >= m_currentSound))
    {
        int track = m_sfxType[sound].start + m_random.rand(m_sfxType[sound].count);
        ASSERT(track >= MEM_IMAGE_NUM_DIR);
        ASSERT(track < MEM_IMAGE_TOTAL);

        CStr<9> filename;
        const MemUnit& memUnit = m_manifest.getUnit(track);
        memUnit.name.toStr(&filename);

        Log.p("SFX play=").p(filename.c_str()).p(" track=").p(track)
            .p(" [")
            .p(m_sfxType[sound].start).p(",")
            .p(m_sfxType[sound].start + m_sfxType[sound].count).p("]")
            .eol();
        // EventQ.event("[SFX play]", sound);

        m_driver->play(track, sound == SFX_IDLE, channel);
        m_driver->setVolume(m_volume, channel);
        if (channel == 0)
            m_currentSound = sound;
        return true;
    }
    return false;
}

bool SFX::playSound(const char* sfx)
{
    if (!m_driver) return false;

    int index = m_manifest.getFile(m_currentFont, sfx);
    if (index > 0) {
        m_driver->play(index, false, 0);
        m_driver->setVolume(m_volume, 0);
        return true;
    }
    return false;
}

void SFX::stopSound()
{
    if (!m_driver) return;

    m_driver->stop(0);
    m_currentSound = SFX_NONE;
}

void SFX::sm_ignite()
{
    playSound(SFX_POWER_ON, SFX_OVERRIDE, CHANNEL_IDLE);
    m_driver->setVolume(m_volume, CHANNEL_IDLE);
    m_driver->setVolume(0, CHANNEL_MOTION_0);
    m_driver->setVolume(0, CHANNEL_MOTION_1);
    m_driver->setVolume(0, CHANNEL_EVENT);
}

void SFX::sm_retract()
{
    playSound(SFX_POWER_OFF, SFX_OVERRIDE, CHANNEL_IDLE);
    m_driver->setVolume(m_volume, CHANNEL_IDLE);
    m_driver->setVolume(0, CHANNEL_MOTION_0);
    m_driver->setVolume(0, CHANNEL_MOTION_1);
    m_driver->setVolume(0, CHANNEL_EVENT);
}

bool SFX::sm_playEvent(int sfx)
{
    playSound(sfx, SFX_OVERRIDE, CHANNEL_EVENT);
    m_driver->setVolume(m_volume, CHANNEL_EVENT);
}

// TODO: blend 2 swing sounds.
void SFX::sm_swingToVolume(float radPerSec, int* hum, int* swing)
{
    static const float STILL = 1.0f;
    static const float FAST = 5.0f;

    FixedNorm motionFraction = 0;
    if (radPerSec >= FAST) {
        motionFraction = 1;
    }
    else if (radPerSec > STILL) {
        motionFraction = (radPerSec - STILL) / (FAST - STILL);
    }
    *hum = 256 - motionFraction.scale(256 - 64);
    *swing = motionFraction.scale(256);
}

void SFX::process(bool bladeOn)
{
    if (bladeOn) {
        if (m_smoothMode) {
            if (!m_driver->isPlaying(CHANNEL_IDLE)) {
                playSound(SFX_IDLE, SFX_OVERRIDE, CHANNEL_IDLE);
                // TODO: change motion sounds when still
                playSound(SFX_MOTION, SFX_OVERRIDE, CHANNEL_MOTION_0);
                playSound(SFX_MOTION_HIGH, SFX_OVERRIDE, CHANNEL_MOTION_1);
            }
            int hum = 256;
            int swing = 0;
            sm_swingToVolume(m_speed, &hum, &swing);
            m_driver->setVolume(scaleVolume(hum), CHANNEL_IDLE);
            m_driver->setVolume(scaleVolume(swing), CHANNEL_MOTION_0);
        }
        else {
            if (!m_driver->isPlaying(0)) {
                playSound(SFX_IDLE, SFX_GREATER);
            }
        }
    }
}

void SFX::readIgniteRetract()
{
    m_igniteTime = m_retractTime = 1000;

    if(m_sfxType[SFX_POWER_ON].count) {
        const MemUnit& memUnit = m_manifest.getUnit(m_sfxType[SFX_POWER_ON].start);
        m_igniteTime = memUnit.timeInMSec();
    }
    if(m_sfxType[SFX_POWER_OFF].count) {
        const MemUnit& memUnit = m_manifest.getUnit(m_sfxType[SFX_POWER_OFF].start);
        m_retractTime = memUnit.timeInMSec();
    }
}


int SFX::setFont(int font)
{
    font = glClamp(font, 0, MEM_IMAGE_NUM_DIR-1);
    const MemUnit& unit = m_manifest.getUnit(font);
    if (unit.size == 0) 
        font = 0;

    m_currentFont = font;
    scanFiles();
    return m_currentFont;
}

int SFX::setFont(const char* name)
{
    int index = m_manifest.getDir(name);
    if (index < 0) {
        index = 0;;
    }
    return setFont(index);
}


