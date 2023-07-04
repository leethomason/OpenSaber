#pragma once

#include <stdint.h>

class ShiftedReg
{
public:
    ShiftedReg() = default;
    void begin(int latchPin, int clockPin, int dataPin) {
        m_latchPin = latchPin;
        m_clockPin = clockPin;
        m_dataPin = dataPin;
    }

    // This sets as if a1 is the low bit of the byte and b1 is low bit of the high (second) byte.
    void set(uint32_t value);

private:
    int m_latchPin = 0;
    int m_clockPin = 0;
    int m_dataPin = 0;
};