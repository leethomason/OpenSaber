#include "grinliz_lis3dh.h"

const SPISettings spiSettings(500000, MSBFIRST, SPI_MODE0);

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

static const uint8_t LIS3DH_DATARATE_400_HZ = 0b0111; //  400Hz
static const uint8_t LIS3DH_DATARATE_200_HZ = 0b0110; //  200Hz
static const uint8_t LIS3DH_DATARATE_100_HZ = 0b0101; //  100Hz
static const uint8_t LIS3DH_DATARATE_50_HZ = 0b0100;  //   50Hz
static const uint8_t LIS3DH_DATARATE_25_HZ = 0b0011;  //   25Hz
static const uint8_t LIS3DH_DATARATE_10_HZ = 0b0010;  // 10 Hz
static const uint8_t LIS3DH_DATARATE_1_HZ = 0b0001;   // 1 Hz

static const uint8_t SPI_INCREMENT = 0x40;
static const uint8_t SPI_READ = 0x80;

#define DEBUG_DEEP 1

void GrinlizLIS3DH::begin(SPIClass* spi)
{
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);

    _spi = spi;
    _spi->begin();
    delay(10);

    uint32_t id = readReg(LIS3DH_REG_WHOAMI);
    if (id != 0b00110011) {
        Serial.println("LIS3DH WhoAmI failed.\n");
    }
    Serial.print("WhoAmI: ");
    Serial.println(id, HEX);

    static const uint8_t ctrl1Val = 0x07 | (LIS3DH_DATARATE_1_HZ << 4);   // all axis, 100hz, normal power
    static const uint8_t ctrl2Val = 0;  // high pass filter off
    static const uint8_t ctrl3Val = 0;  // interupts off
    // block data: 0 (should be 1?)
    // endian 0: LSB at lower address
    // range: 11 (16g)
    // high res 0
    // self test 00
    // interface mode 0
    static const uint8_t ctrl4Val = 0b00110000;
    static const uint8_t ctrl5Val = 0b01000000; // enable FIFO

    writeReg(LIS3DH_REG_CTRL1, ctrl1Val);
    writeReg(LIS3DH_REG_CTRL2, ctrl2Val);
    writeReg(LIS3DH_REG_CTRL3, ctrl3Val);
    writeReg(LIS3DH_REG_CTRL4, ctrl4Val);
    writeReg(LIS3DH_REG_CTRL5, ctrl5Val);

    #if DEBUG_DEEP
    Serial.print("CTRL-1: "); Serial.print(readReg(LIS3DH_REG_CTRL1), HEX); Serial.print(" expected:"); Serial.println(ctrl1Val, HEX);
    Serial.print("CTRL-2: "); Serial.print(readReg(LIS3DH_REG_CTRL2), HEX); Serial.print(" expected:"); Serial.println(ctrl2Val, HEX);
    Serial.print("CTRL-3: "); Serial.print(readReg(LIS3DH_REG_CTRL3), HEX); Serial.print(" expected:"); Serial.println(ctrl3Val, HEX);
    Serial.print("CTRL-4: "); Serial.print(readReg(LIS3DH_REG_CTRL4), HEX); Serial.print(" expected:"); Serial.println(ctrl4Val, HEX);
    Serial.print("CTRL-5: "); Serial.print(readReg(LIS3DH_REG_CTRL5), HEX); Serial.print(" expected:"); Serial.println(ctrl5Val, HEX);
    #endif
}

uint8_t GrinlizLIS3DH::readReg(uint8_t reg)
{
    uint8_t buf[1];
    readData(reg, buf, 1);
    return buf[0];
}

void GrinlizLIS3DH::writeReg(uint8_t reg, uint8_t val)
{
    writeData(reg, &val, 1);
}

void GrinlizLIS3DH::readData(uint8_t addr, uint8_t *buf, int n) 
{
    _spi->beginTransaction(spiSettings);
    digitalWrite(_cs, LOW);

	if (n > 1) {
		addr |= SPI_INCREMENT;
	}

	_spi->transfer(SPI_READ | addr);

	for(int i = 0; i < n; i++) {
		buf[i] = _spi->transfer(0);
	}

    digitalWrite(_cs, HIGH);
    _spi->endTransaction();
}

void GrinlizLIS3DH::writeData(uint8_t addr, const uint8_t *buf, int n) 
{
    _spi->beginTransaction(spiSettings);
    digitalWrite(_cs, LOW);

	if (n > 1) {
		addr |= SPI_INCREMENT;
	}

	_spi->transfer(addr);
	for(int i = 0; i < n; i++) {
		_spi->transfer(buf[i]);
	}

    digitalWrite(_cs, HIGH);
    _spi->endTransaction();
}