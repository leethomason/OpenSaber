#include "audioStreamer.h"
#include <SD.h>

// 22.05 kHz playback, 2 bytes per sample, mono.
// 44,100 bytes / second
static const uint32_t BUFFER_SIZE = 4096; // 1/4 second
static const uint32_t BUFFER_SIZE_MASK = BUFFER_SIZE - 1;

uint16_t buffer[BUFFER_SIZE];
uint32_t samplesRemaining = 0;
volatile uint32_t playHead = 0;
uint32_t dataHead = 0;

AudioStreamer::AudioStreamer() {
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
  dataHead = BUFFER_SIZE - 1;
  samplesRemaining = nSamples;

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

void AudioStreamer::fillBuffer(int start, int n) {
  int pos = start;
  while (n--) {
    int32_t v0 = readS16(*file) + 32767;
    int32_t v1 = readS16(*file) + 32767;
    buffer[pos & BUFFER_SIZE_MASK] = uint16_t((v0 + v1) / 2);
    ++pos;
  }
}

int AudioStreamer::fillBuffer() {
  noInterrupts();
  uint32_t delta = ((playHead + BUFFER_SIZE - dataHead) & BUFFER_SIZE_MASK) - 1;
  interrupts();
  fillBuffer(dataHead, delta);
  dataHead = (playHead + BUFFER_SIZE - 1) & BUFFER_SIZE_MASK;
  return delta;
}

void AudioStreamer::interuptFunc() {
  if (samplesRemaining) {
    --samplesRemaining;
    analogWrite(A14, buffer[playHead]);
    playHead = (playHead + 1) & BUFFER_SIZE_MASK;
  }
}

