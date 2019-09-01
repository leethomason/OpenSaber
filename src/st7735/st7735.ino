#include <Adafruit_GFX.h>	// Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#include "vrender.h"
#include "vectorui.h"

// For the breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.
#define TFT_CS 10
#define TFT_RST 9 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 7

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
VRender vRender;

void BlockDrawST7735(const BlockDrawChunk *chunks, int y, int n)
{
    for(int i=0; i<n; ++i) {
        const BlockDrawChunk& chunk = chunks[i];

        uint16_t c =  ((chunk.rgb.r >> 3) << 0) 
                    | ((chunk.rgb.g >> 2) << 5)
                    | ((chunk.rgb.b >> 3) << 11);  

        int len = chunk.x1 - chunk.x0;
        while(len--)
            SPI.transfer16(~c);                  
    }
}

void setup(void)
{
	Serial.begin(9600);
	Serial.print(F("Hello! ST77xx TFT Test"));

	// Use this initializer if using a 1.8" TFT screen:
	//tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

	// OR use this initializer (uncomment) if using a 1.44" TFT:
	//tft.initR(INITR_144GREENTAB); // Init ST7735R chip, green tab

	// OR use this initializer (uncomment) if using a 0.96" 180x60 TFT:
	tft.initR(INITR_MINI160x80); // Init ST7735S mini display

	// OR use this initializer (uncomment) if using a 1.3" or 1.54" 240x240 TFT:
	//tft.init(240, 240);           // Init ST7789 240x240

	// OR use this initializer (uncomment) if using a 2.0" 320x240 TFT:
	//tft.init(240, 320);           // Init ST7789 320x240

	tft.setRotation(3);
	Serial.println(F("Initialized"));

	uint16_t time = millis();
	tft.fillScreen(ST77XX_WHITE);
	time = millis() - time;

    vRender.Attach(BlockDrawST7735);
    vRender.SetSize(160, 80);
    vRender.SetClip(VRender::Rect(0, 0, 160, 80));

    vRender.DrawRect(0, 10, 40, 40, osbr::RGBA(255, 0, 0));
    vRender.DrawRect(50, 10, 40, 40, osbr::RGBA(0, 255, 0));
    vRender.DrawRect(120, 10, 40, 40, osbr::RGBA(0, 0, 255));

    tft.startWrite();
    vRender.Render();
    tft.endWrite();
}


int yPos = 0;

void loop()
{
    yPos++;
    if (yPos > 40) yPos = 0;

    static const int W = 160;
    static const int H = 82;

    vRender.Clear();
    vRender.SetSize(W, H);
    vRender.SetClip(VRender::Rect(0, 0, W, H));
    vRender.DrawRect(0, yPos, 40, 40, osbr::RGBA(255, 0, 0));
    vRender.DrawRect(50, yPos, 40, 40, osbr::RGBA(0, 255, 0));
    vRender.DrawRect(120, yPos, 40, 40, osbr::RGBA(0, 0, 255));

    tft.startWrite();
    tft.setAddrWindow(1, 0, W, H);
    vRender.Render();
    tft.endWrite();
}
