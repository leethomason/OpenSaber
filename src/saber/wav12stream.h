#ifndef WAV12_STREAM_INCLUDED
#define WAV12_STREAM_INCLUDED

#include <stdint.h>

namespace wav12 {

    class IStream {
    public:
        virtual uint32_t fetch(uint8_t* target, uint32_t nBytes) = 0;
    };

}


#endif // WAV12_STREAM_INCLUDED
