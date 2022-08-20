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
#include <stdlib.h>
#include <algorithm>

#if defined(_MSC_VER)
#   include <assert.h>
#	define W12ASSERT assert
#else
#   define W12ASSERT
#endif

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

    8/2022 changes:
    - Processing of looping sounds will now look for a good zero point. This fixes
      the "pop on loop".
    - 8bit was removed: popping fixed, quality improved in 4bit
    - More tables.
    - Encoding/decoding will exceed 16bit limits, but the value passed on is clamped.
      this is a big code simplification
    - The value encoded in the stream isn't a multiplier, but an index to STEP.
      This was the big gain to encoding and the algorithm
    - Error is about 0.5 to 0.8 of previous values, across the board.
    Nice improvements. I'm very happy with the insights, and fiati.wav
    is audibly improved.
*/
class S4ADPCM
{
public:
    static const int ZERO_INDEX = 8;
    static const int TABLE_SIZE = 9;

    struct State {
        bool high = false;
        int32_t prev2 = 0;
        int32_t prev1 = 0;
        int32_t shift = 0;
        int32_t volumeShifted = 0;
        int32_t volumeTarget = 0;

        int32_t guess() const {
            // TotalError = 895  SimpleError = 52945
            //return 2 * prev1 - prev2;
            
            // In between: good numbers across the board,
            // but favors hum & swing
            // TotalError = 588  SimpleError = 33127
            return prev1 + (prev1 - prev2) / 2;
            
            // TotalError = 586  SimpleError = 28855
            //return prev1;
        }
        void push(int32_t value) {
            prev2 = prev1;
            prev1 = value;
        }

        void doShift(const int* table, int index) {
            W12ASSERT(index >= 0 && index < 16);

            int delta = abs(index - ZERO_INDEX); // 0 - 8
            shift += table[delta];
            if (shift < 0) shift = 0;
            if (shift > SHIFT_LIMIT_4) shift = SHIFT_LIMIT_4;
        }
    };

    static int encode4(const int16_t* data, int32_t nSamples, uint8_t* compressed, State* state, const int* table);
    static void decode4(const uint8_t *compressed,
                        int32_t nSamples,
                        int32_t volume, // 256 is neutral; normally 0-256. Above 256 can boost & clip.
                        bool add,   // if true, add to the 'data' buffer, else write to it
                        int32_t *samples, State *state, const int *table);

    static const int* getTable(int i) {
        assert(i >= 0 && i < N_TABLES);
        return DELTA_TABLE_4[i];
    }

private:
    static const int SHIFT_LIMIT_4 = 14;
    static const int VOLUME_EASING = 32;    // 8, 16, 32, 64? initial test on powerOn sound seemed 32 was good.

    inline static int32_t sat_mult(int32_t a, int32_t b)
    {
        int64_t s64 = int64_t(a) * int64_t(b);
        return (int32_t)std::min(std::max(s64, int64_t(INT32_MIN)), int64_t(INT32_MAX));
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
    static const int N_TABLES = 6;
    static const int DELTA_TABLE_4[N_TABLES][TABLE_SIZE];
    static const int STEP[16];
};
