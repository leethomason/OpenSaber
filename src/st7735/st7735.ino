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

//uint16_t gMemory[160];

/*
void BlockDrawMem(const BlockDrawChunk *chunks, int yIn, int n)
{
    for(int i=0; i<n; ++i) {
        const BlockDrawChunk& chunk = chunks[i];
        uint16_t c =  ((chunk.rgb.r >> 3) << 0) 
            | ((chunk.rgb.g >> 2) << 5)
            | ((chunk.rgb.b >> 3) << 11);  

        int subN = chunk.x1 - chunk.x0;
        uint16_t* p = gMemory + yIn * 160 + chunk.x0;
        while(subN--) {
            *p++ = c;
        }
    }
}
*/
void BlockDrawST7735(const BlockDrawChunk *chunks, int y, int n)
{
    for(int i=0; i<n; ++i) {
        const BlockDrawChunk& chunk = chunks[i];

//        tft.setAddrWindow(x0, y, x1 - x0, 1);
        uint16_t c =  ((chunk.rgb.r >> 3) << 0) 
                    | ((chunk.rgb.g >> 2) << 5)
                    | ((chunk.rgb.b >> 3) << 11);  

        /*
        for(int x=chunk.x0; x<chunk.x1; ++x) {
            gMemory[x] = c;
        }
        if (chunk.x1 == 160) {
            tft.writePixels(gMemory, 160);
        }
        */

        // Neither approach helps. Calculating, we seem off
        // by the theoretical SPI throughput of a factor of 10.
        // 16,000,000 bits/second
        // bits needed = 16 * 80 * 160 = 204,800
        // update = 78 fps
        // 
        // but see 40/7 = 6 fps ??   
        // hmm. taking out the window in the inner loop helps:
        // 40/5 = 8 fps yay.
        //
        // The spec doesn't specify a block write. DMA?     

        int len = chunk.x1 - chunk.x0;
        while(len--)
            SPI.transfer16(~c);                  
	    //tft.writeColor(~c, x1 - x0);
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
/*
	for (int i = 0; i < 10; ++i)
	{
		static int M = 4;
		tft.startWrite();
		tft.setAddrWindow(0, i * M, 160, 80 - i * M);
		tft.writeColor(0xabcd, 160);
		tft.endWrite();
	}*/

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

/*
    vRender.Attach(BlockDrawMem);
    vRender.Render();    
    tft.startWrite();
    tft.setAddrWindow(0, 0, 160, 80);
    tft.writePixels(gMemory, 160*80);
    tft.endWrite();
*/
/*
    tft.startWrite();
    if (yPos > 0) {
        tft.setAddrWindow(0, 0, 160, yPos);
        tft.writeColor(0xffff, yPos * 160);
    }
    tft.setAddrWindow(0, yPos, 160, 40);
    tft.writeColor(0, 40 * 160);
    if (yPos < 40) {
        tft.setAddrWindow(0, yPos + 40, 160, yPos);
        tft.writeColor(0xffff, (80 - yPos) * 160);
    }
*/

/*	for (int i = 0; i < 80; ++i)
	{
		tft.setAddrWindow(1, i+2, 160, 1);
		tft.writeColor(i < yPos || i > yPos + 40 ? 0xffff : 0, 160);
	}
*/    
    tft.endWrite();


    //delay(100);
}
