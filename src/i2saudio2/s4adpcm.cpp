#include "s4adpcm.h"

// 8 bit version:
// fiati:   mse=    460 -1 0 1 2 2 2 3 4
// poweron: mse=1056136 -1 0 1 2 3 3 3 4
// hum:     mse=     73 -1 0 1 2 2 2 3 4

// 4 bit version:
// fiati:   mse=  20122 -1 0 0 0 0 0 1 2
// poweron: mse=4349732 -1 0 0 1 1 1 2 3
// hum:     mse=   2437 -1 0 0 0 1 1 2 3

const int S4ADPCM::DELTA_TABLE_4[TABLE_SIZE] = {
    -1, 0, 0, 1, 1, 1, 2, 3
};

const int S4ADPCM::DELTA_TABLE_8[TABLE_SIZE] = {
    -1, 0, 1, 2, 2, 2, 3, 4
};

void S4ADPCM::encode4(const int16_t* data, int32_t nSamples, uint8_t* target, State* state)
{
    for (int i = 0; i < nSamples; ++i) {
        int value = data[i];
        int guess = state->guess();
        int delta = value - guess;

        uint8_t sign = 0;
        if (delta < 0) {
            sign = 8;
            delta = -delta;
        }

        delta >>= state->shift;
        if (delta > 7) delta = 7;

        if (state->high) {
            *target++ |= (delta | sign) << 4;
        }
        else {
            *target = (delta | sign);
        }
        state->high = !state->high;

        int p = guess + (delta << state->shift) * (sign ? -1 : 1);
        state->push(p);

        state->shift += DELTA_TABLE_4[delta];
        if (state->shift < 0) state->shift = 0;
        if (state->shift > 12) state->shift = 12;
    }
}


void S4ADPCM::encode8(const int16_t* data, int32_t nSamples, uint8_t* target, State* state)
{
    for (int i = 0; i < nSamples; ++i) {
        int value = data[i];
        int guess = state->guess();
        int delta = value - guess;

        uint8_t sign = 0;
        if (delta < 0) {
            sign = 0x80;
            delta = -delta;
        }

        delta >>= state->shift;
        if (delta > 127) delta = 127;
        *target++ = delta | sign;

        int p = guess + (delta << state->shift) * (sign ? -1 : 1);
        state->push(p);

        state->shift += DELTA_TABLE_8[delta >> 4];
        if (state->shift < 0) state->shift = 0;
        if (state->shift > 8) state->shift = 8;
    }
}


void S4ADPCM::decode4(const uint8_t* p, int32_t nSamples,
    int volume, bool add, int32_t* out, State* state)
{
    for (int32_t i = 0; i < nSamples; ++i) {
        uint8_t delta = 0;
        if (state->high) {
            delta = *p >> 4;
            p++;
        }
        else {
            delta = *p & 0xf;
        }
        int guess = state->guess();

        uint8_t sign = delta & 0x8;
        delta &= 0x7;
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

        state->shift += DELTA_TABLE_4[delta];
        if (state->shift < 0) state->shift = 0;
        if (state->shift > 12) state->shift = 12;
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
        if (state->shift > 8) state->shift = 8;
    }
}
