#ifndef DRAW_INCLUDED
#define DRAW_INCLUDED

#include <stdint.h>

const uint8_t* get_dial0(int* width, int* height);
const uint8_t* get_dial1(int* width, int* height);
const uint8_t* get_dial2(int* width, int* height);
const uint8_t* get_dial3(int* width, int* height);
const uint8_t* get_dial4(int* width, int* height);

const uint8_t* getGlypth_consolas(int charID, int* advance, int* w, int* rows);
const uint8_t* getGlypth_aurekBesh7(int charID, int* advance, int* w, int* rows);
const uint8_t* getGlypth_aurekBesh6(int charID, int* advance, int* w, int* rows);

#endif // DRAW_INCLUDED
