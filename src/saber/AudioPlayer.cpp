#include "AudioPlayer.h"
#include "pins.h"

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Audio.h>

AudioPlaySdWav      playWav;
AudioMixer4         mixer;
AudioOutputAnalog   dac;
AudioConnection     patchCord1(playWav, mixer);
AudioConnection     patchCord2(mixer, 0, dac, 0);

AudioPlayer::AudioPlayer() {
  m_muted = false;
  m_shutdown = false;
  m_startPlayingTime = 0;
  m_volume = 1.0f;
  pinMode(PIN_AMP_SHUTDOWN, INPUT);
}

void AudioPlayer::init() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(8);

  SPI.setMOSI(PIN_SDCARD_MOSI);
  SPI.setSCK(PIN_SDCARD_SCK);
  if (!(SD.begin(PIN_SDCARD_CS))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

void AudioPlayer::play(const char* filename) 
{
  playWav.play(filename);
  Serial.print("AudioPlayer::play: "); Serial.println(filename);// Serial.print(" len(ms)="); Serial.println(playWav.lengthMillis());
  // remember, about a 5ms warmup for header to be loaded and start playing.
  m_startPlayingTime = millis();
}

void AudioPlayer::stop() {
  playWav.stop();
//  m_stopPlayingTime = millis();
}

bool AudioPlayer::isPlaying() const {
  uint32_t currentTime = millis();
  uint32_t warmTime = m_startPlayingTime + 20u;  // PJRC uses a 5ms time. Can be generous; no reason to support sub-second samples.
  if (m_startPlayingTime && currentTime > warmTime) {
    return playWav.isPlaying();
  }
  return true;
}

void AudioPlayer::mute(bool m) {
  m_muted = m;
  if (m_muted && !m_shutdown) {
    Serial.println("shutdown audio");
    digitalWrite(PIN_AMP_SHUTDOWN, LOW);
    pinMode(PIN_AMP_SHUTDOWN, OUTPUT);
  }
  else if (!m_muted && m_shutdown) {
    Serial.println("enable audio");
    pinMode(PIN_AMP_SHUTDOWN, INPUT);
  }
  m_shutdown = m_muted;
}

void AudioPlayer::setVolume(float v) {
  m_volume = v;
  mixer.gain(0, m_volume);
}

