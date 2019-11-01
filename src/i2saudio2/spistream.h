#pragma once

#include "wav12stream.h"

#include <stdint.h>

class Adafruit_SPIFlash;

class SPIStream : public wav12::IStream
{
public:
    SPIStream() {}
    void init(Adafruit_SPIFlash* flash) { m_flash = flash; }

    virtual void set(uint32_t addr, uint32_t size);
    virtual uint32_t fetch(uint8_t* target, uint32_t nBytes);
    virtual void rewind();
    virtual bool done() const;

private:
    Adafruit_SPIFlash* m_flash = 0;
    uint32_t m_addr = 0;
    uint32_t m_size = 0;
    uint32_t m_pos = 0;
};

