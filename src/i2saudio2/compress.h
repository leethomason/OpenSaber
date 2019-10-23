#ifndef WAV_COMPRESSION
#define WAV_COMPRESSION

#include "wav12stream.h"
#include "s4adpcm.h"

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

//#define TUNE_MODE

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

        // Returns the number of samples it could expand. nTarget should be even,
        // unless it is the last sample (which can be odd if it uses up the
        // entire track.)
        int expand(int32_t* target, uint32_t nTarget, int32_t volume, bool add, bool use8Bit);
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
        static void compress4(const int16_t* data, int32_t nSamples, uint8_t** compressed, uint32_t* nCompressed);

        static void compress8(const int16_t* data, int32_t nSamples, uint8_t** compressed, uint32_t* nCompressed);

    private:
        uint8_t m_buffer[BUFFER_SIZE];
        IStream* m_stream = 0;
        S4ADPCM::State m_state;
    };
}
#endif
