#include "audioStreamer.h"
#include <SD.h>

// 22.05 kHz playback, 2 bytes per sample, mono.
// 44,100 bytes / second
static const int BUFFER_SIZE = 4096; // 1/4 second
uint16_t buffer[BUFFER_SIZE];
volatile int bufferHead = 0;
volatile int bufferTail = 0;

AudioStreamer::AudioStreamer() {
  startTimeMicro = 0;
  freq = 0;
  currentSample = 0;
  endSample = 0;
}

AudioStreamer::~AudioStreamer() {
}

void AudioStreamer::play(File* _file) {
  file = _file;

  /*
    file->seek(8);
      for(int i=0; i<4; ++i) {
        int b = a440.read();
        Serial.println(char(b));
      }
  */
  file->seek(22);
  Serial.print("channels:        "); Serial.println(readU32(*file, 2));
  Serial.print("nSamplesPerSec:  "); Serial.println(readU32(*file, 4));
  Serial.print("nAvgBytesPerSec: "); Serial.println(readU32(*file, 4));
  Serial.print("nBlockAlign:     "); Serial.println(readU32(*file, 2));
  Serial.print("wBitsPerSample:  "); Serial.println(readU32(*file, 2));

  file->seek(44);
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



