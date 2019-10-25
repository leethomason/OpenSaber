#include "spistream.h"

#include <Adafruit_SPIFlash.h>

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

    uint32_t r = m_flash->readBuffer(
        m_addr + m_pos, 
        target,
        nBytes);

    m_pos += r;
    return r;
}
