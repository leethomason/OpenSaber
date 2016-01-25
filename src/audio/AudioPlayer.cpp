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
}

void AudioPlayer::stop() {
  playWav.stop();
}

