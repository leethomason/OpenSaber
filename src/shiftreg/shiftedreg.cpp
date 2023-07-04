#include "shiftedreg.h" 
#include <Arduino.h>

void ShiftedReg::set(uint32_t value) {
    digitalWrite(m_latchPin, LOW);
    shiftOut(m_dataPin, m_clockPin, MSBFIRST, ((value >> 8) & 0xff) << 1);
    shiftOut(m_dataPin, m_clockPin, MSBFIRST, (value & 0xff) << 1);
    digitalWrite(m_latchPin, HIGH);
}
