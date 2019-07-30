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
    int readInner(RawData* rawData, Data* data, int n);
    
private:
    int available();
    void write8(uint8_t address, uint8_t reg, uint8_t value);
    uint8_t read8(uint8_t address, uint8_t reg);
};



