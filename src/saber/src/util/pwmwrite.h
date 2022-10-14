#pragma once

#include <stdint.h>

void InitPWM();

void PWMWrite(int pin, uint8_t pwm);

// Converts from linear color to power.
void PWMWriteColor(int pin, uint8_t color);
// Range [0,1000]
void PWMWrite1000(int pin, uint32_t pwm);
// Range [0,1024) 
void PWMWrite1024(int pin, uint32_t pwm);