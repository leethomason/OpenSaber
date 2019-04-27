#pragma once
#include <stdint.h>

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define LIS3DH_DEFAULT_ADDRESS  (0x18)    // if SDO/SA0 is 3V, its 0x19
/*=========================================================================*/

#define LIS3DH_REG_STATUS1       0x07
#define LIS3DH_REG_OUTADC1_L     0x08
#define LIS3DH_REG_OUTADC1_H     0x09
#define LIS3DH_REG_OUTADC2_L     0x0A
#define LIS3DH_REG_OUTADC2_H     0x0B
#define LIS3DH_REG_OUTADC3_L     0x0C
#define LIS3DH_REG_OUTADC3_H     0x0D
#define LIS3DH_REG_INTCOUNT      0x0E
#define LIS3DH_REG_WHOAMI        0x0F
#define LIS3DH_REG_TEMPCFG       0x1F
#define LIS3DH_REG_CTRL1         0x20
#define LIS3DH_REG_CTRL2         0x21
#define LIS3DH_REG_CTRL3         0x22
#define LIS3DH_REG_CTRL4         0x23
#define LIS3DH_REG_CTRL5         0x24
#define LIS3DH_REG_CTRL6         0x25
#define LIS3DH_REG_REFERENCE     0x26
#define LIS3DH_REG_STATUS2       0x27
#define LIS3DH_REG_OUT_X_L       0x28
#define LIS3DH_REG_OUT_X_H       0x29
#define LIS3DH_REG_OUT_Y_L       0x2A
#define LIS3DH_REG_OUT_Y_H       0x2B
#define LIS3DH_REG_OUT_Z_L       0x2C
#define LIS3DH_REG_OUT_Z_H       0x2D
#define LIS3DH_REG_FIFOCTRL      0x2E
#define LIS3DH_REG_FIFOSRC       0x2F
#define LIS3DH_REG_INT1CFG       0x30
#define LIS3DH_REG_INT1SRC       0x31
#define LIS3DH_REG_INT1THS       0x32
#define LIS3DH_REG_INT1DUR       0x33
#define LIS3DH_REG_CLICKCFG      0x38
#define LIS3DH_REG_CLICKSRC      0x39
#define LIS3DH_REG_CLICKTHS      0x3A
#define LIS3DH_REG_TIMELIMIT     0x3B
#define LIS3DH_REG_TIMELATENCY   0x3C
#define LIS3DH_REG_TIMEWINDOW    0x3D
#define LIS3DH_REG_ACTTHS        0x3E
#define LIS3DH_REG_ACTDUR        0x3F

#define LIS3DH_RANGE_16_G        0x03   // +/- 16g
#define LIS3DH_RANGE_8_G         0x02   // +/- 8g
#define LIS3DH_RANGE_4_G         0x01   // +/- 4g
#define LIS3DH_RANGE_2_G         0x00   // +/- 2g (default value)

/* Used with register 0x2A (LIS3DH_REG_CTRL_REG1) to set bandwidth */
#define LIS3DH_DATARATE_400_HZ     0x07     //  400Hz 
#define LIS3DH_DATARATE_200_HZ     0x06     //  200Hz
#define LIS3DH_DATARATE_100_HZ     0x05     //  100Hz
#define LIS3DH_DATARATE_50_HZ      0x04     //   50Hz
#define LIS3DH_DATARATE_25_HZ      0x03     //   25Hz
#define LIS3DH_DATARATE_10_HZ      0x02     // 10 Hz
#define LIS3DH_DATARATE_1_HZ       0x02     // 1 Hz
#define LIS3DH_DATARATE_POWERDOWN           0
#define LIS3DH_DATARATE_LOWPOWER_1K6HZ      0x08
#define LIS3DH_DATARATE_LOWPOWER_5KHZ       0x09


class GrinlizLIS3DH
{
public:
    struct AccelData {
        int16_t x, y, z;
    };

    GrinlizLIS3DH(uint8_t enable, uint8_t scale = LIS3DH_RANGE_8_G, uint8_t dataRate = LIS3DH_DATARATE_100_HZ);

    bool begin();
    int readRaw(AccelData* data, int inData, int16_t* divisor);

private:
    uint8_t readReg(uint8_t reg);
    void writeReg(uint8_t reg, uint8_t value);

    uint8_t enable;
    uint8_t scale;
    uint8_t dataRate;
    int16_t divisor;
};
