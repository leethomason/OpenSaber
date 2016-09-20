#include "isin.h"
#include <math.h>

static int16_t gSinTable[256] = { 0 };

int16_t isin(uint16_t x)
{
	if (gSinTable[64] == 0) {
		for (int i = 0; i < 256; ++i) {
			float x = 2 * 3.14159f * float(i) / 256.0f;
			gSinTable[i] = int16_t(sin(x) * 256.0f);
		}
	}
	return gSinTable[x & 0xff];
}
