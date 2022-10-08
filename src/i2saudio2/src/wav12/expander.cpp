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
#include <algorithm>

using namespace wav12;

uint8_t ExpanderAD4::m_buffer[BUFFER_SIZE] = {0};

void ExpanderAD4::init(::IStream* stream, int _table)
{
    W12ASSERT(stream);
    m_stream = stream;
    m_table = _table;
    rewind();
}

void ExpanderAD4::rewind()
{
    m_state = S4ADPCM::State();
    m_stream->rewind();
}


int ExpanderAD4::expand(int32_t *target, uint32_t nSamples, int32_t volume, bool add, 
    const int* table, bool overrideEasing)
{
    if (!m_stream)
        return 0;

    if (overrideEasing) {
        m_state.volumeShifted = volume << 8;
    }

    uint32_t n = 0;

    while(n < nSamples) {
        int samplesWanted = std::min<int>(bytesToSamples(BUFFER_SIZE), nSamples - n);
        int bytesWanted = samplesToBytes(samplesWanted);
        uint32_t bytesFetched = m_stream->fetch(m_buffer, bytesWanted);
        uint32_t samplesFetched = bytesToSamples(bytesFetched);
        if (samplesFetched > nSamples - n)
            samplesFetched = nSamples - n;  // because 2 samples a byte. The last one can be zero.

        if (!bytesFetched)
            break;

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


void ExpanderAD4::fillBuffer(int32_t* buffer, int nBufferSamples, ExpanderAD4* expanders, int nExpanders, const bool* loop, const int *volume, bool disableEasing, const int* altTable)
{
    if (!buffer) return;
    if (nBufferSamples <= 0) return;

    for (int i = 0; i < nExpanders; ++i) {
        ExpanderAD4* expander = expanders + i;
        const int* table = altTable ? altTable : S4ADPCM::getTable(expander->table());

        int n = 0;
        do {
            // 32 bit stereo buffer
            // annoying and inefficent, but that's what is working.
            n += expander->expand(buffer + n * 2, nBufferSamples - n, volume[i], i > 0, table, disableEasing);
            if (loop[i] && expander->done())
                expander->rewind();
        } while (n < nBufferSamples && loop[i]);

        if (i == 0) {
            for (int j = n; j < nBufferSamples; ++j) {
                buffer[j * 2 + 0] = 0;
                buffer[j * 2 + 1] = 0;
            }
        }
    }

}

