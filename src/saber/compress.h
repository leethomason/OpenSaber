#ifndef WAV_COMPRESSION
#define WAV_COMPRESSION

#include "wav12stream.h"

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

    struct Wav12Header
    {
        char id[4];             // 'wv12'
        uint32_t lenInBytes;    // after header, compressed size
        uint32_t nSamples;
        uint8_t  format; 
        uint8_t  unused[3];
    };

    struct Velocity
    {
        int prev2 = 0;
        int prev1 = 0;
        int guess() const {
            int g = 2 * prev1 - prev2;
            if (g > SHRT_MAX) g = SHRT_MAX;
            if (g < SHRT_MIN) g = SHRT_MIN;
            return g;
        }
        void push(int value) {
            prev2 = prev1;
            prev1 = value;
        }
    };

    class MemStream : public wav12::IStream
    {
    public:
        MemStream(const uint8_t* data, uint32_t dataSize);

        virtual void set(uint32_t addr, uint32_t size);
        virtual uint32_t fetch(uint8_t* buffer, uint32_t nBytes);
        virtual void rewind();
        virtual bool done() { return m_pos == m_size; }

    protected:
        const uint8_t* m_data;
        const uint8_t* m_data_end;
        uint32_t m_addr = 0;
        uint32_t m_size = 0;
        uint32_t m_pos = 0;
    };

    class ExpanderAD4
    {
    public:
        static const int BUFFER_SIZE = 128;

        ExpanderAD4() {}
        void attach(IStream* stream);

        // Returns the number of samples it could expand. nTarget should be even,
        // unless it is the last sample (which can be odd if it uses up the
        // entire track.)
        int expand(int32_t* target, uint32_t nTarget, int32_t volume, bool add);

        void rewind();
        bool done() const;

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
        static void compress(const int16_t* data, int32_t nSamples, uint8_t** compressed, uint32_t* nCompressed);

#ifdef TUNE_MODE
    public:
        static const int TABLE_SIZE = 8;
        static int DELTA[TABLE_SIZE];
#else
    private:
        static const int TABLE_SIZE = 8;
        static const int DELTA[TABLE_SIZE];
#endif

    private:
        uint8_t m_buffer[BUFFER_SIZE];
        IStream* m_stream = 0;

        // State for decompression
        Velocity m_vel;
        int m_shift;
        bool m_high;
    };
}
#endif
