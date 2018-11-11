#include "compress.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

using namespace wav12;

template<class T>
T wav12Min(T a, T b) { return (a < b) ? a : b; }
template<class T>
T wav12Max(T a, T b) { return (a > b) ? a : b; }
template<class T>
T wav12Clamp(T x, T a, T b) {
	if (x < a) return a;
	if (x > b) return b;
	return x;
}

/*
    wav12 format=1 writes 16 samples in 24 bytes (75%)
    wav12 format=2 writes 16 samples in:
        12 bit header + 4 bits scale + 15 * 8 bits =
        2 bytes + 15 bytes = 17 bytes (53%)
 */

bool wav12::compress8(
    const int16_t* data, int32_t nSamples, 
    uint8_t** compressed, uint32_t* nCompressed, 
    float* errorFraction)
{
    static const int FRAME = 16;
    static const int DIV = 16;
    int scalingError = 0;
    int16_t samples12[FRAME];

    *compressed = new uint8_t[nSamples*2 + FRAME]; // more than needed
    //uint8_t* target = *compressed;
    uint8_t* p = *compressed;

    for(int i=0; i<nSamples; i += FRAME) {
        //memset(samples12, 0, sizeof(int16_t) * FRAME);
        for (int j = 0; j < FRAME && (i + j) < nSamples; ++j) {
            samples12[j] = data[i + j] / DIV;
        }
        // Pad the end w/ a repeating value so the delta will be 0.
        for (int j = nSamples - i; j < FRAME; ++j) {
            samples12[j] = data[nSamples - 1] / DIV;
        }
        
        // Scan frame.
        int maxInc = 0;
        int maxDec = 0;

        for (int j = 1; j < FRAME; ++j) {
            int d = samples12[j] - samples12[j - 1];
            if (d > 0)
                maxInc = wav12Max(maxInc, d);
            else
                maxDec = wav12Min(maxDec, d);
        }
        int scale = 1;
        // 2^12 = 4096
        // Jump from -2048 to 2047 is possible at 12 bits. (+4095)
        // Scale from 1-16, but it multiplies 127*16 = 2032
        // 
        int upScale = 1;
        int downScale = 1;
        while (maxInc > 127 * upScale)
            upScale++;
        while (maxDec < -128 * downScale)
            downScale++;

        scale = wav12Max(upScale, downScale);
        if (scale > 16) {
            delete[] *compressed;
            *compressed = 0;
            *nCompressed = 0;
            return false;
        }

        scalingError += scale - 1;

        // Encode!
        *p = uint8_t(data[i] >> 8);  // high bits of sample, in full 12 bits.
        p++;
        *p = uint8_t((data[i] & 0xff) >> 4) | ((scale - 1) << 4); // low bits
        p++;

        int current = samples12[0];
        for (int j = 1; j < FRAME; ++j) {
            int delta = samples12[j] - current;
            int deltaToWrite = delta / scale;
            deltaToWrite = wav12Clamp(deltaToWrite, -128, 127);
            int deltaPrime = deltaToWrite * scale;
            current += deltaPrime;
            assert(abs(current - samples12[j]) < 256 * scale);

            *p++ = (uint8_t)(int8_t(deltaToWrite));
        }
    }
    *errorFraction = float(scalingError * FRAME) / float(nSamples);
    *nCompressed = int32_t(p - *compressed);
    return true;
}


void wav12::compress(const int16_t* data, int32_t nSamples, uint8_t** compressed, uint32_t* nCompressed)
{
    *compressed = new uint8_t[nSamples*2];
    uint8_t* target = *compressed;

    for (int i = 0; i < nSamples; i += 2)
    {
        // Always use an even size, to avoid unaligned reads in the 16 bit section.
        int16_t v0 = data[i];
        int16_t v1 = (i + 1 < nSamples) ? data[i + 1] : 0;

        uint16_t v0u = uint16_t(v0);
        uint16_t v1u = uint16_t(v1);

        *target++ = (v0u >> 8) & 0xff;
        *target++ = (v0u & 0xf0) | ((v1u >> 12) & 0x0f);
        *target++ = (v1u >> 4) & 0xff;
    }
    size_t size = target - *compressed;
    *nCompressed = uint32_t(size);
}


Expander::Expander(uint8_t* buffer, uint32_t bufferSize)
{
    m_buffer = buffer;
    m_bufferSize = bufferSize;
    init(0, 0, 0);
}


void Expander::init(wav12::IStream* stream, uint32_t nSamples, int format)
{
    m_stream = stream;
    m_nSamples = nSamples;
    m_pos = 0;
    m_format = format;
}


void Expander::expand2(int32_t* target, uint32_t nSamples, int32_t volume)
{
    m_pos += nSamples;
    const int32_t* end = target + nSamples * 2;

    if (m_format == 0) {
        uint32_t i = 0;
        while (i < nSamples) {
            int nSamplesFetched = wav12Min(nSamples - i, m_bufferSize / 2);
            m_stream->fetch(m_buffer, nSamplesFetched * 2);

            int32_t* t = target + i * 2;
            const int16_t* src = (const int16_t*) m_buffer;
            for (int j = 0; j < nSamplesFetched; j++) {
                int32_t v = *src  * volume;
                ++src;
                *t++ = v;
                *t++ = v;
            }
            i += nSamplesFetched;
        }
    }
    else if (m_format == 1) {
        uint32_t i = 0;
        while (i < nSamples) {
            uint32_t samplesRemaning = nSamples - i;
            uint32_t samplesCanFetch = (m_bufferSize / 3) * 2;
            uint32_t fetched = 0;
            if (samplesRemaning <= samplesCanFetch) {
                // But there can be padding so that bytes % 3 always zero.
                fetched = (samplesRemaning + 1) & (~1);
            }
            else {
                // Normal case: 
                //  samplesCanFetch MULT 2 so that
                //  bytes MULT 3
                assert(samplesCanFetch % 2 == 0);
                fetched = samplesCanFetch;
            }
            uint32_t bytes = fetched * 3 / 2;
            assert(bytes % 3 == 0);
            m_stream->fetch(m_buffer, bytes);

            int32_t* t = target + i * 2;
            uint8_t* src = m_buffer;
            for (uint32_t j = 0; j < fetched; j += 2) {
                uint8_t s0 = *src++;
                uint8_t s1 = *src++;
                uint8_t s2 = *src++;
                int32_t v0 = int16_t((uint16_t(s0) << 8) | (uint16_t(s1 & 0xf0))) * volume;
                int32_t v1 = int16_t((uint16_t(s1 & 0x0f) << 12) | (uint16_t(s2) << 4)) * volume;
                *t++ = v0;
                *t++ = v0;
                if (t < end) {
                    *t++ = v1;
                    *t++ = v1;
                }
            }
            i += fetched;
        }
    }
    else if (m_format == 2) {
        uint32_t i = 0;
        static const int FRAME_SAMPLES = 16;
        static const int FRAME_BYTES = 17;
        // Align sample read to frame.
        const uint32_t nSamplesPadded = ((nSamples + FRAME_SAMPLES - 1) / FRAME_SAMPLES) * FRAME_SAMPLES;
        const uint32_t bufferCap = (m_bufferSize / FRAME_BYTES) * FRAME_SAMPLES;

        while (i < nSamplesPadded) {
            int nSamplesFetched = wav12Min(nSamplesPadded - i, bufferCap);
            m_stream->fetch(m_buffer, nSamplesFetched * FRAME_BYTES / FRAME_SAMPLES);

            int32_t* t = target + i * 2;
            const uint8_t* src = m_buffer;

            assert(nSamplesFetched % FRAME_SAMPLES == 0);
            const int nFrames = nSamplesFetched / FRAME_SAMPLES;

            for (int f = 0; f < nFrames; ++f) {
                assert(src < m_buffer + m_bufferSize);
                int32_t base = int8_t(*src) << 8;
                src++;
                base |= (*src & 0xf) << 4;
                int scale = (*src >> 4) + 1;
                assert(src < m_buffer + m_bufferSize);
                src++;

                assert(t < end);
                *t++ = base * volume;
                *t++ = base * volume;

                for (int j = 1; j < FRAME_SAMPLES; ++j) {
                    base += scale * int8_t(*src) * 16;
                    src++;
                    assert(t < end);
                    *t++ = base * volume;
                    *t++ = base * volume;
                    if (t == end) break;
                }
            }
            i += nSamplesFetched;
        }
    }
    else {
        assert(false); // bad format
    }
}

uint32_t MemStream::fetch(uint8_t* buffer, uint32_t nBytes)
{
    uint32_t n = wav12Min(nBytes, m_size - m_pos);
    memcpy(buffer, m_data + m_pos, n);
    m_pos += n;
    return n;
}
