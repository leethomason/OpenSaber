#ifndef ISIN_INCLUDED
#define ISIN_INCLUDED

#include <stdint.h>

// Input: 0-255 (range will be clipped correctly.
// Output:: -256 - 256
int16_t isin(uint16_t x);

#endif // ISIN_INCLUDED
