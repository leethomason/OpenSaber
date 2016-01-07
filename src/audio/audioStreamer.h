#ifndef AUDIO_STREAMER_INCLUDED
#define AUDIO_STREAMER_INCLUDED

#include <stdint.h>

class File;

class AudioStreamer {
  public:
    AudioStreamer();
    ~AudioStreamer();

    void play(File* file);
    int fillBuffer();

    static int16_t readS16(File& file);
    static uint32_t readU32(File& file, int n);

    static void interuptFunc();

  private:
    void fillBuffer(int start, int n);
  
    File* file;
    uint32_t nSamples;
    uint32_t nChannels; // 1 or 2
    uint32_t freq;      // 1 (22) or 2 (44)
    uint32_t bytesPerSample;
};

#endif // AUDIO_STREAMER_INCLUDED



