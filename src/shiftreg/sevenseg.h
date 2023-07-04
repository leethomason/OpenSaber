#pragma once

#include "shiftedreg.h"

#include <stdint.h>

class SevenSeg
{
public:
    SevenSeg();
    void begin(int latchPin, int clockPin, int dataPin) { m_reg.begin(latchPin, clockPin, dataPin); }
    void set(const char* str);
    void loop(uint32_t  msec);

private:
    ShiftedReg m_reg;
    uint16_t m_value[4];
};