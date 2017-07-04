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
    //dac.analogReference(EXTERNAL); // much louder!
    delay(50);  //stabalize
}

void AudioPlayer::play(const char* filename)
{
    bool okay = true;
#if (SABER_SOUND_ON == SABER_SOUND_SD)
    okay = playWav.play(filename);
#elif (SABER_SOUND_ON == SABER_SOUND_FLASH)
    okay = playWav.play(filename, 44);
#endif
    if (!okay) {
        ASSERT(okay);
        Log.p("Error playing:").p(filename).eol();
    }

    setShutdown();
    //Serial.print("AudioPlayer::play: "); Serial.println(filename);// Serial.print(" len(ms)="); Serial.println(playWav.lengthMillis());
    // remember, about a 5ms warmup for header to be loaded and start playing.
    m_startPlayingTime = millis();
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
    if (m_startPlayingTime && currentTime > warmTime) {
        return playWav.isPlaying();
    }
    return true;
}

void AudioPlayer::process()
{
    setShutdown();
}

void AudioPlayer::setShutdown() {
    bool shouldEnable = (m_volume > 0) && isPlaying();

    if (shouldEnable && !m_enabled) {
        Log.p("AudioPlayer: amp enabled.").eol();
        digitalWrite(PIN_AMP_EN, HIGH);
        delay(10);  // warm up the amp.
        m_enabled = true;
    }
    else if (!shouldEnable && m_enabled) {
        Log.p(" AudioPlayer: amp shutdown.").eol();
        digitalWrite(PIN_AMP_EN, LOW);
        m_enabled = false;
    }
}

void AudioPlayer::setVolume(float v) {
    m_volume = v;
    mixer.gain(0, m_volume);
    setShutdown();
}


uint32_t AudioPlayer::lengthMillis() const
{
    return playWav.lengthMillis();
}
