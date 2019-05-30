#include "GrinlizLIS3DH.h"
#include <Arduino.h>
#include <SPI.h>

SPISettings gSPISettings(500000, MSBFIRST, SPI_MODE0);

GrinlizLIS3DH* GrinlizLIS3DH::_instance = 0;


GrinlizLIS3DH::GrinlizLIS3DH(uint8_t enable, uint8_t scale, uint8_t dataRate)
{
    _instance = this;

    this->m_enable = enable;
    this->m_scale = scale;
    this->m_dataRate = dataRate;

    m_divisor = 1000;
    if (scale == LIS3DH_RANGE_2_G) m_divisor = 16384;
    if (scale == LIS3DH_RANGE_4_G) m_divisor = 8192;
    if (scale == LIS3DH_RANGE_8_G) m_divisor = 4096;
    if (scale == LIS3DH_RANGE_16_G) m_divisor = 1280;

    m_divInv = 1.f / m_divisor;
}

bool GrinlizLIS3DH::begin()
{
    delay(10);  // initial warmup

    uint8_t deviceid = readReg(LIS3DH_REG_WHOAMI);
    if (deviceid != 0x33)
        return false;

    delay(10);  // initial warmup

    // Reset
    writeReg(LIS3DH_REG_CTRL1, 0x07);
    writeReg(LIS3DH_REG_CTRL2, 0x00);
    writeReg(LIS3DH_REG_CTRL3, 0x00);
    writeReg(LIS3DH_REG_CTRL4, 0x00);
    writeReg(LIS3DH_REG_CTRL5, 0x00);
    writeReg(LIS3DH_REG_CTRL6, 0x00);
    writeReg(LIS3DH_REG_INT1CFG, 0x00);
    writeReg(LIS3DH_REG_INT1THS, 0x00);
    writeReg(LIS3DH_REG_INT1DUR, 0x00);
    writeReg(LIS3DH_REG_CLICKCFG, 0x00);
    writeReg(LIS3DH_REG_CLICKSRC, 0x00);
    writeReg(LIS3DH_REG_CLICKTHS, 0x00);
    writeReg(LIS3DH_REG_TIMELIMIT, 0x00);
    writeReg(LIS3DH_REG_TIMELATENCY, 0x00);
    writeReg(LIS3DH_REG_TIMEWINDOW, 0x00);
    writeReg(LIS3DH_REG_FIFOCTRL, 0x00);
    writeReg(LIS3DH_REG_FIFOSRC, 0x00);

    // Configure to run in high resolution mode.
    // Note this 12 bits resolution, even though
    // the output is 16 bits. It should nominally
    // consume about 20 micro-amps (whereas the LEDs 
    // are probably about 2000 mico-amps)

    // Also want Stream mode. FIFO, but new values will
    // replace old ones as they come in.

    writeReg(LIS3DH_REG_TEMPCFG, 0);    // disable temperature and adc

    static uint8_t AXIS_ENABLE = 7;
    writeReg(LIS3DH_REG_CTRL1, (m_dataRate << 4) | AXIS_ENABLE);
    writeReg(LIS3DH_REG_CTRL2, 0);  // turn off high pass filter
    writeReg(LIS3DH_REG_CTRL3, 0);  // turn off interrupts

    // BDU (block data update) 
    // High resolution (12 bit) 
    static const uint8_t BDU = 0x80;
    static const uint8_t HIGHRES = 0x08;
    writeReg(LIS3DH_REG_CTRL4, (m_scale << 4) | HIGHRES | BDU);

    writeReg(LIS3DH_REG_CTRL5, 0x40);   // FIFO enable
    writeReg(LIS3DH_REG_CTRL6, 0);   
    delay(10);

    writeReg(LIS3DH_REG_REFERENCE, 0);
    writeReg(LIS3DH_REG_CTRL5, 0x40);   // FIFO enable (again)
    writeReg(LIS3DH_REG_FIFOCTRL, 2 << 6);   // Stream start

    return true;
}

uint8_t GrinlizLIS3DH::readReg(uint8_t reg)
{
    SPI.beginTransaction(gSPISettings);
    digitalWrite(m_enable, LOW);
    SPI.transfer(reg | 0x80);
    uint8_t value = SPI.transfer(0);
    digitalWrite(m_enable, HIGH);
    SPI.endTransaction();
    return value;
}

void GrinlizLIS3DH::writeReg(uint8_t reg, uint8_t value)
{
    SPI.beginTransaction(gSPISettings);
    digitalWrite(m_enable, LOW);
    SPI.transfer(reg & (~0x80));
    SPI.transfer(value);
    digitalWrite(m_enable, HIGH);
    SPI.endTransaction();
}

void GrinlizLIS3DH::flush(int remaining, int* removed)
{
    int avail = available();
    int n = avail > remaining ? avail - remaining : 0;
    if (removed) *removed = n;
    if (n == 0) return;

    SPI.beginTransaction(gSPISettings);
    digitalWrite(m_enable, LOW);
    for(int i=0; i<n; ++i) {
        SPI.transfer(LIS3DH_REG_OUT_X_L | 0x80 | 0x40);
        int16_t x, y, z;

        x = SPI.transfer(0xff);
        x |= uint16_t(SPI.transfer(0xff)) << 8;

        y = SPI.transfer(0xff);
        y |= uint16_t(SPI.transfer(0xff)) << 8;

        z = SPI.transfer(0xff);
        z |= uint16_t(SPI.transfer(0xff)) << 8;
    }
    digitalWrite(m_enable, HIGH);
    SPI.endTransaction();
}

int GrinlizLIS3DH::available()
{
    uint8_t fifosrc = readReg(LIS3DH_REG_FIFOSRC);
    uint8_t overrun = fifosrc & (1 << 6);
    uint8_t avail = fifosrc & 31;

    int maxRead = overrun ? 32 : avail;
    return maxRead;
}

int GrinlizLIS3DH::readInner(RawData* rawData, Data* data, int n)
{
    int i = 0;

    int avail = available();
    n = n > avail ? avail : n;
    if (n == 0) return 0;

    SPI.beginTransaction(gSPISettings);
    digitalWrite(m_enable, LOW);
    for(i=0; i<n; ++i) {

        SPI.transfer(LIS3DH_REG_OUT_X_L | 0x80 | 0x40);
        int16_t x, y, z;

        x = SPI.transfer(0xff);
        x |= uint16_t(SPI.transfer(0xff)) << 8;

        y = SPI.transfer(0xff);
        y |= uint16_t(SPI.transfer(0xff)) << 8;

        z = SPI.transfer(0xff);
        z |= uint16_t(SPI.transfer(0xff)) << 8;

        if (rawData) {
            rawData[i].x = x;
            rawData[i].y = y;
            rawData[i].z = z;
        }
        if (data) {
            data[i].ax = x * m_divInv;
            data[i].ay = y * m_divInv;
            data[i].az = z * m_divInv;
        }
    }
    digitalWrite(m_enable, HIGH);
    SPI.endTransaction();
    return n;
}

