#pragma once

#include <stdint.h>

// 0-1000 scale
uint16_t LinearToPWM1000(uint8_t linear);
// 0-1023 scale
uint16_t LinearToPWM1024(uint8_t linear);