#include <OLED_SSD1306.h>

#define PIN_OLED_DC 7
#define PIN_OLED_RESET 9
#define PIN_OLED_CS 10
OLED_SSD1306 display(PIN_OLED_DC, PIN_OLED_RESET, PIN_OLED_CS);

static const int OLED_WIDTH = 128;
static const int OLED_HEIGHT = 32;
static const int OLED_BYTES = OLED_WIDTH * OLED_HEIGHT / 8;
uint8_t oledBuffer[OLED_BYTES] = {0};

void setup()
{
    delay(100);
    static const uint8_t BYTES[3] = {0xff, 0, 0x24};
    for(int i=0; i<OLED_BYTES; ++i) {
        oledBuffer[i] = BYTES[i%3]; 
    }
    display.begin(OLED_WIDTH, OLED_HEIGHT, SSD1306_SWITCHCAPVCC);
    delay(100);
    display.display(oledBuffer);
}

void loop()
{

}