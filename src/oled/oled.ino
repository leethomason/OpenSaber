#include <OLED_SSD1306.h>

#include "vrender.h"
#include "vectorui.h"

#include "Grinliz_Arduino_Util.h"

#define PIN_OLED_DC 7
#define PIN_OLED_RESET 9
#define PIN_OLED_CS 10
OLED_SSD1306 display(PIN_OLED_DC, PIN_OLED_RESET, PIN_OLED_CS);

static const int OLED_WIDTH = 128;
static const int OLED_HEIGHT = 32;
static const int OLED_BYTES = OLED_WIDTH * OLED_HEIGHT / 8;

uint8_t oledBuffer[OLED_BYTES] = {0};
VRender vrender;
UIRenderData data;
uint32_t lastProfile = 0;
int nFrames = 0;

void BlockDrawOLED(const BlockDrawChunk* chunks, int y, int n)
{
    for (int i = 0; i < n; ++i) {
        const BlockDrawChunk& chunk = chunks[i];
        int row = y / 8;
        int bit = 1 << (y & 7);

        if (chunk.rgb.get()) {
            uint8_t* p = oledBuffer + row * OLED_WIDTH + chunk.x0;
            for(int nPix = chunk.x1 - chunk.x0; nPix > 0; nPix--, p++) {
                 *p |= bit;
            }
        }
    }
}


void setup()
{
    delay(100);
    Serial.begin(19200);
    Log.attachSerial(&Serial);
    delay(100);

    static const uint8_t BYTES[3] = {0xff, 0, 0x24};
    for(int i=0; i<OLED_BYTES; ++i) {
        oledBuffer[i] = BYTES[i%3]; 
    }
    display.begin(OLED_WIDTH, OLED_HEIGHT, SSD1306_SWITCHCAPVCC);

    vrender.Attach(BlockDrawOLED);
    vrender.SetClip(VRender::Rect(0, 0, OLED_WIDTH, OLED_HEIGHT));
    vrender.SetSize(OLED_WIDTH, OLED_HEIGHT);

    delay(100);

    data.color.set(0, 255, 0);
    data.fontName = "none";
    data.mVolts = 3900;
    data.palette = 0;
    data.volume = 2;
    memset(oledBuffer, 0, OLED_BYTES);
    VectorUI::Draw(&vrender, 0, UIMode::NORMAL, false, &data);

    display.display(oledBuffer);
    lastProfile = millis();
}

void loop()
{
    data.mVolts += 1;
    if (data.mVolts == 4200) data.mVolts = 3200;
    memset(oledBuffer, 0, OLED_BYTES);
    VectorUI::Draw(&vrender, 0, UIMode::NORMAL, false, &data);

    {
        static ProfileData data("display");
        ProfileBlock block(&data);
        display.display(oledBuffer);
    }

    ++nFrames;
    if (millis() - lastProfile > 2000) {
        lastProfile = millis();
        #if SERIAL_DEBUG > 0
        Log.p("SERIAL ON").eol();
        #endif
        Log.p("FPS=").p(nFrames / 2).eol();
        DumpProfile();
        nFrames = 0;
    }
}
