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
        // Has the stream's data been consumed? "done" from the view of the
        // stream, not necessarily the downstream output.
        virtual bool done() const = 0;
    };

}


#endif // WAV12_STREAM_INCLUDED
