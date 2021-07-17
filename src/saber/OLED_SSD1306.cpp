#include <SPI.h>
#include "OLED_SSD1306.h"

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E

const SPISettings oledSettings(8000000, MSBFIRST, SPI_MODE0);

// constructor for hardware SPI
OLED_SSD1306::OLED_SSD1306(int8_t DC, int8_t RST, int8_t CS) {
    dc = DC;
    rst = RST;
    cs = CS;
}

void OLED_SSD1306::begin(int width, int height, uint8_t vccstate, bool reset) {
    _vccstate = vccstate;
    pinMode(dc, OUTPUT);
    pinMode(cs, OUTPUT);
    this->width = width;
    this->height = height;

    SPI.begin();
    SPI.beginTransaction(oledSettings);

    if ((reset) && (rst >= 0)) {
        // Setup reset pin direction (used by both SPI and I2C)
        pinMode(rst, OUTPUT);
        digitalWrite(rst, HIGH);
        // VDD (3.3V) goes high at start, lets just chill for a ms
        delay(1);
        // bring reset low
        digitalWrite(rst, LOW);
        // wait 10ms
        delay(10);
        // bring out of reset
        digitalWrite(rst, HIGH);
        // turn on VCC (9V?)
    }

    // Init sequence
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                                  // the suggested ratio 0x80

    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(height - 1);

    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0);                                   // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    if (vccstate == SSD1306_EXTERNALVCC)
    {
        ssd1306_command(0x10);
    }
    else
    {
        ssd1306_command(0x14);
    }
    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);                                  // 0x0 act like ks0108
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);

    if( width == 128 && height == 32) {
        ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
        ssd1306_command(0x02);
        ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
        ssd1306_command(0x8F);
    }
    else if (width == 128 && height == 64) {
        ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
        ssd1306_command(0x12);
        ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
        if (vccstate == SSD1306_EXTERNALVCC)
        {
            ssd1306_command(0x9F);
        }
        else
        {
            ssd1306_command(0xCF);
        }
    }
    else if (width == 96 && height == 16) {
        ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
        ssd1306_command(0x2);   //ada x12
        ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
        if (vccstate == SSD1306_EXTERNALVCC)
        {
            ssd1306_command(0x10);
        }
        else
        {
            ssd1306_command(0xAF);
        }
    }

    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    if (vccstate == SSD1306_EXTERNALVCC)
    {
        ssd1306_command(0x22);
    }
    else
    {
        ssd1306_command(0xF1);
    }
    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6

    ssd1306_command(SSD1306_DEACTIVATE_SCROLL);

    ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel
    SPI.endTransaction();
}


void OLED_SSD1306::invertDisplay(uint8_t i) {
    SPI.beginTransaction(oledSettings);
    if (i) {
        ssd1306_command(SSD1306_INVERTDISPLAY);
    } else {
        ssd1306_command(SSD1306_NORMALDISPLAY);
    }
    SPI.endTransaction();
}

// private; already did beginTransaction()
void OLED_SSD1306::ssd1306_command(uint8_t c) {
    digitalWrite(cs, HIGH);
    digitalWrite(dc, LOW);
    digitalWrite(cs, LOW);
    SPI.transfer(c);
    digitalWrite(cs, HIGH);
}

void OLED_SSD1306::display(const uint8_t* buffer) {
    SPI.beginTransaction(oledSettings);
    ssd1306_command(SSD1306_COLUMNADDR);
    ssd1306_command(0);   // Column start address (0 = reset)
    ssd1306_command(width-1); // Column end address (127 = reset)

    ssd1306_command(SSD1306_PAGEADDR);
    ssd1306_command(0); // Page start address (0 = reset)
    if(height == 64) {
        ssd1306_command(7); // Page end address
    }
    else if (height == 32) {
        ssd1306_command(3); // Page end address
    }
    else if (height == 16) {
        ssd1306_command(1); // Page end address
    }

    digitalWrite(cs, HIGH);
    digitalWrite(dc, HIGH);
    digitalWrite(cs, LOW);

    const uint16_t size = width * height / 8;

    for (uint16_t i=0; i<size; i++) {
        SPI.transfer(buffer[i]);
    }
    digitalWrite(cs, HIGH);
    SPI.endTransaction();
}
