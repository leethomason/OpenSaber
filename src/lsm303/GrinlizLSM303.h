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
    int readMag(RawData* rawData, float* x, float* y, float* z);
    int setMagCalibration(int x0, int y0, int z0, int x1, int y1, int z1) { 
        this->x0 = x0; this->y0 = y0; this->z0 = z0;
        this->x1 = x1; this->y1 = y1; this->z1 = z1;    
    }

    void logStatus();

    int minX = 1000, minY = 1000, minZ = 1000;
    int maxX = -1000, maxY = -1000, maxZ = -1000;
    int x0=0, y0=0, z0=0, x1=0, y1=0, z1=0;

private:

    void testMag();
    int available();
    void write8(uint8_t address, uint8_t reg, uint8_t value);
    uint8_t read8(uint8_t address, uint8_t reg);
};



