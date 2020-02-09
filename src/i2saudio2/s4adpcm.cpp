#include "s4adpcm.h"

#ifdef _MSC_VER
#include <assert.h>
#define ASSERT assert
#endif

#ifndef S4ADPCM_OPT
const
#endif
int S4ADPCM::DELTA_TABLE_4[TABLE_SIZE] = {
    // The last entry is used for the special delta=8 case
    -1, 0, 0, 0, 1, 1, 1, 2, 3
};

#ifndef S4ADPCM_OPT
const
#endif
int S4ADPCM::DELTA_TABLE_8[TABLE_SIZE] = {
    // 8-bit version doesn't use the last entry
    -1, 0, 1, 2, 3, 3, 4, 4, 4  
};

void S4ADPCM::encode4(const int16_t* data, int32_t nSamples, uint8_t* target, State* state)
{
    for (int i = 0; i < nSamples; ++i) {
        int value = data[i];
        int guess = state->guess();
        int error = value - guess;
        int positiveGuess = guess;

        uint8_t sign = 0;
        if (error < 0) {
            sign = 8;
            error = -error;
            positiveGuess = -guess;
        }
#ifdef _MSC_VER
        ASSERT(positiveGuess >= SHRT_MIN && positiveGuess <= SHRT_MAX);
#endif
        // Bias up so we round up and down equally.
        // Error always positive, delta always positive.
        int bias = ((1 << state->shift) - 1)/2;
        int delta = (error + bias) >> state->shift;

        int maxDelta = 7;
        if (state->sign == sign)
            maxDelta = 8;
        if (delta > maxDelta)
            delta = maxDelta;

        if ((positiveGuess + (delta << state->shift)) > SHRT_MAX) {
            delta = (SHRT_MAX - positiveGuess) >> state->shift;
            if (delta > maxDelta) 
                delta = maxDelta;
        }

#ifdef _MSC_VER
        if (positiveGuess + (delta << state->shift) > SHRT_MAX) {
            ASSERT(false);
        }
#endif
        if (delta == 8) {
            if (state->high)
                *target++ |= (8 << 4);
            else
                *target = 8;
        }
        else if (delta == 0) {
            if (state->high)
                *target++ |= 0;
            else
                *target = 0;
        }
        else {
            state->sign = sign;
            if (state->high)
                *target++ |= (delta | sign) << 4;
            else
                *target = (delta | sign);
        }

        state->high = !state->high;

        int p = guess + (delta << state->shift) * (sign ? -1 : 1);
#ifdef _MSC_VER
        ASSERT(p >= SHRT_MIN && p <= SHRT_MAX);
#endif
        state->push(p);

        state->shift += DELTA_TABLE_4[delta];
        if (state->shift < 0) state->shift = 0;
        if (state->shift > SHIFT_LIMIT_4) state->shift = SHIFT_LIMIT_4;
    }
}


void S4ADPCM::encode8(const int16_t* data, int32_t nSamples, uint8_t* target, State* state)
{
    for (int i = 0; i < nSamples; ++i) {
        int value = data[i];
        int guess = state->guess();
        int error = value - guess;
        int positiveGuess = guess;

        uint8_t sign = 0;
        if (error < 0) {
            sign = 0x80;
            error = -error;
            positiveGuess = -positiveGuess;
        }
#ifdef _MSC_VER
        ASSERT(positiveGuess >= SHRT_MIN && positiveGuess <= SHRT_MAX);
#endif

        // Bias up so we round up and down equally.
        // Error always positive, delta always positive.
        int bias = ((1 << state->shift) - 1) / 2;
        int delta = (error + bias) >> state->shift;
        if (delta > 127) delta = 127;

        if ((positiveGuess + (delta << state->shift)) > SHRT_MAX) {
            delta = (SHRT_MAX - positiveGuess) >> state->shift;
            if (delta > 127) delta = 127;
        }

        *target++ = delta | sign;

        int p = guess + (delta << state->shift) * (sign ? -1 : 1);
        state->push(p);

        state->shift += DELTA_TABLE_8[delta >> 4];
        if (state->shift < 0) state->shift = 0;
        if (state->shift > SHIFT_LIMIT_8) state->shift = SHIFT_LIMIT_8;
    }
}


void S4ADPCM::decode4(const uint8_t* p, int32_t nSamples,
    int volume, bool add, int32_t* out, State* state)
{
    uint8_t data = 0;
    uint8_t sign = 0;
    int delta = 0;
    int value = 0;

    for (int32_t i = 0; i < nSamples; ++i) {
        if (state->high) {
            data = *p >> 4;
            p++;
        }
        else {
            data = *p & 0xf;
        }

        if (data == 8) {
            sign = state->sign;
            delta = 8;
        }
        else if (data == 0) {
            sign = state->sign;
            delta = 0;
        }
        else {
            sign = data & 8;
            delta = data & 0x7;
        }

        int guess = state->guess();
        value = sign ? (guess - (delta << state->shift))
                     : (guess + (delta << state->shift));

#ifdef _MSC_VER
        if (value < SHRT_MIN || value > SHRT_MAX) {
            ASSERT(false);
        }
#endif
        state->push(value);

        int32_t s = value * (volume << 8);
        out[0] = out[1] = add ? (out[0] + s) : s;
        out += 2;

        state->sign = sign;
        state->shift += DELTA_TABLE_4[delta];
        if (state->shift < 0) state->shift = 0;
        if (state->shift > SHIFT_LIMIT_4) state->shift = SHIFT_LIMIT_4;
        state->high = !state->high;
    }
}


void S4ADPCM::decode8(const uint8_t* p, int32_t nSamples,
    int volume, bool add, int32_t* out, State* state)
{
    for (int32_t i = 0; i < nSamples; ++i) {
        int delta = *p & 0x7f;
        uint8_t sign = *p & 0x80;
        p++;

        int guess = state->guess();
        int value = 0;
        if (sign) {
            value = guess - (delta << state->shift);
        }
        else {
            value = guess + (delta << state->shift);
        }
        state->push(value);

        int32_t s = value * (volume << 8);
        if (add) {
            out[0] = out[1] = out[0] + s;
        }
        else {
            out[0] = out[1] = s;
        }
        out += 2;

        state->shift += DELTA_TABLE_8[delta >> 4];
        if (state->shift < 0) state->shift = 0;
        if (state->shift > SHIFT_LIMIT_8) state->shift = SHIFT_LIMIT_8;
    }
}
