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
#include "modes.h"

Timer2 smoothTimer(253);

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
    m_blend256 = 0;

    scanFiles();
}

void SFX::scanFiles()
{
    ASSERT(m_currentFont >= 0 && m_currentFont < MEM_IMAGE_NUM_DIR);
    const MemUnit& dirUnit = m_manifest.getUnit(m_currentFont);
    int start = dirUnit.offset;
    int count = dirUnit.size;
    //Log.p("scanning: ").p(dirUnit.getName().c_str()).p(" start=").p(start).p(" count=").p(count).eol();

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

        //Log.p("slot name=").pt(name).p(" slot=").p(slot).eol();

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
        "Motion(-low)",
        "Motion-High ",
        "Impact      ",
        "Blaster     ",
        "Power_On    ",
        "Power_Off   "
    };
    for(int i=0; i<NUM_SFX_TYPES; ++i) {
        Log.p(NAMES[i]).p("start=").p(m_sfxType[i].start).p(" count=").p(m_sfxType[i].count).eol();
    }
    if (m_smoothMode && (m_sfxType[SFX_MOTION].count != m_sfxType[SFX_MOTION_HIGH].count)) {
        Log.p("High/Low motion counts are not equal. Disabling smooth swing.").eol();
        m_smoothMode = false;
    }
    readIgniteRetract();
    Log.p("SFX mode=").p(m_smoothMode ? "Smooth" : "Event").p(" ignite=").p(m_igniteTime).p(" retract=").p(m_retractTime).eol();
}

int SFX::calcSlot(const char *name)
{
    int slot = -1;

    if (strstr(name, "POWERONF"))
        return -1;

    if (istrStarts(name, "BLDON") || istrStarts(name, "POWERON") || istrStarts(name, "out"))
        slot = SFX_POWER_ON;
    else if (istrStarts(name, "BLDOFF") || istrStarts(name, "POWEROFF") || istrStarts(name, "in"))
        slot = SFX_POWER_OFF;
    else if (istrStarts(name, "IDLE") || istrStarts(name, "HUM"))
        slot = SFX_IDLE;
    else if (istrStarts(name, "IMPACT") || istrStarts(name, "CLASH") || istrStarts(name, "clsh"))
        slot = SFX_IMPACT;
    else if (istrStarts(name, "USRTAP") || istrStarts(name, "BLASTER") || istrStarts(name, "blst"))
        slot = SFX_USER_TAP;
    else if (istrStarts(name, "swingL") || istrStarts(name, "lswing"))
        slot = SFX_MOTION;
    else if (istrStarts(name, "swingH") || istrStarts(name, "hswing"))
        slot = SFX_MOTION_HIGH;
    else if (istrStarts(name, "MOTION") || istrStarts(name, "SWING"))
        slot = SFX_MOTION;

    return slot;
}


int SFX::getTrack(int sound)
{
    ASSERT(sound >= 0);
    ASSERT(sound < NUM_SFX_TYPES);
    ASSERT(m_sfxType[sound].count > 0);

    int track = m_sfxType[sound].start + m_random.rand(m_sfxType[sound].count);

    if (track < MEM_IMAGE_NUM_DIR || track >= MEM_IMAGE_TOTAL) {
        Log.p("track=").p(track).p(" sound=").p(sound).eol();
        ASSERT(track >= MEM_IMAGE_NUM_DIR);
        ASSERT(track < MEM_IMAGE_TOTAL);
    }
    return track;
}

void SFX::playMotionTracks()
{
    ASSERT(m_sfxType[SFX_MOTION].count == m_sfxType[SFX_MOTION_HIGH].count);
    int offset = m_random.rand(m_sfxType[SFX_MOTION].count);
    int t0 = SFX_MOTION;
    int t1 = SFX_MOTION_HIGH;

    if (m_random.rand(3) == 0) {
        glSwap(t0, t1);
    }
    m_driver->play(m_sfxType[t0].start + offset, true, CHANNEL_MOTION_0);
    m_driver->play(m_sfxType[t1].start + offset, true, CHANNEL_MOTION_1);
    Log.p("playMotionTracks track=").p(offset).eol();
}

bool SFX::playSound(int sound, int mode, int channel)
{
    if (!m_driver) return false;
    Log.p("SFX play sound=").p(sound).eol();

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
        int track = getTrack(sound);

        CStr<9> filename;
        const MemUnit& memUnit = m_manifest.getUnit(track);
        memUnit.name.toStr(&filename);

        Log.p("SFX play=").p(filename.c_str()).p(" track=").p(track)
            .p(" [")
            .p(m_sfxType[sound].start).p(",")
            .p(m_sfxType[sound].start + m_sfxType[sound].count).p(")")
            .eol();

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
        m_driver->play(index, false, m_smoothMode ? CHANNEL_EVENT : 0);
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


void SFX::setVolume(int v) 
{ 
    m_volume = glClamp(v, 0, 512);
    if (!m_smoothMode) {          
        m_driver->setVolume(m_volume, 0);
    }
}

void SFX::setBoost(int boost, int ch)
{
    if (ch >=0 && ch < N_CHANNEL) {
        m_boost[ch] = boost;
    }
}

void SFX::sm_ignite()
{
    Log.p("sm_ignite").eol();
    m_driver->play(getTrack(SFX_IDLE), true, CHANNEL_IDLE);
    playMotionTracks();
    m_driver->play(getTrack(SFX_POWER_ON), false, CHANNEL_EVENT);

    //m_driver->setVolume(0, CHANNEL_IDLE);
    //m_driver->setVolume(0, CHANNEL_MOTION_0);
    //m_driver->setVolume(0, CHANNEL_MOTION_1);
    m_driver->setVolume(m_volume, CHANNEL_EVENT);
    //m_driver->setVolume(m_volume /** m_boost[CHANNEL_EVENT] / 256*/, CHANNEL_EVENT);
    //volumeEnvelope.start(m_igniteTime, 0, 256);
    sfxCalc.ignite(m_igniteTime);
}

void SFX::sm_retract()
{
    Log.p("sm_retract").eol();
    m_driver->play(getTrack(SFX_POWER_OFF), false, CHANNEL_EVENT);
    //m_driver->setVolume(0, CHANNEL_MOTION_0);
    //m_driver->setVolume(0, CHANNEL_MOTION_1);
    m_driver->setVolume(m_volume, CHANNEL_EVENT);
    //volumeEnvelope.start(m_retractTime, 256, 0);
    sfxCalc.retract(m_retractTime);
}

bool SFX::sm_playEvent(int sfx)
{
    m_driver->play(getTrack(sfx), false, CHANNEL_EVENT);
    m_driver->setVolume(m_volume /** m_boost[CHANNEL_EVENT] / 256*/, CHANNEL_EVENT);
    return true; // fixme
}

bool SFX::process(int bladeMode, uint32_t delta)
{
    bool stillReset = false;
    if (m_smoothMode) {
        SFXCalc::TrackVolume tv = sfxCalc.sm_processTrackVolume(delta, m_radPerSec, m_blend256);
        if (tv.stillReset) {
            playMotionTracks(); // new random tracks.
        }

#if false
        if (smoothTimer.tick(delta)) {
            Log.p("rps=").p(m_radPerSec).p(" blend=").p(m_blend256).eol();
        }
#endif        

        m_driver->setVolume(scaleVolume(tv.hum), CHANNEL_IDLE);
        m_driver->setVolume(scaleVolume(tv.swing0), CHANNEL_MOTION_0);
        m_driver->setVolume(scaleVolume(tv.swing1), CHANNEL_MOTION_1);
        stillReset = tv.stillReset;
    }
    else {
        if ((bladeMode == BLADE_ON) && !m_driver->isPlaying(0)) {
            playSound(SFX_IDLE, SFX_GREATER);
        }
    }
    return stillReset;
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
