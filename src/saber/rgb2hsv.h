#ifndef RGB_TO_HSV_INCLUDED
#define RGB_TO_HSV_INCLUDED

#include <stdint.h>

// hue:        [0, 180)
// saturation: [0, 255]
// value:      [0, 255]

void rgb2hsv(
    uint8_t src_r, uint8_t src_g, uint8_t src_b,
    uint8_t* dst_h, uint8_t* dst_s, uint8_t* dst_v);

void hsv2rgb(
    uint8_t src_h, uint8_t src_s, uint8_t src_v,
    uint8_t* dst_r, uint8_t* dst_g, uint8_t* dst_b);


#endif // RGB_TO_HSV_INCLUDED

