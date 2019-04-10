#ifndef WAV_COMPRESSION
#define WAV_COMPRESSION

#include "wav12stream.h"

#include <stdint.h>
#include <assert.h>
#include <string.h>

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
        uint8_t  format;        // 3 is the only supported
        uint8_t  unused[3];
    };

    struct Velocity
    {
        int prev2 = 0;
        int prev1 = 0;
        int guess() const { return 2 * prev1 - prev2; }
        void push(int value) {
            prev2 = prev1;
            prev1 = value;
        }
    };

    // Codec 0 is uncompressed. (100%)
    // Codec 1 is 12 bit (loss) (75%)
    // Codec 2 is 12 bit (loss) with delta in a frame (63%)
    // Codec 3 is 12 bit (loss) predictive, and already better (58%)
    // Codec 3b is 12 bit (loss) predictive, uses extra bits, and gets to 55%
    bool compressVelocity(const int16_t* data, int32_t nSamples, uint8_t** compressed, uint32_t* nCompressed);
    
    class MemStream : public wav12::IStream
    {
    public:
        MemStream(const uint8_t* data, uint32_t dataSize);
        
        virtual void set(uint32_t addr, uint32_t size);
        virtual uint32_t fetch(uint8_t* buffer, uint32_t nBytes);
        virtual void rewind();

     protected:
        const uint8_t* m_data;
        const uint8_t* m_data_end;
        uint32_t m_addr = 0;
        uint32_t m_size = 0;
        uint32_t m_pos = 0;
    };

    class ExpanderV
    {
    public:
        static const int BUFFER_SIZE = 256;

        ExpanderV() {}
        void init(IStream* stream);

        // Returns the number of samples it could expand.
        int expand(int32_t* target, uint32_t nTarget, int32_t volume, bool add);
        bool done() const { return m_done; }
        void rewind();

    private:
        inline bool hasSample() {
            if (m_bufferStart < m_bufferEnd - 1)
                return true;

            if (m_bufferStart == m_bufferEnd - 1 &&
                m_buffer[m_bufferStart] & 0x80)
                return true;
        
            return false;
        }
        void fetch();
        
        uint8_t m_buffer[BUFFER_SIZE];
        IStream* m_stream = 0;
        int m_bufferEnd = 0;      // position in the buffer
        int m_bufferStart = 0;
        bool m_done = false;

        // State for decompression
        Velocity m_vel;
        int m_high3 = 0;
        bool m_hasHigh3 = false;

    };
}
#endif
