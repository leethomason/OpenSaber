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

//#define USE_SPLIT
#ifdef USE_SPLIT
// goes from 4.6 for full frame to 3.5 for half frame. Not a huge win.
// probably just ping ponging render vs. transfer will do.
int split = 0;
#endif

// 4.6 good, VECTOR_MONO
// 3.9 was...callback improvements? An error implementing immediate mode is still faster.
/*
// Immediate
FPS=167 time/frame=5
Profile:
  display              aveTime=1.45ms maxTime=1.46ms nCalls=335
  draw                 aveTime=3.33ms maxTime=3.66ms nCalls=335
  VRender::Rasterize   aveTime=1.02ms maxTime=1.02ms nCalls=335

// Classic
FPS=151 time/frame=6
Profile:
  display              aveTime=1.47ms maxTime=1.48ms nCalls=303
  draw                 aveTime=1.00ms maxTime=1.11ms nCalls=303
  VRender::Rasterize   aveTime=3.87ms maxTime=4.45ms nCalls=303

// Immediate w/ mask table
PS=164 time/frame=6
Profile:
  display              aveTime=1.47ms maxTime=1.48ms nCalls=328
  draw                 aveTime=3.44ms maxTime=3.82ms nCalls=328
  VRender::Rasterize   aveTime=1.02ms maxTime=1.02ms nCalls=328

  Nothing really moving the needle. 
*/


void BlockDrawOLED(const BlockDrawChunk* chunks, int n)
{
    static const uint8_t MASK0[] = { 1, 3, 7, 15, 31, 63, 127, 255 };
    static const uint8_t MASK1[] = { 2, 6, 14, 30, 62, 126, 254 };
    static const uint8_t MASK2[] = { 4, 12, 28, 60, 124, 252 };
    static const uint8_t MASK3[] = { 8, 24, 56, 120, 248 };
    static const uint8_t MASK4[] = { 16, 48, 112, 240 };
    static const uint8_t MASK5[] = { 32, 96, 224 };
    static const uint8_t MASK6[] = { 64, 192 };
    static const uint8_t MASK7[] = { 128 };
    static const uint8_t* MASK[] = {
        MASK0, MASK1, MASK2, MASK3, MASK4, MASK5, MASK6, MASK7
    };

    // Clear to black beforehand; don't need to set black runs.
    for (int i = 0; i < n; ++i) {
        const BlockDrawChunk& chunk = chunks[i];

#ifdef VECTOR_MONO
        if (chunk.rgb == 0) continue;
#else
        if (chunk.rgb.get() == 0) continue;
#endif
        // Simple for the single row.
        if (chunk.y0 == chunk.y1 - 1) {
            int row = chunk.y0 / 8;
            int bit = chunk.y0 - row * 8;
            uint8_t mask = 1 << bit;
            uint8_t* p = oledBuffer + row * OLED_WIDTH + chunk.x0;
            for (int nPix = chunk.x1 - chunk.x0; nPix > 0; nPix--, p++) {
                *p |= mask;
            }
        }
        else {
            int row0 = chunk.y0 / 8;
            int row1 = (chunk.y1 + 7) / 8;

            for (int r = row0; r < row1; ++r) {
                int bit0 = chunk.y0 - r * 8;
                int bit1 = chunk.y1 - r * 8;
                bit0 = glMax(bit0, 0);
                bit1 = glMin(bit1, 8);
#if false
                uint8_t mask = 0;
                for (int b = bit0; b < bit1; ++b) {
                    mask |= 1 << b;
                }
#else
                const uint8_t* m = MASK[bit0];
                uint8_t mask = m[bit1 - bit0 - 1];
#endif
                uint8_t* p = oledBuffer + r * OLED_WIDTH + chunk.x0;
                for (int nPix = chunk.x1 - chunk.x0; nPix > 0; nPix--, p++) {
                    *p |= mask;
                }
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
    vrender.Render();

    display.display(oledBuffer);
    lastProfile = millis();
}

void loop()
{
    #ifdef USE_SPLIT
    if (split == 0) {
        data.mVolts += 1;
        if (data.mVolts == 4200) data.mVolts = 3200;
        memset(oledBuffer, 0, OLED_BYTES);
        VectorUI::Draw(&vrender, 0, UIMode::NORMAL, false, &data);
    }
    else if (split == 1) {
        vrender.Render();
    }
    else if (split == 2) {
        static ProfileData data("display");
        ProfileBlock block(&data);
        display.display(oledBuffer);
    }
    split++;
    if (split == 3) split = 0;

    #else
    data.mVolts += 1;
    if (data.mVolts == 4200) data.mVolts = 3200;
    memset(oledBuffer, 0, OLED_BYTES);
    {
        static ProfileData profileData("draw");
        ProfileBlock block(&profileData);
        VectorUI::Draw(&vrender, 0, UIMode::NORMAL, false, &data);
    }
    {
        vrender.Render();
    }
    {
        static ProfileData profileData("display");
        ProfileBlock block(&profileData);
        display.display(oledBuffer);
    }
    #endif

    ++nFrames;
    static const int SEC = 2;
    if (millis() - lastProfile > 1000 * SEC) {
        lastProfile = millis();
        #if SERIAL_DEBUG > 0
        Log.p("SERIAL ON").eol();
        #endif
        Log.p("FPS=").p(nFrames / SEC).p(" time/frame=").p(SEC * 1000 / nFrames).eol();
        DumpProfile();
        nFrames = 0;
    }
}
