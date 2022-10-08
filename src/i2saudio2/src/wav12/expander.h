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

#ifndef WAV_COMPRESSION
#define WAV_COMPRESSION

#include "s4adpcm.h"
#include "interface.h"

#include <stdint.h>
#include <string.h>
#include <limits.h>

namespace wav12 {

    class ExpanderAD4
    {
    public:
        static const int BUFFER_SIZE = 128;

        ExpanderAD4() {}
        void init(IStream* stream, int _table);

        // Returns the number of samples it could expand. nSamples should be even,
        // unless it is the last sample (which can be odd if it uses up the
        // entire track.)
        int expand(int32_t* target, uint32_t nSamples, int32_t volume, bool add, const int* table, bool overrideEasing);
        void rewind();
        bool done() const { return m_stream->done(); }
        int table() const { return m_table; }

        static void generateTestData(int nSamples, int16_t* data);

        // Exact; but there can be extra. For 7 samples, 4 bytes are needed, but a nibble is unused.
        static int samplesToBytes(int n) {
            return (n + 1) / 2;
        }

        // Exact; returns the possible number of samples. (Could be one less.)
        static int bytesToSamples(int b) {
            return b * 2;
        }

        static void fillBuffer(int32_t* buffer, int bufferSamples, ExpanderAD4* expanders, int nExpanders, const bool* loop, const int* volume, bool disableEasing, const int* altTable=0);

    private:
        static uint8_t m_buffer[BUFFER_SIZE];
        IStream* m_stream = 0;
        S4ADPCM::State m_state;
        int m_table = 0;
    };
}
#endif
    