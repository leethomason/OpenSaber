#ifndef IMEMORY_AND_ISTREAM
#define IMEMORY_AND_ISTREAM

#include <stdint.h>

// Abstract out something that acts like random access memory.
class IMemory {
public: 
    virtual void readMemory(uint32_t addr, uint8_t *data, uint32_t len) = 0;
    virtual int32_t memorySize() = 0;
};

// And something that acts like a stream w/ rewind.
// Written for sound files, good for anything that has play & rewind
class IStream {
public:

    // Set the head to a particular sound.
    virtual void set(uint32_t addr, uint32_t size) = 0;
    // Fetch bytes within a sound. Returns bytes fetch, or 0 if none left.
    virtual uint32_t fetch(uint8_t* target, uint32_t nBytes) = 0;
    // Rewind back to the beginning of the sound.
    virtual void rewind() = 0;
    // Has the stream's data been consumed? "done" from the view of the
    // stream, not necessarily the downstream output.
    virtual bool done() const = 0;
};

#endif // IMEMORY_AND_ISTREAM