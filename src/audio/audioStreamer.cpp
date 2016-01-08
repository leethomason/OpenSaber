#include "audioStreamer.h"
#include <SD.h>
#include <Grinliz_Arduino_Util.h>

// 22.05 kHz playback, 2 bytes per sample, mono.
// 44,100 bytes / second
static const uint32_t BUFFER_SIZE = 4096; // 1/4 second
static const uint32_t BUFFER_SIZE_MASK = BUFFER_SIZE - 1;

uint16_t buffer[BUFFER_SIZE];
uint32_t dataHead = 0;
uint32_t samplesRemaining = 0;
uint8_t  looping = 0;

volatile uint8_t  playVolume = 128; // 0 - 128
volatile uint32_t playHead = 0;     // owned by interrupt, read by main

AudioStreamer::AudioStreamer() {
  playHead = 0;
  dataHead = 0;
  playVolume = 0;
  memset(buffer, 0, sizeof(buffer[0]) * BUFFER_SIZE);
}

AudioStreamer::~AudioStreamer() {
}

void AudioStreamer::play(File* _file) {
  file = _file;

  file->seek(22);
  nChannels = readU32(*file, 2);
  Serial.print("channels:        "); Serial.println(nChannels);
  uint32_t nSamplesPerSec = readU32(*file, 4);
  freq = 1;
  if (nSamplesPerSec >= 40000) freq = 2;
  Serial.print("nSamplesPerSec:  "); Serial.println(nSamplesPerSec);
  Serial.print("nAvgBytesPerSec: "); Serial.println(readU32(*file, 4));
  Serial.print("nBlockAlign:     "); Serial.println(readU32(*file, 2));
  Serial.print("wBitsPerSample:  "); Serial.println(readU32(*file, 2));

  bytesPerSample = freq * nChannels;
  nSamples = (file->size() - 44) / bytesPerSample;

  Serial.print("bytesPerSample:  "); Serial.println(bytesPerSample);
  Serial.print("nSamples:        "); Serial.println(nSamples);

  file->seek(44);

  fillBuffer(0, BUFFER_SIZE);
  playHead = 0;
  dataHead = 0;
  samplesRemaining = nSamples;
  looping = 1;
  playVolume = 128;

  /*
    uint32_t startTime = millis();
    static const int32_t nSamples = 44100;
    for (int32_t i = 0; i < nSamples; ++i) {
    int32_t sample = readS16(*file);
    //uint32_t value = uint32_t(sample + 32767);  // convert to 0-65535
    analogWrite(A14, sample + 32767);
    }
    uint32_t endTime = millis();
    Serial.print("Time to read/set "); Serial.print(nSamples); Serial.print(" samples: ");
    Serial.print(endTime - startTime); Serial.println(" millis");
  */
}


uint32_t AudioStreamer::readU32(File& file, int n) {
  uint32_t v = 0;
  for (int i = 0; i < n; ++i) {
    int b = file.read();
    v += b << (i * 8);
  }
  return v;
}

int16_t AudioStreamer::readS16(File& file) {
  int16_t v = 0;
  ((uint8_t*)&v)[0] = file.read();
  ((uint8_t*)&v)[1] = file.read();
  return v;
}

void AudioStreamer::fillBuffer(int pos, int n) {
  if (freq == 1) {
    // 22kHz
    while (n--) {
      int32_t v0 = readS16(*file) + 32767;
      buffer[pos & BUFFER_SIZE_MASK] = v0;
      ++pos;
      if (nChannels == 2) {
        file->read();
      }
    }
  }
  else {
    // 44kHz - downsample
    while (n--) {
      int32_t v0 = readS16(*file) + 32767;
      int32_t v1 = readS16(*file) + 32767;
      buffer[pos & BUFFER_SIZE_MASK] = uint16_t((v0 + v1) / 2);
      ++pos;
      if (nChannels == 2) {
        file->read();
        file->read();
      }
    }
  }
}

int AudioStreamer::fillBuffer() {
  // playHead dataHead
  // 0        0 (4096)  0 (full buffer)
  // 0        4095      1
  // 1        0         1
  // 0        1         4095

  uint32_t _playHead = atomicRead(&playHead) & BUFFER_SIZE_MASK;
  uint32_t nToFill   = (_playHead + BUFFER_SIZE - dataHead) & BUFFER_SIZE_MASK;
  uint32_t nFromCard = min(nToFill, samplesRemaining);

  fillBuffer(dataHead, nFromCard);
  dataHead = (dataHead + nFromCard) & BUFFER_SIZE_MASK;

  nToFill -= nFromCard;

  // Handle the looping case by going 
  // back to the beginning.
  if (nToFill && looping) {
    samplesRemaining = nSamples;  // reset.
    file->seek(44);
    return fillBuffer() + nFromCard;
  }

  // If not looping, fill with 0.
  while(nToFill--) {
    buffer[dataHead] = 0;
    dataHead = (dataHead + 1) & BUFFER_SIZE_MASK;
  }
  return nFromCard;
}

void AudioStreamer::interuptFunc() {
  uint32_t v = (uint32_t(buffer[playHead]) * playVolume) >> 7;
  analogWrite(A14, v);
  playHead = (playHead + 1) & BUFFER_SIZE_MASK;
}

