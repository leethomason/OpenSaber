#pragma once

#include <stdint.h>

class GrinlizLSM303
{
public:
    GrinlizLSM303() {}

    void begin();

    void write8(uint8_t address, uint8_t reg, uint8_t value);
    uint8_t read8(uint8_t address, uint8_t reg);

};



