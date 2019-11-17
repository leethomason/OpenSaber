#include <OLED_SSD1306.h>

#include "vrender.h"
#include "vectorui.h"

#include "Grinliz_Arduino_Util.h"

#define PIN_OLED_DC 7
#define PIN_OLED_RESET 9
#define PIN_OLED_CS 10
OLED_SSD1306 display(PIN_OLED_DC, PIN_OLED_RESET, PIN_OLED_CS);

static const int OLED_WIDTH = 128;
static const int OLED_WIDTH_SHIFT = 7;
static const int OLED_HEIGHT = 32;
static const int OLED_BYTES = OLED_WIDTH * OLED_HEIGHT / 8;

uint8_t oledBuffer[OLED_BYTES] = {0};
VRender vrender;
UIRenderData data;
uint32_t lastProfile = 0;
int nFrames = 0;

/*
FPS=217 time/frame=4 size=2620
Profile:
  display              aveTime=1.48ms maxTime=1.48ms nCalls=434
  draw                 aveTime=1.13ms maxTime=1.26ms nCalls=434
  VRender::Rasterize   aveTime=1.79ms maxTime=1.86ms nCalls=434

ActiveEdge only; doesn't hold on to edges. Simpler. Smaller.
Perf is a wash.
FPS=215 time/frame=4 size=2012
Profile:
  display              aveTime=1.50ms maxTime=1.51ms nCalls=430
  draw                 aveTime=1.46ms maxTime=1.61ms nCalls=430
  VRender::Rasterize   aveTime=1.55ms maxTime=1.61ms nCalls=430

Vector numbers. Text is always such trouble.
FPS=193 time/frame=5 size=3012 edges=90/200
Profile:
  display              aveTime=1.50ms maxTime=1.51ms nCalls=386
  draw                 aveTime=1.14ms maxTime=1.20ms nCalls=386
  VRender::Rasterize   aveTime=2.41ms maxTime=2.55ms nCalls=386

ActiveEdge only, immediate text, simpler callback.
FPS=230 time/frame=4 size=1612 edges=64/100
Profile:
  display              aveTime=1.50ms maxTime=1.51ms nCalls=461
  draw                 aveTime=1.28ms maxTime=1.36ms nCalls=461
  VRender::Rasterize   aveTime=1.43ms maxTime=1.56ms nCalls=461
*/

void BlockDrawOLED(const BlockDrawChunk* chunks, int n)
{
    // Clear to black beforehand; don't need to set black runs.
    for (int i = 0; i < n; ++i) {
        const BlockDrawChunk& chunk = chunks[i];

#ifdef VECTOR_MONO
        if (chunk.rgb == 0) continue;
#else
        if (chunk.rgb.get() == 0) continue;
#endif
        // Simple for the single row.
        int row = chunk.y >> 3;
        int bit = chunk.y - (row << 3);
        uint8_t mask = 1 << bit;
        uint8_t* p = oledBuffer + (row << OLED_WIDTH_SHIFT) + chunk.x0;
        for (int nPix = chunk.x1 - chunk.x0; nPix > 0; nPix--, p++) {
            *p |= mask;
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
    data.mVolts += 1;
    if (data.mVolts == 4200) data.mVolts = 3200;
    memset(oledBuffer, 0, OLED_BYTES);
    {
        static ProfileData profileData("draw");
        ProfileBlock block(&profileData);
        VectorUI::Draw(&vrender, millis(), UIMode::NORMAL, false, &data);
    }
    {
        vrender.Render();
    }
    {
        static ProfileData profileData("display");
        ProfileBlock block(&profileData);
        display.display(oledBuffer);
    }

    ++nFrames;
    static const int SEC = 2;
    if (millis() - lastProfile > 1000 * SEC) {
        lastProfile = millis();
        #if SERIAL_DEBUG > 0
        Log.p("SERIAL ON").eol();
        #endif
        Log.p("FPS=").p(nFrames / SEC).p(" time/frame=").p(SEC * 1000 / nFrames)
            .p(" size=").p(sizeof(VRender))
            .p(" edges=").p(vrender.NumEdges()).p("/").p(VRender::MAX_EDGES)
            .eol();
        DumpProfile();
        nFrames = 0;
    }
}
