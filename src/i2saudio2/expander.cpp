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

#include "expander.h"

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

using namespace wav12;

uint8_t ExpanderAD4::m_buffer[BUFFER_SIZE] = {0};

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
    uint8_t* compressed, uint32_t* nCompressed, const int* table, int32_t* aveErrSquared)
{
    S4ADPCM::State state;
    *nCompressed = S4ADPCM::encode4(data, nSamples, compressed, &state, table, aveErrSquared);
}

void ExpanderAD4::compress8(const int16_t* data, int32_t nSamples,
    uint8_t* compressed, uint32_t* nCompressed, const int* table, int32_t* aveErrSquared)
{
    *nCompressed = nSamples;
    S4ADPCM::State state;
    S4ADPCM::encode8(data, nSamples, compressed, &state, table, aveErrSquared);
}


int ExpanderAD4::expand(int32_t *target, uint32_t nSamples, int32_t volume, bool add, 
    int codec, const int* table, bool overrideEasing)
{
    if (!m_stream)
        return 0;

    if (overrideEasing) {
        m_state.volumeShifted = volume << 8;
    }

    uint32_t n = 0;

    while(n < nSamples) {
        int samplesWanted = wav12Min<int>(bytesToSamples(BUFFER_SIZE, codec), nSamples - n);
        int bytesWanted = samplesToBytes(samplesWanted, codec);
        uint32_t bytesFetched = m_stream->fetch(m_buffer, bytesWanted);
        uint32_t samplesFetched = bytesToSamples(bytesFetched, codec);
        if (samplesFetched > nSamples - n)
            samplesFetched = nSamples - n;  // because 2 samples a byte. The last one can be zero.

        if (!bytesFetched)
            break;

        if (codec == S4ADPCM_8BIT)
            S4ADPCM::decode8(m_buffer, samplesFetched, volume, add, target + intptr_t(n) * 2, &m_state, table);
        else
            S4ADPCM::decode4(m_buffer, samplesFetched, volume, add, target + intptr_t(n) * 2, &m_state, table);
        n += samplesFetched;
    }
    return n;
}


void ExpanderAD4::generateTestData(int nSamples, int16_t* data)
{
    static const int32_t FREQ = 22050;
    static const int32_t WAVE0 = 440;   // A4
    //static const int32_t WAVE1 = 330;   // E3

    static const int RANGE = 16000;
    static const int NAPPROX = 64;
    static const int SIN[32] = {
        0, 1568, 3121, 4644, 6122, 7542, 8889, 10150, 11313, 12368, 13303, 14110, 14782, 15311, 15692, 15922,
        16000, 15922, 15692, 15311, 14782, 14110, 13303, 12368, 11313, 10150, 8889, 7542, 6122, 4644, 3121, 1568
    };

    for (int i = 0; i < nSamples; ++i) {
        int index = (i * NAPPROX * WAVE0 / FREQ) % NAPPROX;
        if (index < 32)
            data[i] = SIN[index];
        else
            data[i] = -SIN[index - 32];
    }
}
