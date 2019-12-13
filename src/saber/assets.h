#ifndef DRAW_INCLUDED
#define DRAW_INCLUDED

#include <stdint.h>

const uint8_t* get_jBird(int* width, int* height);

// Created using the DotFactory.
// Fonts:
//		Y: fixed
//		X: tightest
//		Bit layout: Column Major
//		Bit order: MSB first
// Images:
//		Bit layout: Column Major
//		Bit order: MSB first

const uint8_t* getGlypth_aurekBesh6(int charID, int* advance, int* w, int* rows);
const uint8_t* getGlypth_calibri8(int charID, int* advance, int* w, int* rows);

void getGlypth_tomThumb5(int charID, uint8_t* glyph, int* advance=0, int* w=0, int* rows=0);
const uint8_t* getGlypth_tomThumb5(int charID, int* advance, int* w, int* rows);

#endif // DRAW_INCLUDED
