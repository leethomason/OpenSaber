#include "compress.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

using namespace wav12;

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
            int nSamplesFetched = wmin(nSamples - i, m_bufferSize / 2);
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
    else {
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
}

uint32_t MemStream::fetch(uint8_t* buffer, uint32_t nBytes)
{
    uint32_t n = wmin(nBytes, m_size - m_pos);
    memcpy(buffer, m_data + m_pos, n);
    m_pos += n;
    return n;
}
