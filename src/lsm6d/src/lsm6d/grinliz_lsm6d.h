#pragma once

#include <stdint.h>
#include "../util/Grinliz_Util.h"
#include "../util/Grinliz_SPI.h"

// Accelerometer & Gyroscope
// SPI interface
class GrinlizLSM6D
{
public:
    GrinlizLSM6D(int cs);

    void begin(SPIClass *spi);
    int flushFifo();

    enum
    {
        WHOAMI = 0x6c
    };
    uint8_t whoAmI();

private:
    uint8_t _cs;
    Grinliz_SPI _spiDriver;

    void init();
    void readStatus();
};