#include "GrinlizLSM303.h"
#include <Arduino.h>
#include <Wire.h>
#include "Grinliz_Arduino_Util.h"
#include "Grinliz_Util.h"

#define LSM303_ADDRESS_ACCEL (0x32 >> 1) // 0011001x
#define LSM303_ADDRESS_MAG (0x3C >> 1)   // 0011110x

enum
{
    LSM303_TEMP_CFG_REG_A             = 0x1F,
                                               // DEFAULT    TYPE
    LSM303_REGISTER_ACCEL_CTRL_REG1_A = 0x20,  // 00000111   rw
    LSM303_REGISTER_ACCEL_CTRL_REG2_A = 0x21,  // 00000000   rw
    LSM303_REGISTER_ACCEL_CTRL_REG3_A = 0x22,  // 00000000   rw
    LSM303_REGISTER_ACCEL_CTRL_REG4_A = 0x23,  // 00000000   rw
    LSM303_REGISTER_ACCEL_CTRL_REG5_A = 0x24,  // 00000000   rw
    LSM303_REGISTER_ACCEL_CTRL_REG6_A = 0x25,  // 00000000   rw
    LSM303_REGISTER_ACCEL_REFERENCE_A = 0x26,  // 00000000   r
    LSM303_REGISTER_ACCEL_STATUS_REG_A = 0x27, // 00000000   r
    LSM303_REGISTER_ACCEL_OUT_X_L_A = 0x28,
    LSM303_REGISTER_ACCEL_OUT_X_H_A = 0x29,
    LSM303_REGISTER_ACCEL_OUT_Y_L_A = 0x2A,
    LSM303_REGISTER_ACCEL_OUT_Y_H_A = 0x2B,
    LSM303_REGISTER_ACCEL_OUT_Z_L_A = 0x2C,
    LSM303_REGISTER_ACCEL_OUT_Z_H_A = 0x2D,
    LSM303_REGISTER_ACCEL_FIFO_CTRL_REG_A = 0x2E,
    LSM303_REGISTER_ACCEL_FIFO_SRC_REG_A = 0x2F,
    LSM303_REGISTER_ACCEL_INT1_CFG_A = 0x30,
    LSM303_REGISTER_ACCEL_INT1_SOURCE_A = 0x31,
    LSM303_REGISTER_ACCEL_INT1_THS_A = 0x32,
    LSM303_REGISTER_ACCEL_INT1_DURATION_A = 0x33,
    LSM303_REGISTER_ACCEL_INT2_CFG_A = 0x34,
    LSM303_REGISTER_ACCEL_INT2_SOURCE_A = 0x35,
    LSM303_REGISTER_ACCEL_INT2_THS_A = 0x36,
    LSM303_REGISTER_ACCEL_INT2_DURATION_A = 0x37,
    LSM303_REGISTER_ACCEL_CLICK_CFG_A = 0x38,
    LSM303_REGISTER_ACCEL_CLICK_SRC_A = 0x39,
    LSM303_REGISTER_ACCEL_CLICK_THS_A = 0x3A,
    LSM303_REGISTER_ACCEL_TIME_LIMIT_A = 0x3B,
    LSM303_REGISTER_ACCEL_TIME_LATENCY_A = 0x3C,
    LSM303_REGISTER_ACCEL_TIME_WINDOW_A = 0x3D
};

enum
{
    LSM303_REGISTER_MAG_CRA_REG_M = 0x00,
    LSM303_REGISTER_MAG_CRB_REG_M = 0x01,
    LSM303_REGISTER_MAG_MR_REG_M = 0x02,
    LSM303_REGISTER_MAG_OUT_X_H_M = 0x03,
    LSM303_REGISTER_MAG_OUT_X_L_M = 0x04,
    LSM303_REGISTER_MAG_OUT_Z_H_M = 0x05,
    LSM303_REGISTER_MAG_OUT_Z_L_M = 0x06,
    LSM303_REGISTER_MAG_OUT_Y_H_M = 0x07,
    LSM303_REGISTER_MAG_OUT_Y_L_M = 0x08,
    LSM303_REGISTER_MAG_SR_REG_Mg = 0x09,
    LSM303_REGISTER_MAG_IRA_REG_M = 0x0A,
    LSM303_REGISTER_MAG_IRB_REG_M = 0x0B,
    LSM303_REGISTER_MAG_IRC_REG_M = 0x0C,
    LSM303_REGISTER_MAG_TEMP_OUT_H_M = 0x31,
    LSM303_REGISTER_MAG_TEMP_OUT_L_M = 0x32
};

enum
{
    LSM303_MAGGAIN_1_3 = 0x20, // +/- 1.3
    LSM303_MAGGAIN_1_9 = 0x40, // +/- 1.9
    LSM303_MAGGAIN_2_5 = 0x60, // +/- 2.5
    LSM303_MAGGAIN_4_0 = 0x80, // +/- 4.0
    LSM303_MAGGAIN_4_7 = 0xA0, // +/- 4.7
    LSM303_MAGGAIN_5_6 = 0xC0, // +/- 5.6
    LSM303_MAGGAIN_8_1 = 0xE0  // +/- 8.1
};

enum
{
    LSM303_MAGRATE_0_7 = 0x00, // 0.75 Hz
    LSM303_MAGRATE_1_5 = 0x01, // 1.5 Hz
    LSM303_MAGRATE_3_0 = 0x62, // 3.0 Hz
    LSM303_MAGRATE_7_5 = 0x03, // 7.5 Hz
    LSM303_MAGRATE_15 = 0x04,  // 15 Hz
    LSM303_MAGRATE_30 = 0x05,  // 30 Hz
    LSM303_MAGRATE_75 = 0x06,  // 75 Hz
    LSM303_MAGRATE_220 = 0x07  // 200 Hz
};

bool GrinlizLSM303::begin()
{
    Log.p("---- LSM303 begin ----").eol();

    Wire.begin();
    delay(10);  // initial warmup

    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_CTRL_REG1_A, 0x57);
    uint8_t reg1_a = read8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_CTRL_REG1_A);

    Log.p("ctrlA=0x").p(reg1_a, HEX).p(" ").p(reg1_a == 0x57 ? "ok" : "error").eol();

    uint8_t whoAmIA = read8(LSM303_ADDRESS_ACCEL, 0x0f);
    Log.p("A:whoAmA=0x").p(whoAmIA, HEX).p(" ").p(whoAmIA == 0x33 ? "ok" : "error").eol();

    uint8_t whoAmIM = read8(LSM303_ADDRESS_MAG, 0x4f);
    Log.p("A:whoAmM=0x").p(whoAmIM, HEX).p(" ").p(whoAmIM == 0x40 ? "ok" : "error").eol();

    delay(10);

    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_CTRL_REG1_A, 0x07);
    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_CTRL_REG2_A, 0x00);
    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_CTRL_REG3_A, 0x00);
    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_CTRL_REG4_A, 0x00);
    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_CTRL_REG5_A, 0x00);
    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_CTRL_REG6_A, 0x00);

    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_INT1_CFG_A, 0x00);
    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_INT2_CFG_A, 0x00);
    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_CLICK_CFG_A, 0x00);

    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_FIFO_CTRL_REG_A, 0x00);
    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_FIFO_SRC_REG_A, 0x00);
    write8(LSM303_ADDRESS_ACCEL, LSM303_TEMP_CFG_REG_A, 0);
 
    // Configure to run in high resolution mode.
    // Note this 12 bits resolution, even though
    // the output is 16 bits. It should nominally
    // consume about 20 micro-amps (whereas the LEDs 
    // are probably about 2000 mico-amps)

    // Also want Stream mode. FIFO, but new values will
    // replace old ones as they come in.

    static uint8_t AXIS_ENABLE = 7;
    static uint8_t dataRate = 5;    // 100hz
    static uint8_t scale = 2;       // 8g
    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_CTRL_REG1_A, (dataRate << 4) | AXIS_ENABLE);

    // BDU (block data update) 
    // High resolution (12 bit) 
    static const uint8_t BDU = 0x80;
    static const uint8_t HIGHRES = 0x08;
    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_CTRL_REG4_A, (scale << 4) | HIGHRES | BDU);

    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_CTRL_REG5_A, 0x40);   // FIFO enable
    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_CTRL_REG6_A, 0);   
    delay(10);

    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_REFERENCE_A, 0);
    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_CTRL_REG5_A, 0x40);   // FIFO enable (again)
    write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_FIFO_CTRL_REG_A, 2 << 6);   // Stream start

    return (whoAmIA == 0x33) && (whoAmIM == 0x40);
}

int GrinlizLSM303::available()
{
    uint8_t fifosrc = read8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_FIFO_SRC_REG_A);
    uint8_t overrun = fifosrc & (1 << 6);
    uint8_t avail = fifosrc & 31;

    return avail + (overrun ? 1 : 0);
}

int GrinlizLSM303::readInner(RawData* rawData, Data* data, int n)
{
    int i = 0;

    int avail = available();
    n = n > avail ? avail : n;
    if (n == 0) return 0;

    for(i=0; i<n; ++i) {
        Wire.beginTransmission(LSM303_ADDRESS_ACCEL);
        Wire.write(LSM303_REGISTER_ACCEL_OUT_X_L_A | 0x80);
        Wire.endTransmission();

        Wire.requestFrom((byte)LSM303_ADDRESS_ACCEL, (byte)6);
        
        while (Wire.available() < 2) {}
        uint32_t xlo = Wire.read();
        uint32_t xhi = Wire.read();

        while (Wire.available() < 2) {}
        uint32_t ylo = Wire.read();
        uint32_t yhi = Wire.read();
        
        while (Wire.available() < 2) {}
        uint32_t zlo = Wire.read();
        uint32_t zhi = Wire.read();

        uint32_t x = xlo | (xhi << 8);
        uint32_t y = ylo | (yhi << 8);
        uint32_t z = zlo | (zhi << 8);

        if (rawData) {
            rawData[i].x = x;
            rawData[i].y = y;
            rawData[i].z = z;
        }
        if (data) {
            static const float divInv = 1.0f / 4096;
            data[i].ax = x * divInv;
            data[i].ay = y * divInv;
            data[i].az = z * divInv;
        }
    }
    return n;
}

void GrinlizLSM303::write8(uint8_t address, uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(address);
    Wire.write((uint8_t)reg);
    Wire.write((uint8_t)value);
    Wire.endTransmission();
}

uint8_t GrinlizLSM303::read8(uint8_t address, uint8_t reg)
{
    uint8_t value;

    Wire.beginTransmission(address);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();

    Wire.requestFrom(address, (byte)1);
    value = Wire.read();
    Wire.endTransmission();

    return value;
}
