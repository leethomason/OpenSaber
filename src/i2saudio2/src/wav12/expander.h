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

        ExpanderAD4() : m_state(nullptr, 0) {}
        void init(IStream* stream, const int32_t* _table, int32_t _predictor);

        // Returns the number of samples it could expand. nSamples should be even.
        // Pulls samples from the IStream.
        int expand(int32_t* target, uint32_t nSamples, int32_t volume, bool add, bool overrideEasing);

        void rewind();
        bool done() const { return m_stream->done(); }

        static int samplesToBytes(int n) {
            return (n + 1) / 2;
        }

        static int bytesToSamples(int b) {
            return b * 2;
        }

        static void generateTestData(int nSamples, int16_t* data);

        // Fill a buffer from n exparenders. Will rewind() and loop as needed.
        static void fillBuffer(int32_t* buffer, int bufferSamples, 
            ExpanderAD4* expanders, int nExpanders, 
            const bool* loop, const int* volume, 
            bool disableEasing);

    private:
        uint8_t m_buffer[BUFFER_SIZE];
        IStream* m_stream = 0;
        S4ADPCM::State m_state;
    };
}
#endif
    