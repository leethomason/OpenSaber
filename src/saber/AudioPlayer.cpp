#include <Arduino.h>

#ifdef CORE_TEENSY
#include "AudioPlayer.h"
#include "pins.h"

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Audio.h>

#include "Grinliz_Arduino_Util.h"

#if (SABER_SOUND_ON == SABER_SOUND_FLASH)
AudioPlaySerialflashRaw playWav;
#else
AudioPlaySdWav      playWav;
#endif

AudioMixer4         mixer;
AudioOutputAnalog   dac;
#ifdef SABER_LOWPASS_FILTER
AudioFilterBiquad   biquad;
AudioConnection     patchCord0(playWav, biquad);
AudioConnection     patchCord1(biquad, mixer);
#else
AudioConnection     patchCord1(playWav, mixer);
#endif
AudioConnection     patchCord2(mixer, 0, dac, 0);

AudioPlayer::AudioPlayer() {
    m_enabled = false;
    m_startPlayingTime = 0;
    m_volume = 1.0f;
    m_nEnabled = 0;
    m_nDisabled = 0;

    // Start disabled.
    digitalWrite(PIN_AMP_EN, LOW);
    pinMode(PIN_AMP_EN, OUTPUT);
    digitalWrite(PIN_AMP_EN, LOW);

#ifdef SABER_LOWPASS_FILTER
    biquad.setLowpass(0, 4000, 0.707);
#endif
}

void AudioPlayer::init() {
    // Audio connections require memory to work.  For more
    // detailed information, see the MemoryAndCpuUsage example
    AudioMemory(8);
    // This is much louder, but started causing distortion.
    // Bug in the system? Better value to use?
    //#ifdef SABER_AUDIO_EXTERNAL_REF
    //dac.analogReference(AR_EXTERNAL); // much louder!
    //#endif
    dac.analogReference(INTERNAL);
    delay(50);  //stabalize
}

bool AudioPlayer::play(const char* filename)
{
    bool okay = true;
#if (SABER_SOUND_ON == SABER_SOUND_SD)
    okay = playWav.play(filename);
#elif (SABER_SOUND_ON == SABER_SOUND_FLASH)
    okay = playWav.play(filename, 44);
#endif
    if (!okay) {
        Log.p("Error playing:").p(filename).eol();
        ASSERT(okay);
    }

    m_startPlayingTime = millis();
    setShutdown();
    return okay;
}

void AudioPlayer::stop() {
    playWav.stop();
    // Cool the data xfer
    delay(10);
    setShutdown();
}

bool AudioPlayer::isPlaying() const {
    uint32_t currentTime = millis();
    uint32_t warmTime = m_startPlayingTime + 20u;  // PJRC uses a 5ms time. Can be generous; no reason to support sub-second samples.

    if (!m_startPlayingTime)
        return false;

    // Special case: did we just start up? Because isPlaying() will be false during warm-up.
    if (m_startPlayingTime && currentTime < warmTime) 
        return true;

    return playWav.isPlaying();
}

void AudioPlayer::process()
{
    setShutdown();
}

void AudioPlayer::setShutdown() {
    bool shouldEnable = (m_volume > 0) && isPlaying();

    if (shouldEnable && !m_enabled) {
        //Log.p("AudioPlayer: amp enabled.").eol();
        digitalWrite(PIN_AMP_EN, HIGH);
        delay(10);  // warm up the amp.
        m_enabled = true;
        ++m_nEnabled;
    }
    else if (!shouldEnable && m_enabled) {
        //Log.p(" AudioPlayer: amp shutdown.").eol();
        digitalWrite(PIN_AMP_EN, LOW);
        m_enabled = false;
        ++m_nDisabled;
    }
}

void AudioPlayer::setVolume(int vInt) {
    float v = float(vInt) / 256.0f;

    m_volume = v;
    mixer.gain(0, m_volume);
    setShutdown();
}

#endif // CORE_TEENSY
