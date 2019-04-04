#ifndef WAV12_STREAM_INCLUDED
#define WAV12_STREAM_INCLUDED

#include <stdint.h>

namespace wav12 {

    // Represents an entire stream, which may contain multiple compressed sounds.
    // On the AVR, these methods are *only* called from the interrupt.
    // The IStream has no concept of samples; it returns data until
    // that data is consumed.
    class IStream {
    public:
    
        // Set the head to a particular sound.
        virtual void set(uint32_t addr, uint32_t size) = 0;
        // Fetch bytes within a sound. Returns bytes fetch, or 0 if none left.
        virtual uint32_t fetch(uint8_t* target, uint32_t nBytes) = 0;
        // Rewind back to the beginning of the sound.
        virtual void rewind() = 0;
    };

}


#endif // WAV12_STREAM_INCLUDED
