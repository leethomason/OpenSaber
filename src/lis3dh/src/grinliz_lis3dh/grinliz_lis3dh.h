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
    uint8_t readReg(uint8_t);
    void writeReg(uint8_t reg, uint8_t val);

    void readData(uint8_t addr, uint8_t* buf, int n);
    void writeData(uint8_t addr, const uint8_t* buf, int n);

    SPIClass* _spi;
    int _cs;
};