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
  startPlayingTime = 0;
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
  startPlayingTime = millis();
}

void AudioPlayer::stop() {
  playWav.stop();
}

bool AudioPlayer::isPlaying() const {
  uint32_t currentTime = millis();
  uint32_t warmTime = startPlayingTime + 20u;  // PJRC uses a 5ms time. Can be generous; no reason to support sub-second samples.
  if (currentTime > warmTime) {
    return playWav.isPlaying();
  }
  return true;
}

