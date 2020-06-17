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

#ifdef _WIN32
#include <assert.h>
#define ASSERT assert
#endif

const int S4ADPCM::DELTA_TABLE_4[N_TABLES][TABLE_SIZE] = {
    {-1, 0, 0, 0, 0, 1, 1, 2, 2},
    {-1, 0, 0, 1, 1, 1, 2, 3, 3},
    {-1, 0, 0, 1, 1, 2, 3, 4, 4},
    {-1, -1, 0, 1, 2, 3, 4, 5, 5}
};

const int S4ADPCM::DELTA_TABLE_8[N_TABLES][TABLE_SIZE] = {
    {-1, 0, 1, 2, 3, 3, 3, 3, 4 },
    {-1, 0, 1, 2, 3, 4, 4, 4, 4},
    {-1, 0, 1, 2, 3, 4, 4, 4, 5},
    {-1, 0, 1, 2, 3, 4, 5, 6, 6},
};

void S4ADPCM::encode8(const int16_t* data, int32_t nSamples, uint8_t* target, State* state, const int* table, Error* pError)
{
    for (int i = 0; i < nSamples; ++i) {
        int value = data[i];
        int guess = state->guess();
        int error = value - guess;

        // Bias up so we round up and down equally.
        int mult = 1 << state->shift;                
        int bias = error >= 0 ? mult / 2 : -mult / 2;
        int delta = (error + bias) / mult;

        if (delta > 127) delta = 127;
        if (delta < -128) delta = -128;

        while (guess + (delta << state->shift) > SHRT_MAX)
            --delta;
        while (guess + (delta << state->shift) < SHRT_MIN)
            ++delta;

        assert(delta >= -128 && delta <= 127);

        *target++ = int8_t(delta);

        int p = guess + (delta << state->shift);
#ifdef _WIN32
        assert(p >= SHRT_MIN && p <= SHRT_MAX);
#endif
        state->push(p);

        if (pError) {
            pError->e16squared += calcError(value, p);
            assert(pError->e16squared >= 0);    // check for overflow
            pError->set(value - p);
        }

        int dTable = (delta >= 0 ? delta : -delta) >> 4;
        state->shift += table[dTable];
        if (state->shift < 0) state->shift = 0;
        if (state->shift > SHIFT_LIMIT_8) state->shift = SHIFT_LIMIT_8;
    }
}


int S4ADPCM::encode4(const int16_t* data, int32_t nSamples, uint8_t* target, State* state, const int* table, Error* pError)
{
    const uint8_t* start = target;
    for (int i = 0; i < nSamples; ++i) {
        int value = data[i];
        int guess = state->guess();
        int error = value - guess;

        // Bias up so we round up and down equally.
        int mult = 1 << state->shift;
        int bias = (error >= 0) ? (mult / 2) : -(mult / 2);
        int delta = (error + bias) / mult;

        if (delta > 7) delta = 7;
        if (delta < -8) delta = -8;

        while (guess + (delta << state->shift) > SHRT_MAX)
            --delta;
        while (guess + (delta << state->shift) < SHRT_MIN)
            ++delta;

        assert(delta >= -8 && delta <= 7);

        uint8_t d = (delta >= 0) ? uint8_t(delta) : uint8_t(delta & 0x0f);
        assert((d & 0xf0) == 0);

        if (state->high)
            *target++ |= d << 4;
        else
            *target = d;

        state->high = !state->high;

        int p = guess + (delta << state->shift);
#ifdef _MSC_VER
        ASSERT(p >= SHRT_MIN && p <= SHRT_MAX);
#endif

        state->push(p);

        if (pError) {
            pError->e16squared += calcError(value, p);
            assert(pError->e16squared >= 0);    // check for overflow
            pError->set(value - p);
        }

        state->shift += table[delta >= 0 ? delta : -delta];
        if (state->shift < 0) state->shift = 0;
        if (state->shift > SHIFT_LIMIT_4) state->shift = SHIFT_LIMIT_4;
    }
    if (state->high) target++;
    return int(target - start);
}

void S4ADPCM::decode4(const uint8_t* p, int32_t nSamples,
    int volume,
    bool add,
    int32_t* out, State* state, const int* table)
{
    state->volumeTarget = volume << 8;

    for (int32_t i = 0; i < nSamples; ++i) {
        uint8_t d = 0;
        if (state->high) {
            d = *p >> 4;
            p++;
        }
        else {
            d = *p & 0xf;
        }
        int8_t delta = int8_t(d << 4) >> 4;

        int guess = state->guess();
        int value = guess + (delta << state->shift);

#ifdef _WIN32
        if (value < SHRT_MIN || value > SHRT_MAX) {
            assert(false);
        }
#endif
        state->push(value);

        if (state->volumeShifted < state->volumeTarget)
            state->volumeShifted += VOLUME_EASING;
        else if (state->volumeShifted > state->volumeTarget)
            state->volumeShifted -= VOLUME_EASING;

        int32_t s = scaleVol(value, state->volumeShifted);
        if (add)
            out[0] = out[1] = sat_add(s, out[0]);
        else
            out[0] = out[1] = s;
        out += 2;

        state->shift += table[delta >= 0 ? delta : -delta];
        if (state->shift < 0) state->shift = 0;
        if (state->shift > SHIFT_LIMIT_4) state->shift = SHIFT_LIMIT_4;
        state->high = !state->high;
    }
}

void S4ADPCM::decode8(const uint8_t* p, int32_t nSamples,
    int volume, bool add, int32_t* out, State* state, const int* table)
{
    state->volumeTarget = volume << 8;

    for (int32_t i = 0; i < nSamples; ++i) {
        int delta = int8_t(*p++);

        int guess = state->guess();
        int value = guess + (delta << state->shift);

#ifdef _WIN32
        if (value < SHRT_MIN || value > SHRT_MAX) {
            assert(false);
        }
#endif
        state->push(value);

        if (state->volumeShifted < state->volumeTarget)
            state->volumeShifted += VOLUME_EASING;
        else if (state->volumeShifted > state->volumeTarget)
            state->volumeShifted -= VOLUME_EASING;

        int32_t s = scaleVol(value, state->volumeShifted);
        if (add) {
            out[0] = out[1] = sat_add(s, out[0]);
        }
        else {
            out[0] = out[1] = s;
        }
        out += 2;

        int dTable = (delta >= 0 ? delta : -delta) >> 4;
        state->shift += table[dTable];
        if (state->shift < 0) state->shift = 0;
        if (state->shift > SHIFT_LIMIT_8) state->shift = SHIFT_LIMIT_8;
    }
}
