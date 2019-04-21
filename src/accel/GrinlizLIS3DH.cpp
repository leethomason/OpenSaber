#include "GrinlizLIS3DH.h"
#include <Arduino.h>
#include <SPI.h>

SPISettings gSPISettings(500000, MSBFIRST, SPI_MODE0);

GrinlizLIS3DH::GrinlizLIS3DH(uint8_t enable, uint8_t scale, uint8_t dataRate)
{
    this->enable = enable;
    this->scale = scale;
    this->dataRate = dataRate;

    divisor = 1000;
    if (dataRate == LIS3DH_RANGE_2_G) divisor = 16384;
    if (dataRate == LIS3DH_RANGE_4_G) divisor = 8192;
    if (dataRate == LIS3DH_RANGE_8_G) divisor = 4096;
    if (dataRate == LIS3DH_RANGE_16_G) divisor = 2048;
}

bool GrinlizLIS3DH::begin()
{
    delay(10);  // initial warmup
    uint8_t deviceid = readReg(LIS3DH_REG_WHOAMI);
    if (deviceid != 0x33)
        return false;

    // Temperature default disable

    writeReg(LIS3DH_REG_CTRL1, (dataRate << 4) | LIS3DH_AXIS_X | LIS3DH_AXIS_Y | LIS3DH_AXIS_Z);
    writeReg(LIS3DH_REG_CTRL2, 0);  // turn off high pass filter
    writeReg(LIS3DH_REG_CTRL3, 0);  // turn off interrupts

    // BDU (block data update) 
    // High resolution (12 bit) 
    static const uint8_t BDU = 0x80;
    static const uint8_t HIGHRES = 0x08;
    writeReg(LIS3DH_REG_CTRL4, (scale << 4) | HIGHRES | BDU);

    writeReg(LIS3DH_REG_CTRL5, 0x40);   // FIFO enable
    delay(10);  // unclear if needed
    writeReg(LIS3DH_REG_REFERENCE, 0);
    delay(10);  // unclear if needed
}

uint8_t GrinlizLIS3DH::readReg(uint8_t reg)
{
    SPI.beginTransaction(gSPISettings);
    digitalWrite(enable, LOW);
    SPI.transfer(reg | 0x80);
    uint8_t value = SPI.transfer(0);
    digitalWrite(enable, HIGH);
    SPI.endTransaction();
    return value;
}

void GrinlizLIS3DH::writeReg(uint8_t reg, uint8_t value)
{
    SPI.beginTransaction(gSPISettings);
    digitalWrite(enable, LOW);
    SPI.transfer(reg & (~0x80));
    SPI.transfer(value);
    digitalWrite(enable, HIGH);
    SPI.endTransaction();
}

int GrinlizLIS3DH::readRaw(AccelData* data, int n, int16_t* div)
{
    *div = divisor;
    int i = 0;

    for(i=0; i<n; ++i) {
        uint8_t status = readReg(LIS3DH_REG_STATUS1);
        if (status & 0x7 != 0x7)
            break;

        //Serial.println("have data");
        SPI.beginTransaction(gSPISettings);
        digitalWrite(enable, LOW);
        SPI.transfer(LIS3DH_REG_OUT_X_L | 0x80 | 0x40);

        data[i].x = SPI.transfer(0xff);
        data[i].x |= uint16_t(SPI.transfer(0xff)) << 8;

        data[i].y = SPI.transfer(0xff);
        data[i].y |= uint16_t(SPI.transfer(0xff)) << 8;

        data[i].z = SPI.transfer(0xff);
        data[i].z |= uint16_t(SPI.transfer(0xff)) << 8;

        digitalWrite(enable, HIGH);
        SPI.endTransaction();
    }
    return i;
}
