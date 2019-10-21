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

void ExpanderAD4::init(IStream* stream)
{
    m_stream = stream;
    rewind();
}

void ExpanderAD4::rewind()
{
    m_state = S4ADPCM::State();
    m_stream->rewind();
}

bool ExpanderAD4::done() const { 
    ASSERT(m_stream); 
    return m_stream->done(); 
}

void ExpanderAD4::compress(const int16_t* data, int32_t nSamples, 
    uint8_t** compressed, uint32_t* nCompressed)
{
    *nCompressed = (nSamples + 1) / 2;
    *compressed = new uint8_t[nSamples];

    S4ADPCM::State state;
    S4ADPCM::encode(data, nSamples, *compressed, &state);
}


int ExpanderAD4::expand(int32_t *target, uint32_t nSamples, int32_t volume, bool add)
{
    if (!m_stream)
        return 0;

    uint32_t n = 0;

    while (n < nSamples) {
        // Rembering that everything but the last call n should be even.
        uint32_t nBytes = (nSamples - n + 1) / 2;
        uint32_t bytesFetched = m_stream->fetch(m_buffer, wav12Min(nBytes, uint32_t(BUFFER_SIZE)));
        if (bytesFetched == 0)
            break;

        uint32_t ns = bytesFetched * 2;
        if (ns > (nSamples - n))
            ns = nSamples - n;

        S4ADPCM::decode(m_buffer, ns, volume, add, target + n, &m_state);
        n += ns;
    }
    return n;
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
