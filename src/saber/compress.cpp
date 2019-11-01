#include "compress.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#ifdef _MSC_VER
#include <assert.h>
#define ASSERT assert
#else
#define ASSERT
#endif

using namespace wav12;

// 8 bit version:
// fiati:   mse=460     -1 0 1 2 2 2 3 4
// poweron: mse=1056136 -1 0 1 2 3 3 3 4
// hum:     mse=73      -1 0 1 2 2 2 3 4

// 4 bit version:
// poweron: mse=4349732 -1 0 0 1 1 1 2 3
// hum:     mse=2437    -1 0 0 0 1 1 2 3
// fiati:   mse=20122   -1 0 0 0 0 0 1 2
#ifndef TUNE_MODE
const 
#endif
int ExpanderAD4::DELTA[TABLE_SIZE] = {
    -1, 0, 0, 1, 1, 1, 2, 3
};

void ExpanderAD4::attach(IStream* stream)
{
    m_stream = stream;
    rewind();
}

void ExpanderAD4::rewind()
{
    m_vel = Velocity();
    m_shift = 1;
    m_high = true;
    m_stream->rewind();
}

bool ExpanderAD4::done() const
{
    return m_stream->done();
}


void ExpanderAD4::compress(const int16_t* data, int32_t nSamples, uint8_t** compressed, uint32_t* nCompressed)
{
    Velocity vel;
    *nCompressed = (nSamples + 1) / 2;
    uint8_t* target = *compressed = new uint8_t[nSamples];

    int shift = 1;
    bool high = true;

    for (int i = 0; i < nSamples; ++i) {
        int value = data[i];
        int guess = vel.guess();
        int delta = value - guess;

        uint8_t sign = 0;
        if (delta < 0) {
            sign = 8;
            delta = -delta;
        }

        delta >>= shift;
        if (delta > 7) delta = 7;

        if (high) {
            *target = (delta | sign) << 4;
        }
        else {
            *target++ |= (delta | sign);
        }
        high = !high;

        int p = guess + (delta << shift) * (sign ? -1 : 1);
        vel.push(p);

        shift += DELTA[delta];
        if (shift < 0) shift = 0;
        if (shift > 12) shift = 12;
    }
}


int ExpanderAD4::expand(int32_t *target, uint32_t nSamples, int32_t volume16, bool add)
{
    if (!m_stream)
        return 0;

    int mult = add ? 1 : 0;
    uint32_t n = 0;

    while (n < nSamples) {
        // Rembering that everything but the last call n should be even.
        uint32_t nBytes = (nSamples - n + 1) / 2;
        uint32_t bytesFetched = m_stream->fetch(m_buffer, wav12Min(nBytes, uint32_t(BUFFER_SIZE)));
        const uint8_t* p = m_buffer;

        uint32_t ns = bytesFetched * 2;
        if (ns > (nSamples - n))
            ns = nSamples - n;

        for (uint32_t i = 0; i < ns; ++i) {
            uint8_t delta = 0;
            if (m_high) {
                delta = *p >> 4;
            }
            else {
                delta = *p & 0xf;
                p++;
            }
            int guess = m_vel.guess();

            uint8_t sign = delta & 0x8;
            delta &= 0x7;
            int value = 0;
            if (sign) {
                value = guess - (delta << m_shift);
            }
            else {
                value = guess + (delta << m_shift);
            }
            m_vel.push(value);

            int32_t s = value * volume16;
            target[0] = target[1] = (target[0] * mult) + s;
            target += 2;

            m_shift += DELTA[delta];
            if (m_shift < 0) m_shift = 0;
            if (m_shift > 12) m_shift = 12;
            m_high = !m_high;
        }
        n += ns;
    }
    return nSamples;
}


MemStream::MemStream(const uint8_t *data, uint32_t size)
{
    m_data = data;
    m_data_end = data + size;
    m_addr = 0;
    m_size = 0;
    m_pos = 0;
}

void MemStream::set(uint32_t addr, uint32_t size)
{
    m_addr = addr;
    m_size = size;
    m_pos = 0;
    ASSERT(m_data + m_addr + m_size <= m_data_end);
}

void MemStream::rewind()
{
    m_pos = 0;
}

uint32_t MemStream::fetch(uint8_t *buffer, uint32_t nBytes)
{
    if (m_pos + nBytes > m_size)
        nBytes = m_size - m_pos;

    memcpy(buffer, m_data + m_addr + m_pos, nBytes);
    m_pos += nBytes;
    return nBytes;
}
