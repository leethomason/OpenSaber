#ifndef PIXEL_MATRIX_INCLUDED
#define PIXEL_MATRIX_INCLUDED

#include <stdint.h>

class PixelMatrix
{
public:
    PixelMatrix();

    void loop(uint32_t time);
private:
//    void render(uint8_t* g0, uint8_t* g1);
};

#endif // PIXEL_MATRIX_INCLUDED
