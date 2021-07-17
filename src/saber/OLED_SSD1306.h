
#ifndef _OLED_SSD1306_H_
#define _OLED_SSD1306_H_

#include "Arduino.h"
#include <SPI.h>

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

/**
 *  Class to manage sending a buffer of
 *  data to the OLED. (Note that drawing to
 *  the buffer needs to be handled separately.)
 *  Only hardware SPI is supported. Manages
 *  transactions and the CS line correctly.
 */
class OLED_SSD1306 {
public:
    OLED_SSD1306(int8_t DC, int8_t RST, int8_t CS);

    void begin(int width, int height, uint8_t switchvcc = SSD1306_SWITCHCAPVCC, bool reset=true);

    void invertDisplay(uint8_t i);

    /** Send a buffer to the display. The buffer
     *  must be WIDTH * HEIGHT / 8 bytes.
     */
    void display(const uint8_t* buffer);

    void dim(boolean dim);

private:
    void ssd1306_command(uint8_t c);
    int8_t _vccstate, sid, sclk, dc, rst, cs;
    uint16_t width, height;
};

#endif /* _OLED_SSD1306_H_ */
