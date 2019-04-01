#include "compress.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

using namespace wav12;

static const int FRAME_SAMPLES = 16;
static const int FRAME_BYTES = 17;


bool wav12::compressVelocity(const int16_t* data, int32_t nSamples, uint8_t** compressed, uint32_t* nCompressed)
{
    Velocity vel;

    *compressed = new uint8_t[nSamples*2];
    uint8_t* target = *compressed;
    bool hasPrevBits = false;

    for (int i = 0; i < nSamples; i++)
    {
        int value = data[i] / 16;
        int guess = vel.guess();
        int delta = value - guess;

        if (hasPrevBits && delta < 512 && delta >= -512) {
            static const int BIAS = 512;
            uint32_t bits = delta + BIAS;
            uint8_t low7 = (bits & 0x7f);
            uint8_t high3 = (bits & 0x380) >> 7;
            assert((high3 << 7 | low7) == bits);
            *target = low7 | 0x80;
            assert((*(target - 1) & 0xe0) == 0);    // make sure high 3 are empty
            *(target - 1) |= (high3 << 5);
            target++;
            hasPrevBits = false;
        }
        else if (delta < 64 && delta >= -64) {
            assert(!hasPrevBits);
            static const int BIAS = 64;
            uint8_t bits = delta + BIAS;
            *target++ = bits | 0x80;
            hasPrevBits = false;
        }
        else {
            assert(value < 2048 && value >= -2048);
            static const int BIAS = 2048;
            int32_t bits = BIAS + value;
            uint32_t low7 = bits & 0x07f;
            uint32_t high5 = (bits & 0xf80) >> 7;

            *target++ = low7;
            *target++ = high5;
            hasPrevBits = true;
        }
        vel.push(value);
    }
    size_t size = target - *compressed;
    *nCompressed = uint32_t(size);

    return false;
}


void ExpanderV::init(IStream* stream, uint32_t nSamples, int format)
{
    assert(format == 3);
    m_stream = stream;
    m_done = false;
}


void ExpanderV::rewind()
{
    m_bufferEnd = m_bufferStart = 0;
    memset(m_buffer, 0, BUFFER_SIZE);
    m_vel = Velocity();
    m_done = false;
    m_stream->rewind();
}


void ExpanderV::fetch()
{
    // Each sample is one or two bytes.
    uint32_t read = 0;
    if (m_bufferStart < m_bufferEnd) {
        // In this case, we have the 1st byte of a pair, but
        // not the 2nd. So the 2nd has to be there, and
        // we can't be done.
        assert(m_bufferStart == m_bufferEnd - 1);
        assert(m_bufferStart > 0);
        m_buffer[0] = m_buffer[m_bufferStart];
        read = m_stream->fetch(m_buffer+1, BUFFER_SIZE - 1);
        assert(read > 0);
        m_bufferEnd = read + 1;
            }
            else {
        // We were on a sample boundary, so read as much as possible.
        read = m_stream->fetch(m_buffer, BUFFER_SIZE);
        m_bufferEnd = read;
                }
    m_bufferStart = 0;
    if (read == 0) {
        m_done = true;
            }
        }

void ExpanderV::expand(int32_t* target, uint32_t nSamples, int32_t volume, bool add)
{
    for (uint32_t i = 0; i < nSamples; ++i) {
        if (!hasSample()) {
            fetch();
            if (done())
                return;
        }
        const uint8_t* src = m_buffer + m_bufferStart;
        const int32_t guess = m_vel.guess();
        int32_t value = 0;

        if (src[0] & 0x80) {
            int32_t low7 = src[0] & 0x7f;

            if (m_hasHigh3) {
                static const int32_t BIAS = 512;
                int32_t bits = ((m_high3 << 7) | low7);
                int32_t delta = bits - BIAS;
                value = guess + delta;
    }
    else {
                static const int32_t BIAS = 64;
                int32_t delta = low7 - BIAS;
                value = guess + delta;
            }
            m_hasHigh3 = false;
            m_bufferStart++;
        }
        else {
            // Stored as: low7 high5
            static const int32_t BIAS = 2048;
            m_high3 = (src[1] & 0xe0) >> 5; // save for later

            int32_t low7 = src[0] & 0x7f;
            int32_t high5 = (src[1] & 0x1f);
            int32_t bits = low7 | (high5 << 7);
            value = bits - BIAS;

            m_hasHigh3 = true;
            m_bufferStart += 2;
        }
        m_vel.push(value);
        int32_t s = value * volume * 16;
        if (add) {
            target[0] += s;
            target[1] = target[0];
        }
        else {
            target[0] = s;
            target[1] = s;
        }
        target += 2;
    }
}

MemStream::MemStream(const uint8_t* data, uint32_t size) {
    m_data = data;
    m_size = size;
    m_pos = 0;
}


void MemStream::set(uint32_t addr, uint32_t size) {
    assert(addr == 0);  // just not implemented
    assert(size == this->m_size);
    m_pos = 0;
}


void MemStream::rewind() {
    m_pos = 0;
}


uint32_t MemStream::fetch(uint8_t* buffer, uint32_t nBytes)
{
    uint32_t n = wav12Min(nBytes, m_size - m_pos);
    memcpy(buffer, m_data + m_pos, n);
    m_pos += n;
    return n;
}
