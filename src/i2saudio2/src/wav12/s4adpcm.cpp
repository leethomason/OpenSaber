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

#include "s4adpcm.h"

// The DELTA_TABLE and STEP are dependant. It
// was a half day of testing to settle on these
// values. The tables are similar to what was 
// before, although the perhaps are a little
// more conservative. A step range of +/-16 seems
// to work out well.
const int S4ADPCM::DELTA_TABLE_4[N_TABLES][TABLE_SIZE] = {
    {-1,  0, 0, 0, 0, 1, 1, 2, 2},   // 0 default (though not as good, generally)

    // discovered (v2)
    {-1, -1, -1, 0, 0, 0, 1, 2, 2}, // hum01, swinh01
    {-1, 0, 0, 0, 0, 0, 1, 1, 2},   // out01, fiati

    // discovered (v1)
    {-1,  0, 0, 0, 0, 0, 1, 1, 1},  // in01, out01
    {-1,  0, 0, 0, 0, 0, 1, 2, 2},  // fiati, swingh01, swingl01, in02
    {-1, -1, 0, 0, 0, 1, 2, 3, 4}   // hum01
};

// -8 + 7
const int S4ADPCM::STEP[16] = {
#if 0
    // This is the previous algorithm.
    // Still *slightly* more accurate, since the best
    // value is searched for.
    // TotalError = 3180  SimpleError = 197,499
    -8, -7, -6, -5, -4, -3, -2, -1,
    0, 1, 2, 3, 4, 5, 6, 7
#endif
#if 1
    // A little flare on the ends seems to work well.
    // TotalError = 1019  SimpleError = 61187
    -16, -12, -8, -6, -4, -3, -2, -1,
    0, 1, 2, 3, 4, 6, 8, 16
#endif
#if 0
    // Linear range. Doesn't help (as expected) as
    // it is just a bigger multiplier.
    // TotalError = 1211  SimpleError = 70258
    -24, -21, -18, -15, -12, -9, -6, -3,
    0, 3, 6, 9, 12, 15, 18, 24
#endif
#if 0
    // 24 bit flare is a too much. (But still pretty good.)
    // 32 bit is much too much.
    // TotalError = 1118  SimpleError = 64919
    -24, -16, -14, -10, -8, -4, -2, -1,
    0, 1, 2, 4, 8, 12, 16, 24
#endif
};


int S4ADPCM::encode4(const int16_t* data, int32_t nSamples, uint8_t* target, State* state, const int* table)
{
    W12ASSERT(STEP[ZERO_INDEX] == 0);
    W12ASSERT((nSamples & 1) == 0);     // even number. not sure the odd is handled?

    const uint8_t* start = target;
    for (int i = 0; i < nSamples; ++i) {
        const int32_t value = data[i];
        const int32_t guess = state->guess();
        const int32_t mult = 1 << state->shift;

        // Search for minimum error.
        int bestE = INT_MAX;
        uint8_t index = ZERO_INDEX;
        for (int j = 0; j < 16; ++j) {
            int32_t s = guess + mult * STEP[j];
            int32_t e = abs(s - value);

            if (e < bestE) {
                bestE = e;
                index = j;
                if (e == 0) break;
            }
        }
        if (state->high)
            *target++ |= index << 4;
        else
            *target = index;

        int32_t p = guess + STEP[index] * mult;
        state->push(p);

        state->doShift(table, index);
        state->high = !state->high;
    }
    if (state->high) target++;
    return int(target - start);
}

void S4ADPCM::decode4(const uint8_t* p, int32_t nSamples,
    int32_t volume,
    bool add,
    int32_t* out, State* state, const int* table)
{
    W12ASSERT(STEP[ZERO_INDEX] == 0);
    W12ASSERT((nSamples & 1) == 0);     // even number. not sure the odd is handled?
    state->volumeTarget = volume << 8;

    for (int32_t i = 0; i < nSamples; ++i) {
        uint8_t index = 0;
        if (state->high) {
            index = *p >> 4;
            p++;
        }
        else {
            index = *p & 0xf;
        }
        int32_t mult = 1 << state->shift;
        int32_t guess = state->guess();
        int32_t value = guess + STEP[index] * mult;

        state->push(value);

        // The volumeShifted = 256 * volume.
        // So we can add/subtract 32 (the EASING) and know we won't 
        // zig-zag back and forth over the target value.
        if (state->volumeShifted < state->volumeTarget)
            state->volumeShifted += VOLUME_EASING;
        else if (state->volumeShifted > state->volumeTarget)
            state->volumeShifted -= VOLUME_EASING;

        int32_t valueClipped = std::min(std::max(value, int32_t(SHRT_MIN)), int32_t(SHRT_MAX));
        int32_t s = sat_mult(valueClipped, state->volumeShifted);
        out[0] = out[1] = add ? sat_add(s, out[0]) : s;
        out += 2;

        state->doShift(table, index);
        state->high = !state->high;
    }
}
