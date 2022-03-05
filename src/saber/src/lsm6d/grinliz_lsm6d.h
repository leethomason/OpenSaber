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
};