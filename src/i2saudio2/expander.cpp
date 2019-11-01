#include "expander.h"

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

using namespace wav12;

uint8_t ExpanderAD4::m_buffer[BUFFER_SIZE] = {0};

// 8 bit version:
// fiati:   mse=    460 -1 0 1 2 2 2 3 4
// poweron: mse=1056136 -1 0 1 2 3 3 3 4
// hum:     mse=     73 -1 0 1 2 2 2 3 4

// 4 bit version:
// fiati:   mse=  20122 -1 0 0 0 0 0 1 2
// poweron: mse=4349732 -1 0 0 1 1 1 2 3
// hum:     mse=   2437 -1 0 0 0 1 1 2 3

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

void ExpanderAD4::compress4(const int16_t* data, int32_t nSamples, 
    uint8_t** compressed, uint32_t* nCompressed)
{
    *nCompressed = (nSamples + 1) / 2;
    *compressed = new uint8_t[nSamples];

    S4ADPCM::State state;
    S4ADPCM::encode4(data, nSamples, *compressed, &state);
}


void ExpanderAD4::compress8(const int16_t* data, int32_t nSamples,
    uint8_t** compressed, uint32_t* nCompressed)
{
    *nCompressed = nSamples;
    *compressed = new uint8_t[nSamples];

    S4ADPCM::State state;
    S4ADPCM::encode8(data, nSamples, *compressed, &state);
}


int ExpanderAD4::expand(int32_t *target, uint32_t nSamples, int32_t volume, bool add, bool use8Bit)
{
    if (!m_stream)
        return 0;

    uint32_t n = 0;

    while(n < nSamples) {
        uint32_t samplesWanted = nSamples - n;
        uint32_t bytesWanted = use8Bit ? samplesWanted : (samplesWanted+1) / 2;
        uint32_t bytesFetched = m_stream->fetch(m_buffer, wav12Min(bytesWanted, uint32_t(BUFFER_SIZE)));
        uint32_t samplesFetched = use8Bit ? bytesFetched : bytesFetched * 2;
        if (samplesFetched > samplesWanted) {
            // edge case of odd number of samples.
            samplesFetched = samplesWanted;
        }

        if (bytesFetched) {
            if (use8Bit)
                S4ADPCM::decode8(m_buffer, samplesFetched, volume, add, target + n * 2, &m_state);
            else
                S4ADPCM::decode4(m_buffer, samplesFetched, volume, add, target + n * 2, &m_state);
        }
        else {
            break;
        }
        n += samplesFetched;
    }

    return n;
}

