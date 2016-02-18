#ifndef DRAW_INCLUDED
#define DRAW_INCLUDED

#include <stdint.h>

const uint8_t* get_hw(int* width, int* height);
const uint8_t* get_tt(int* width, int* height);

const uint8_t* getGlypth_consolas(int charID, int* advance, int* w, int* rows);
const uint8_t* getGlypth_aurekBesh(int charID, int* advance, int* w, int* rows);

#endif // DRAW_INCLUDED
