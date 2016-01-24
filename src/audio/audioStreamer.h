/*
Copyright (c) 2016 Lee Thomason, Grinning Lizard Software

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.
*/

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
    uint32_t wavePos;
};

#endif // AUDIO_STREAMER_INCLUDED



