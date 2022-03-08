#pragma once

#include <stdint.h>
#include "../util/Grinliz_Util.h"
#include "../util/Grinliz_SPI.h"

// Accelerometer & Gyroscope
// SPI interface
class GrinlizLSM6D
{
public:
    enum class Freq
    {
        HZ_12 = 0b0001,
        HZ_52 = 0b0011,
        HZ_100 = 0b0100,
        HZ_200 = 0b0101
    };

    GrinlizLSM6D(int cs);

    void begin(SPIClass *spi, Freq freq);

    void flush();

    // Accel in g-force
    // Gyro in degrees per second. 
    // Returns true if sample read. Can be used in a while() loop.
    bool sampleAccelGyro(Vec3<Fixed115> *accel, Vec3<int32_t> *gyro);

    enum
    {
        WHOAMI = 0x6c
    };
    uint8_t whoAmI();

    void test();

private:
    uint8_t _cs;
    Freq _freq = Freq::HZ_100;
    Grinliz_SPI _spiDriver;

    void init();
    void readStatus();
    uint32_t numWords();
    uint8_t readWord(Vec3<int16_t> *data);
};