/*
  Copyright (c) Lee Thomason, Grinning Lizard Software

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
  of the Software, and to permit persons to whom the Software is furnished to do
  so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <Arduino.h>
#include <Wire.h>
#include <math.h>

#include "GrinlizLSM303.h"
#include "Grinliz_Arduino_Util.h"

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
    LSM303_REGISTER_MAG_SR_REG_M  = 0x09,
    LSM303_REGISTER_MAG_IRA_REG_M = 0x0A,
    LSM303_REGISTER_MAG_IRB_REG_M = 0x0B,
    LSM303_REGISTER_MAG_IRC_REG_M = 0x0C,
    LSM303_REGISTER_MAG_TEMP_OUT_H_M = 0x31,
    LSM303_REGISTER_MAG_TEMP_OUT_L_M = 0x32
};

enum {
    CFG_REG_A_M = 0x60,
    CFG_REG_B_M = 0x61,
    CFG_REG_C_M = 0x62,
    STATUS_REG_M = 0x67,

    OUTX_LOW_REG_M  = 0x68,
    OUTX_HIGH_REG_M = 0x69,
    OUTY_LOW_REG_M  = 0x6A,
    OUTY_HIGH_REG_M = 0x6B,
    OUTZ_LOW_REG_M  = 0x6C,
    OUTZ_HIGH_REG_M = 0x6D
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

GrinlizLSM303* GrinlizLSM303::s_instance;

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

    // Also the magnemometer in high resolution mode.
    //write8(LSM303_ADDRESS_MAG, CFG_REG_A_M, (1<<6) | (1<<5)); // reboot | reset
    delay(10);
    write8(LSM303_ADDRESS_MAG, CFG_REG_A_M, 
        (1<<7)      // temperature componsation
        | 3 << 2    // 100 Hz
        | 0         // continuous mode 
    );
    write8(LSM303_ADDRESS_MAG, CFG_REG_B_M,
        0 // 1           // low pass filter
    );
    delay(10);

    m_min.set(INT_MAX, INT_MAX, INT_MAX);
    m_max.set(INT_MIN, INT_MIN, INT_MIN);
    m_minQueued = m_min;
    m_maxQueued = m_max;
    return (whoAmIA == 0x33) && (whoAmIM == 0x40);
}

void GrinlizLSM303::logMagStatus()
{
    uint8_t d = read8(LSM303_ADDRESS_MAG, CFG_REG_A_M);
    int tempComp = d & (1<<7);
    int lowPower = d & (1<<4);
    int dataRateBits = (d >> 2) & 3;
    int dataRate = 0;
    switch(dataRateBits) {
        case 0: dataRate = 10; break;
        case 1: dataRate = 20; break;
        case 2: dataRate = 50; break;
        case 3: dataRate = 100; break;
    }
    int mode = d & 3;
    Log.p("Status Mag. tempComp=").p(tempComp ? 1 : 0).p(" lowPower=").p(lowPower ? 1 : 0).p(" dataRate=").p(dataRate).p(" mode=").p(mode == 0 ? "continuous" : "??").eol();

    d = read8(LSM303_ADDRESS_MAG, CFG_REG_B_M);
    int offsetCancellation = d & 2;
    int lowPass = d & 1;
    Log.p("    offsetCancellation=").p(offsetCancellation ? 1 : 0).p(" lowPassFilter=").p(lowPass).eol();

    d = read8(LSM303_ADDRESS_MAG, CFG_REG_C_M);
    int bdu = d & (1<<4);
    int ble = d & (1<<3);
    Log.p("    bdu (coherent read)=").p(bdu ? 1 : 0).p(" ble=").p(ble ? "inverted" : "normal").eol();
    delay(2);
    
}

void GrinlizLSM303::setMagDataRate(int hz)
{
    int rateBits = 3;
    switch(hz) {
        case 10: rateBits = 0; m_queueFrame = 40; break;
        case 20: rateBits = 1; m_queueFrame = 80; break;
        case 50: rateBits = 2; m_queueFrame = 200; break;
        default: rateBits = 3; m_queueFrame = 400; break;
    }
    uint8_t d = read8(LSM303_ADDRESS_MAG, CFG_REG_A_M);
    d = d & (~(3<<2));
    d = d | (rateBits << 2);
    write8(LSM303_ADDRESS_MAG, CFG_REG_A_M, d);
}

int GrinlizLSM303::getMagDataRate() const
{
    uint8_t d = read8(LSM303_ADDRESS_MAG, CFG_REG_A_M) & (3<<2);
    d >>= 2;
    switch(d) {
        case 0: return 10;
        case 1: return 20;
        case 2: return 50;
        default: break;
    }  
    return 100;
}

int GrinlizLSM303::available()
{
    uint8_t fifosrc = read8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_FIFO_SRC_REG_A);
    uint8_t overrun = fifosrc & (1 << 6);
    uint8_t avail = fifosrc & 31;

    return avail + (overrun ? 1 : 0);
}

int GrinlizLSM303::flush(int n) 
{
    return readInner(0, 0, n);
}


int GrinlizLSM303::readInner(Vec3<int32_t>* rawData, Vec3<float>* data, int n)
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

        int16_t x = xlo | (xhi << 8);
        int16_t y = ylo | (yhi << 8);
        int16_t z = zlo | (zhi << 8);

        if (rawData) {
            rawData[i].x = x;
            rawData[i].y = y;
            rawData[i].z = z;
        }
        if (data) {
            static const float divInv = 1.0f / float(DIV);
            data[i].x = x * divInv;
            data[i].y = y * divInv;
            data[i].z = z * divInv;
        }
    }
    return n;
}

void GrinlizLSM303::recalibrateMag()
{
    if (dataValid(m_minQueued, m_maxQueued) && dataValid(m_min, m_max)) {
        Vec3<int32_t> a = m_max - m_min;
        int errA = a.x + a.y + a.z;
        
        Vec3<int32_t> b = m_maxQueued - m_minQueued;
        int errB = b.x + b.y + b.z;

        if (errB < errA) {
            m_min = m_minQueued;
            m_max = m_maxQueued;
            m_samplesSinceSwap = 0;
            Log.p("mag recalibrated.").eol();
        }
        m_minQueued.set(INT_MAX, INT_MAX, INT_MAX);
        m_maxQueued.set(INT_MIN, INT_MIN, INT_MIN);
    }
}

int GrinlizLSM303::readMag(Vec3<int32_t>* rawData, Vec3<float>* data)
{
    uint8_t status = read8(LSM303_ADDRESS_MAG, STATUS_REG_M);
    if ((status & (1<<3)) == 0 && (status & (1<<7)) == 0) return 0;

    Wire.beginTransmission(LSM303_ADDRESS_MAG);
    Wire.write(OUTX_LOW_REG_M);
    Wire.endTransmission();
    Wire.requestFrom(LSM303_ADDRESS_MAG, 6);

    // Wait around until enough data is available
    while (Wire.available() < 6);

    // Note high before low (different than accel)  
    uint16_t xlo = Wire.read();
    uint16_t xhi = Wire.read();
    uint16_t ylo = Wire.read();
    uint16_t yhi = Wire.read();
    uint16_t zlo = Wire.read();
    uint16_t zhi = Wire.read();
    
    // Shift values to create properly formed integer (low byte first)
    int32_t x = int16_t(xlo | (xhi << 8));
    int32_t y = int16_t(ylo | (yhi << 8));
    int32_t z = int16_t(zlo | (zhi << 8));

    m_min.x = glMin(x, m_min.x);
    m_min.y = glMin(y, m_min.y);
    m_min.z = glMin(z, m_min.z);

    m_max.x = glMax(x, m_max.x);
    m_max.y = glMax(y, m_max.y);
    m_max.z = glMax(z, m_max.z);

    if (!magDataValid())
        return 0;

    ++m_samplesSinceSwap;
    if (m_samplesSinceSwap > m_queueFrame) {
        m_minQueued.x = glMin(x, m_minQueued.x);
        m_minQueued.y = glMin(y, m_minQueued.y);
        m_minQueued.z = glMin(z, m_minQueued.z);

        m_maxQueued.x = glMax(x, m_maxQueued.x);
        m_maxQueued.y = glMax(y, m_maxQueued.y);
        m_maxQueued.z = glMax(z, m_maxQueued.z);
    }

    if (rawData) {
        rawData->x = x;
        rawData->y = y;
        rawData->z = z; 
    }
    if (data) {
        float vx = -1.0f + 2.0f * (x - m_min.x) / (m_max.x - m_min.x);
        float vy = -1.0f + 2.0f * (y - m_min.y) / (m_max.y - m_min.y);
        float vz = -1.0f + 2.0f * (z - m_min.z) / (m_max.z - m_min.z);

        float len2 = vx * vx + vy * vy + vz * vz;
        float lenInv = 1.0f / sqrtf(len2);
        data->x = vx * lenInv;
        data->y = vy * lenInv;
        data->z = vz * lenInv;
    }
    return 1;
}

void GrinlizLSM303::write8(uint8_t address, uint8_t reg, uint8_t value) const
{
    Wire.beginTransmission(address);
    Wire.write((uint8_t)reg);
    Wire.write((uint8_t)value);
    Wire.endTransmission();
}

uint8_t GrinlizLSM303::read8(uint8_t address, uint8_t reg) const
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

