#ifndef DRAW_INCLUDED
#define DRAW_INCLUDED

#include <stdint.h>

const uint8_t* get_dial0(int* width, int* height);
const uint8_t* get_dial1(int* width, int* height);
const uint8_t* get_dial2(int* width, int* height);
const uint8_t* get_dial3(int* width, int* height);
const uint8_t* get_dial4(int* width, int* height);

// Created using the DotFactory.
// Fonts:
//		Y: fixed
//		X: tightest
//		Bit layout: Column Major
//		Bit order: MSB first

const uint8_t* getGlypth_aurekBesh6(int charID, int* advance, int* w, int* rows);
const uint8_t* getGlypth_calibri8(int charID, int* advance, int* w, int* rows);
const uint8_t* getGlypth_tomThumb5(int charID, int* advance, int* w, int* rows);

#endif // DRAW_INCLUDED
