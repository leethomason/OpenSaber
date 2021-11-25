#pragma once

#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>

#include "../util/fixed.h"


static const uint8_t LIS3DH_DATARATE_400_HZ = 0b0111;  // 400Hz
static const uint8_t LIS3DH_DATARATE_200_HZ = 0b0110;  // 200Hz
static const uint8_t LIS3DH_DATARATE_100_HZ = 0b0101;  // 100Hz
static const uint8_t LIS3DH_DATARATE_50_HZ =  0b0100;  //  50Hz
static const uint8_t LIS3DH_DATARATE_25_HZ =  0b0011;  //  25Hz
static const uint8_t LIS3DH_DATARATE_10_HZ =  0b0010;  //  10Hz
static const uint8_t LIS3DH_DATARATE_1_HZ =   0b0001;  //   1Hz


class GrinlizLIS3DH
{
public:
    GrinlizLIS3DH(int cs) : _spi(0), _cs(cs) {}

    void begin(SPIClass* spi, uint8_t dataRate);

    bool sample(int16_t* x, int16_t* y, int16_t* z);
    bool sample(Fixed115* x, Fixed115* y, Fixed115* z);
    void flush();

private:
    uint8_t readReg(uint8_t);
    void writeReg(uint8_t reg, uint8_t val);

    void readData(uint8_t addr, uint8_t* buf, int n);
    void writeData(uint8_t addr, const uint8_t* buf, int n);

    SPIClass* _spi;
    int _cs;
};