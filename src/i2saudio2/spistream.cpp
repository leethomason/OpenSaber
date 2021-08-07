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

#include "spistream.h"
#include "interface.h"

void SPIStream::set(uint32_t addr, uint32_t size)
{
    m_addr = addr;
    m_size = size;
    m_pos = 0;
}

// Call from interrupt
void SPIStream::rewind()
{
    m_pos = 0;
}

bool SPIStream::done() const
{
    return m_pos == m_size;
}

uint32_t SPIStream::fetch(uint8_t* target, uint32_t nBytes)
{
    // Normally, the SPI would need to be locked.
    // HOWEVER, this is only called by the interrupt, so 
    // all should be well.

    // Don't read over the end. (Oops - that was a bug.)

    if (m_pos + nBytes > m_size)
        nBytes = m_size - m_pos;
    if (nBytes == 0) return 0;

    uint32_t r = m_flash->readMemory(
        m_addr + m_pos, 
        target,
        nBytes);

    m_pos += r;
    return r;
}
