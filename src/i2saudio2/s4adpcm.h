#pragma once

#include <stdint.h>
#include <limits.h>

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
    compressed data. The value of the delta (-7 to +7) scaled from 0 to 12
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
        int sign = 0;   // 0 or 8
        bool high = false;

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

    static void encode4(const int16_t* data, int32_t nSamples, uint8_t* compressed, State* state);
    static void decode4(const uint8_t* compressed, 
        int32_t nSamples,  
        int volume,         // 0-256 (higher values can overflow)
        bool add,           // if true, add to the 'data' buffer, else write to it
        int32_t* samples, State* state);

    static void encode8(const int16_t* data, int32_t nSamples, uint8_t* compressed, State* state);
    static void decode8(const uint8_t* compressed,
        int32_t nSamples,
        int volume,         // 0-256 (higher values can overflow)
        bool add,           // if true, add to the 'data' buffer, else write to it
        int32_t* samples, State* state);

private:
    static const int SHIFT_LIMIT_4 = 12;
    static const int SHIFT_LIMIT_8 = 8;
    static const int TABLE_SIZE = 9;

#ifdef S4ADPCM_OPT
public:
    static int DELTA_TABLE_4[TABLE_SIZE];
    static int DELTA_TABLE_8[TABLE_SIZE];
#else
    static const int DELTA_TABLE_4[TABLE_SIZE];
    static const int DELTA_TABLE_8[TABLE_SIZE];
#endif
};
