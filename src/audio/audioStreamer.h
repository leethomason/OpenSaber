#ifndef AUDIO_STREAMER_INCLUDED
#define AUDIO_STREAMER_INCLUDED

#include <stdint.h>

class File;

class AudioStreamer {
  public:
    AudioStreamer();
    ~AudioStreamer();

    void play(File* file);
    void fillBuffer();

    static int16_t readS16(File& file);
    static uint32_t readU32(File& file, int n);

    static void interuptFunc();

  private:
    File* file;
    uint32_t startTimeMicro;
    uint32_t freq;
    uint32_t currentSample;
    uint32_t endSample;
};

#endif // AUDIO_STREAMER_INCLUDED



