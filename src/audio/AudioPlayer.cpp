#include "AudioPlayer.h"
#include "pins.h"

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Audio.h>

AudioPlaySdWav      playWav;
AudioOutputAnalog   dac;
AudioConnection     patchCord1(playWav, 0, dac, 0);

AudioPlayer::AudioPlayer() {
  m_startPlayingTime = 0;
  m_muted = false;
  m_shutdown = false;
  pinMode(AMP_SHUTDOWN_PIN, INPUT);
}

void AudioPlayer::init() {

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(8);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

void AudioPlayer::play(const char* filename) {
  Serial.print("AudioPlayer::play: "); Serial.println(filename);
  
  playWav.play(filename);
  // remember, about a 5ms warmup for header to be loaded and start playing.
  m_startPlayingTime = millis();
  m_stopPlayingTime = 0;
}

void AudioPlayer::stop() {
  playWav.stop();
  m_stopPlayingTime = millis();
}

bool AudioPlayer::isPlaying() const {
  uint32_t currentTime = millis();
  uint32_t warmTime = m_startPlayingTime + 20u;  // PJRC uses a 5ms time. Can be generous; no reason to support sub-second samples.
  if (m_startPlayingTime && currentTime > warmTime) {
    return playWav.isPlaying();
  }
  return true;
}

void AudioPlayer::doLoop() {
  if (m_startPlayingTime && !m_stopPlayingTime && !isPlaying()) {
    m_stopPlayingTime = millis();
  }
  setShutdown();
}

void AudioPlayer::setShutdown() {
  uint32_t currentTime = millis();
  uint32_t coolTime = m_stopPlayingTime + 20u;

  bool shouldBeShutdown = m_muted ||
                          (m_stopPlayingTime && currentTime > coolTime);

  if (shouldBeShutdown && !m_shutdown) {
    digitalWrite(AMP_SHUTDOWN_PIN, LOW);
    pinMode(AMP_SHUTDOWN_PIN, OUTPUT);
  }
  else if (!shouldBeShutdown && m_shutdown) {
    pinMode(AMP_SHUTDOWN_PIN, INPUT);
  }
  m_shutdown = shouldBeShutdown;
}


