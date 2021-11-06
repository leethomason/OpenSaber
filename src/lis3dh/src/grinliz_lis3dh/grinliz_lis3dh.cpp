#include "grinliz_lis3dh.h"

const SPISettings spiSettings(1000000, MSBFIRST, SPI_MODE0);

#define LIS3DH_REG_STATUS1 0x07
#define LIS3DH_REG_OUTADC1_L 0x08
#define LIS3DH_REG_OUTADC1_H 0x09
#define LIS3DH_REG_OUTADC2_L 0x0A
#define LIS3DH_REG_OUTADC2_H 0x0B
#define LIS3DH_REG_OUTADC3_L 0x0C
#define LIS3DH_REG_OUTADC3_H 0x0D
#define LIS3DH_REG_INTCOUNT 0x0E
#define LIS3DH_REG_WHOAMI   0x0F
#define LIS3DH_REG_TEMPCFG 0x1F
#define LIS3DH_REG_CTRL1 0x20
#define LIS3DH_REG_CTRL2 0x21
#define LIS3DH_REG_CTRL3 0x22
#define LIS3DH_REG_CTRL4 0x23
#define LIS3DH_REG_CTRL5 0x24
#define LIS3DH_REG_CTRL6 0x25
#define LIS3DH_REG_REFERENCE 0x26
#define LIS3DH_REG_STATUS2 0x27
#define LIS3DH_REG_OUT_X_L 0x28
#define LIS3DH_REG_OUT_X_H 0x29
#define LIS3DH_REG_OUT_Y_L 0x2A
#define LIS3DH_REG_OUT_Y_H 0x2B
#define LIS3DH_REG_OUT_Z_L 0x2C
#define LIS3DH_REG_OUT_Z_H 0x2D
#define LIS3DH_REG_FIFOCTRL 0x2E
#define LIS3DH_REG_FIFOSRC 0x2F
#define LIS3DH_REG_INT1CFG 0x30
#define LIS3DH_REG_INT1SRC 0x31
#define LIS3DH_REG_INT1THS 0x32
#define LIS3DH_REG_INT1DUR 0x33
#define LIS3DH_REG_CLICKCFG 0x38
#define LIS3DH_REG_CLICKSRC 0x39
#define LIS3DH_REG_CLICKTHS 0x3A
#define LIS3DH_REG_TIMELIMIT 0x3B
#define LIS3DH_REG_TIMELATENCY 0x3C
#define LIS3DH_REG_TIMEWINDOW 0x3D

void GrinlizLIS3DH::begin(SPIClass* spi)
{
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, LOW);

    _spi = spi;
    _spi->begin();
    delay(40);

    uint32_t id = read(LIS3DH_REG_WHOAMI);
    if (id != 0b00110011) {
        Serial.println("LIS3DH WhoAmI failed.\n");
    }
    Serial.print("WhoAmI: ");
    Serial.println(id);
}

uint32_t GrinlizLIS3DH::read(uint32_t reg)
{
    _spi->beginTransaction(spiSettings);
    digitalWrite(_cs, LOW);

    uint32_t r = _spi->transfer(reg | 0x80);
    digitalWrite(_cs, HIGH);
    _spi->endTransaction();
    return r;
}

