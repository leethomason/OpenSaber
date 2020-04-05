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
#include <limits.h>
#include <assert.h>

static const int S4ADPCM_4BIT = 4;
static const int S4ADPCM_8BIT = 8;

/* 
    A 4-bit ADPCM encoder/decoder. It is not any of the "official" ADPCM codecs.
    It is very very simple, and surprisingly good quality. It was tuned on
    lightsabers sounds and a piece of classical music. Although you can hear
    some distortion, for the simplicity of the code, the amount of compression,
    and suitability to microcontrollers, it is surprisingly high quality.

    Properties of the codec:
    - very small amount of code
    - 4 bits / sample makes streaming consistent and simple
    - 16 bit codec (can unpack to 16 or 32 target)
    - integer only; does assume a 32 bit multiply (with 32 bit intermediate)
    - computation uses shifts; multiply is used only for volume; no division
    - no index table, only 8 magic-number tuning values

    How it works:
    Each value is predicted from the previous, based on a simple velocity 
    computation. The delta from that prediction is what is stored in the 
    compressed data. The value of the delta (-8 to +7) scaled from 0 to 12
    bits (State.shift). A large delta will increase the scaling for the
    next sample, a small delta will decrease it, so that the codec is always
    "chasing" the correct scale for the current samples.
*/
class S4ADPCM
{
public:
    struct State {
        int prev2 = 0;
        int prev1 = 0;
        int shift = 0;
        bool high = false;
        int volumeShifted = 0;
        int volumeTarget = 0;

        int guess() const {
            int g = 2 * prev1 - prev2;
            if (g > SHRT_MAX) g = SHRT_MAX;
            if (g < SHRT_MIN) g = SHRT_MIN;
            return g;
        }
        void push(int value) {
            prev2 = prev1;
            prev1 = value;
        }
    };

    static int encode4(const int16_t* data, int32_t nSamples, uint8_t* compressed, State* state, const int* table, int64_t* e16squared);
    static void decode4(const uint8_t* compressed,
        int32_t nSamples,
        int volume,         // 0-256 (higher values can overflow)
        bool add,           // if true, add to the 'data' buffer, else write to it
        int32_t* samples, State* state, const int* table);

    static void encode8(const int16_t* data, int32_t nSamples, uint8_t* compressed, State* state, const int* table, int64_t* e16squared);
    static void decode8(const uint8_t* compressed,
        int32_t nSamples,
        int volume,         // 0-256 (higher values can overflow)
        bool add,           // if true, add to the 'data' buffer, else write to it
        int32_t* samples, State* state, const int* table);

    static const int TABLE_SIZE = 9;
    static const int* getTable(int bits, int i) {
        assert(i >= 0 && i < N_TABLES);
        assert(bits == 8 || bits == 4);
        if (bits == 8) return DELTA_TABLE_8[i];
        return DELTA_TABLE_4[i];
    }

private:
    static const int SHIFT_LIMIT_4 = 12;
    static const int SHIFT_LIMIT_8 = 8;
    static const int VOLUME_EASING = 32;    // 8, 16, 32, 64? initial test on powerOn sound seemed 32 was good.

    static int64_t calcError(int value, int p) 
    {
        // Want this is 12 bit space, so divide by 16
        value /= 16;
        p /= 16;
        int64_t d = int64_t(value) - p;
        return d * d;
    }


    inline static int32_t sat_add(int32_t x, int32_t y)
    {
        int32_t sum = (uint32_t)x + y;
        const static int32_t BIT_SHIFT = sizeof(int32_t) * 8 - 1;
        // This is a wonderfully clever way of saying:
        // if (sign(x) != sign(y)) || (sign(x) == sign(sum)
        //      mask = 0;    // we didn't overflow
        // else
        //      mask = 1111s // we did, and create an all-1 mask with sign extension
        // I didn't think of this. Wish I had.
        int32_t mask = (~(x ^ y) & (x ^ sum)) >> BIT_SHIFT;
        int32_t maxOrMin = (1 << BIT_SHIFT) ^ (sum >> BIT_SHIFT);
        return  (~mask & sum) + (mask & maxOrMin);
    }

public:
    static const int N_TABLES = 4;
    static const int DELTA_TABLE_4[N_TABLES][TABLE_SIZE];
    static const int DELTA_TABLE_8[N_TABLES][TABLE_SIZE];
};
