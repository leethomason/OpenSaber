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

// #define SMOOTH_LOG
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

    m_swingDecay.setScaledPeriod(1.5f);
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
    //Log.p("playMotionTracks track=").p(offset).eol();
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
    m_volume = glClamp(v, 0, 256);
    if (!m_smoothMode) {          
        m_driver->setVolume(m_volume, 0);
    }
}

void SFX::sm_ignite()
{
    Log.p("sm_ignite").eol();
    m_driver->play(getTrack(SFX_IDLE), true, CHANNEL_IDLE);
    playMotionTracks();
    m_driver->play(getTrack(SFX_POWER_ON), false, CHANNEL_EVENT);

    m_driver->setVolume(0, CHANNEL_IDLE);
    m_driver->setVolume(0, CHANNEL_MOTION_0);
    m_driver->setVolume(0, CHANNEL_MOTION_1);
    m_driver->setVolume(m_volume, CHANNEL_EVENT);

    volumeEnvelope.start(m_igniteTime, 0, 256);
}

void SFX::sm_retract()
{
    Log.p("sm_retract").eol();
    m_driver->play(getTrack(SFX_POWER_OFF), false, CHANNEL_EVENT);
    m_driver->setVolume(0, CHANNEL_MOTION_0);
    m_driver->setVolume(0, CHANNEL_MOTION_1);
    m_driver->setVolume(m_volume, CHANNEL_EVENT);

    volumeEnvelope.start(m_retractTime, 256, 0);
}

bool SFX::sm_playEvent(int sfx)
{
    m_driver->play(getTrack(sfx), false, CHANNEL_EVENT);
    m_driver->setVolume(m_volume, CHANNEL_EVENT);
    return true; // fixme
}


int SFX::sm_swingToVolume(float radPerSec)
{
    static const float STILL = 3.0f;
    static const float FAST  = 8.0f;

    FixedNorm motionFraction = 0;
    if (radPerSec >= FAST) {
        motionFraction = 1;
    }
    else if (radPerSec > STILL) {
        motionFraction = (radPerSec - STILL) / (FAST - STILL);
    }
    int swing = motionFraction.scale(256);
    // Log.p("rad/sec=").p(radPerSec).p(" motionFraction=").p(motionFraction.toFloat()).p(" swing=").p(swing).eol();
    return swing;
}

int SFX::scaleVolume(int v) const
{
    v = glClamp(v, 0, 255);
    // [0,255] * [0,256] * [0,256]
    v = (v * volumeEnvelope.value() * m_volume) >> 16;
    return v;
}


void SFX::process(int bladeMode, uint32_t delta, bool* still)
{
    if (m_smoothMode) {
        // Animates from 0->256 on ignite, and back to 0 on retract.
        // 256 or 0 at steady state.
        volumeEnvelope.tick(delta);

        int read = sm_swingToVolume(m_speed);
        int nTick = m_swingDecay.tick(delta);

        m_swing -= nTick;
        if (m_swing < 0) m_swing = 0;    
        m_swingTarget -= nTick;
        if (m_swingTarget < 0) m_swingTarget = 0;

        m_swingTarget = glMax(read, m_swingTarget);
        if(m_swingTarget > m_swing) {
            m_swing += nTick * 2;   // because we removed it above...
            if (m_swing > m_swingTarget) m_swing = m_swingTarget;
        }
        int swing = m_swing;

        /*
        int swing = m_swing = glMax(read, m_swing);
        */

        int hum = 256 - swing * 192 / 256;

        if (swing == 0) {
            *still = true;
            m_stillCount++;
            if (m_stillCount == 30)
                playMotionTracks(); // new random tracks.
        }
        else {
            *still = false;
            m_stillCount = 0;
        }

        //int swing0 = iCos(FixedNorm(m_blend256, 1024)).scale(swing);
        //int swing1 = iSin(FixedNorm(m_blend256, 1024)).scale(swing);
        // I think I like linear better?
        int swing0 = lerp256(swing, 0, m_blend256);
        int swing1 = lerp256(0, swing, m_blend256);

#ifdef SMOOTH_LOG
        if (smoothTimer.tick(delta)) {
            Log.p("speed=").p(m_speed).p(" swing=").p(swing).p(" hum=").p(hum)
            .p(" blend=").p(m_blend256)
            .p(" swing0=").p(swing0).p(" swing1=").p(swing1)
            .eol();
        }
#endif        

        // volume is scaled by overall output volume (m_volume) and the
        // current value of the volumeEnvelope. So this accounts for
        // on/off/ignite/retract.
        m_driver->setVolume(scaleVolume(hum),    CHANNEL_IDLE);
        m_driver->setVolume(scaleVolume(swing0), CHANNEL_MOTION_0);
        m_driver->setVolume(scaleVolume(swing1), CHANNEL_MOTION_1);
    }
    else {
        if ((bladeMode == BLADE_ON) && !m_driver->isPlaying(0)) {
            playSound(SFX_IDLE, SFX_GREATER);
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
