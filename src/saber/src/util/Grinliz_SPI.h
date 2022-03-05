#pragma once

#include <stdint.h>
#include "SPI.h"

class Grinliz_SPI
{
public:
    Grinliz_SPI() {}

    void begin(SPIClass *spiinterface, uint8_t cs, uint32_t clock);

    void write(uint8_t command, const uint8_t *data, uint32_t len);
    void read(uint8_t command, uint8_t *data, uint32_t len);

private:
    SPIClass *_spi = 0;
    uint8_t _cs = 255;
    SPISettings _settings;
};