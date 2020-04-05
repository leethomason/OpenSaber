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

#include "wav12stream.h"
#include "s4adpcm.h"

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

namespace wav12 {

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

    class ExpanderAD4
    {
    public:
        static const int BUFFER_SIZE = 128;

        ExpanderAD4() {}
        void init(IStream* stream);

        // Returns the number of samples it could expand. nSamples should be even,
        // unless it is the last sample (which can be odd if it uses up the
        // entire track.)
        int expand(int32_t* target, uint32_t nSamples, int32_t volume, bool add, int codec, const int* table, bool overrideEasing);
        void rewind();
        bool done() const { return m_stream->done(); }

        // Codec 0 is uncompressed. (100%)
        // Codec 1 is 12 bit (loss) (75%)
        // Codec 2 is 12 bit (loss) with delta in a frame (63%)
        // Codec 3 is 12 bit (loss) predictive, and already better (58%)
        // Codec 3b is 12 bit (loss) predictive, uses extra bits, and gets to 55%
        // Codec 3c adds a stack for the high bits. PowerOff 79.2 -> 76.5. Also cleaner code.
        //
        // Codec 4 is lossy; predictive, with sliding scale. Similar to ADPCM 
        // Intending it to be replaced by ADPCM, but need to have a fallback for quality.
        // Always 2:1. Really good quality. Simpler.
        //
        // Codec 4/4-bit is a kind of ADPCM. (Although simpler than the standard algorithm.)
        // Simpler and cleaner. Tuned on the lightsaber sounds and a sample of classical music.
        // The quality is shockingly good for such a simple algorithm at 4 bits / samples.
        static void compress4(const int16_t* data, int32_t nSamples, uint8_t** compressed, uint32_t* nCompressed, const int* table, int64_t* e16squared);
        static void compress8(const int16_t* data, int32_t nSamples, uint8_t** compressed, uint32_t* nCompressed, const int* table, int64_t* e16squared);

        static void compress(int codec, const int16_t* data, int32_t nSamples, uint8_t** compressed, uint32_t* nCompressed, const int* table, int64_t* e16squared)
        {
            switch (codec) {
            case S4ADPCM_4BIT:  compress4(data, nSamples, compressed, nCompressed, table, e16squared);   break;
            case S4ADPCM_8BIT:  compress8(data, nSamples, compressed, nCompressed, table, e16squared);   break;
            }
        }

        // Exact; but there can be extra. For 7 samples, 4 bytes are needed, but a nibble is unused.
        static int samplesToBytes(int n, int codec) {
            switch (codec) {
            case S4ADPCM_4BIT:   return (n + 1) / 2;
            case S4ADPCM_8BIT:   return n;
            }
            assert(false);
            return 0;
        }

        // Exact; returns the possible number of samples. (Could be one less.)
        static int bytesToSamples(int b, int codec) {
            switch (codec) {
            case S4ADPCM_4BIT:   return b * 2;
            case S4ADPCM_8BIT:   return b;
            }
            assert(false);
            return 0;
        }

    private:
        static uint8_t m_buffer[BUFFER_SIZE];
        IStream* m_stream = 0;
        S4ADPCM::State m_state;
    };
}
#endif
