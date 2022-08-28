/*
  Copyright (c) Lee Thomason, Grinning Lizard Software

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

#pragma once

#include <stdint.h>
#include "spistream.h"
#include "./src/wav12/expander.h"
#include "./src/util/interface.h"

class Adafruit_ZeroDMA;
class Adafruit_ZeroI2S;
class Manifest;

#define AUDDRV_NUM_CHANNELS 4
#define AUDDRV_FREQ 22050
#define AUDDRV_BUFFER_SAMPLES 256
#define AUDDRV_STEREO_SAMPLES (AUDDRV_BUFFER_SAMPLES*2)
#define AUDDRV_MSEC_PER_AUDIO_BUFFER (1000 * AUDDRV_BUFFER_SAMPLES / AUDDRV_FREQ)
#define AUDDRV_MICRO_PER_AUDIO_BUFFER (1000 * AUDDRV_MSEC_PER_AUDIO_BUFFER)

class I2SAudioDriver
{
public:
    I2SAudioDriver(Adafruit_ZeroDMA* _dma, Adafruit_ZeroI2S* _i2s, IMemory* _spiFlash, const Manifest& _manifest) :
        dma(_dma),
        i2s(_i2s),
        spiFlash(_spiFlash),
        manifest(_manifest)
    {}

    void begin();

    void play(int index, bool loop, int channel);
    bool isPlaying(int channel);
    void stop(int channel);
    void setVolume(int v, int channel);
    int getVolume(int channel);

private:
    struct Status {
        uint32_t addr = 0;
        uint32_t size = 0;
        int table = 0;
        bool loop = false;

        void clear() {
            addr = size = table = 0;
            loop = false;
        }
    };

    static Status status[AUDDRV_NUM_CHANNELS];
    static bool isQueued[AUDDRV_NUM_CHANNELS];
    static int volume[AUDDRV_NUM_CHANNELS];

    Adafruit_ZeroDMA* dma;
    Adafruit_ZeroI2S* i2s;
    IMemory* spiFlash;
    const Manifest& manifest;

    static SPIStream spiStream[AUDDRV_NUM_CHANNELS];
    static wav12::ExpanderAD4 expander[AUDDRV_NUM_CHANNELS];

    static void DMACallback(Adafruit_ZeroDMA* dma);
};