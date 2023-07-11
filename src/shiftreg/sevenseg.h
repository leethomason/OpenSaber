#pragma once

#include "shiftedreg.h"

#include <stdint.h>

class SevenSeg
{
public:
    SevenSeg() {}
    void begin(int latchPin, int clockPin, int dataPin) { m_reg.begin(latchPin, clockPin, dataPin); }
    void set(const char* str);
    // 4 or greater to disable
    void setDP(uint8_t index) { m_dp = index; }
    void loop(uint32_t  msec);

private:
    ShiftedReg m_reg;
    uint8_t m_dp = 4;
    uint16_t m_value[4];
};