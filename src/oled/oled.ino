#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define PIN_SDCARD_CS     10
#define PIN_SDCARD_MOSI   11
#define PIN_SDCARD_MISO   12
#define PIN_SDCARD_SCK    13
#define PIN_OLED_DC       14
#define PIN_VMETER        15
#define PIN_OLED_RESET    16 
#define PIN_OLED_CS       17
#define PIN_SDA           18
#define PIN_SCL           19

/* Uncomment this block to use hardware SPI */
Adafruit_SSD1306 display(PIN_OLED_DC, PIN_OLED_RESET, PIN_OLED_CS);

static const uint8_t PROGMEM BITMAP[] = {
  B1111000, B00000000
};

void setup()   
{                
  display.begin(SSD1306_SWITCHCAPVCC);
  display.display();

  display.clearDisplay();
  //display.drawBitmap(10, 10, BITMAP, 8, 2, 1);
  //display.drawRect(0, 0, 128, 32, 1);
  display.drawCircle(15, 15, 15, 1);
  display.drawCircle(128-16, 15, 15, 1);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(36,0);
  display.println("Hello Jedi");

  uint8_t* buffer = display.getBuffer();
  *buffer = 0xff;
  
  display.display();
}

void loop()
{
  
}

