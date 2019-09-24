#pragma once

#include <stdint.h>

class GrinlizLSM303
{
public:
    struct RawData {
        int16_t x, y, z;
    };
    struct Data {
        float ax, ay, az;
    };

    GrinlizLSM303() {}

    bool begin();

    int read(Data* data, int nData) { return readInner(0, data, nData); }
    int readRaw(RawData* data, int nData) { return readInner(data, 0, nData); }
    int readInner(RawData* rawData, Data* data, int n);

    /* In my test bed, the rawData isn't even close to being correctly pre-set.
       the ranges [-400, 200] and [-500, 90] have come up. Also totally 
       possible that the presense of non-earth magnetic fields throws it off.

       The min/max is tracked by the library, and the x/y/z values are probably
       the ones to use.
    */
    int readMag(RawData* rawData, float* x, float* y, float* z);

    void setMagCalibration(int x0, int y0, int z0, int x1, int y1, int z1) { 
        this->x0 = x0; this->y0 = y0; this->z0 = z0;
        this->x1 = x1; this->y1 = y1; this->z1 = z1;    
    }

    void getMagCalibration(int *x0, int* y0, int* z0, int* x1, int* y1, int* z1)
    {
        *x0 = this->x0; *y0 = this->y0; *z0 = this->z0;
        *x1 = this->x1; *y1 = this->y1; *z1 = this->z1;
    }

    void logMagStatus();

    // Reduces power consumption at lower frequency
    void setMagDataRate(int hz);   // 10, 20, 50, 100
    int getMagDataRate() const;

private:
    int x0 = 0, y0 = 0, z0 = 0, x1 = 0, y1 = 0, z1 = 0;

    int available();
    void write8(uint8_t address, uint8_t reg, uint8_t value) const;
    uint8_t read8(uint8_t address, uint8_t reg) const;
};
