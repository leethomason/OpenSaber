#ifndef PIXEL_MATRIX_INCLUDED
#define PIXEL_MATRIX_INCLUDED

#include <stdint.h>

class PixelMatrix
{
public:
    PixelMatrix();

    void loop(uint32_t time, const uint8_t* display);
private:
};

#endif // PIXEL_MATRIX_INCLUDED
