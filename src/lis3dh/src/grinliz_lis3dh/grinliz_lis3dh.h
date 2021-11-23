#pragma once

#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>

class GrinlizLIS3DH
{
public:
    GrinlizLIS3DH(int cs) : _spi(0), _cs(cs) {}

    void begin(SPIClass* spi);

private:
    uint32_t read8(uint32_t);

    SPIClass* _spi;
    int _cs;
};