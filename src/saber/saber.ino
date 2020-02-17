/*
  Copyright (c) Lee Thomason, Grinning Lizard Software

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
  of the Software, and to permit persons to whom the Software is furnished to do
  so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

// #define PROFILE

// Arduino Libraries
#include <Adafruit_ZeroI2S.h>
#include <Adafruit_ZeroDMA.h>
#include <Adafruit_SPIFlash.h>
#include <Wire.h>
#include <SPI.h>
#include <OLED_SSD1306.h>

#include "Button.h"
#include "Grinliz_Arduino_Util.h"

// Includes
// -- Must be first. Has configuration. -- //
#include "pins.h"

#ifdef SABER_NUM_LEDS
    #include "DotStar.h"
#endif

#include "GrinlizLSM303.h"
#include "voltmeter.h"
#include "sfx.h"
#include "saberdb.h"
#include "cmdparser.h"
#include "blade.h"
#include "sketcher.h"
#include "renderer.h"
#include "saberUtil.h"
#include "tester.h"
#include "ShiftedSevenSeg.h"
#include "i2saudiodrv.h"
#include "manifest.h"
#include "vrender.h"
#include "vectorui.h"
#include "bladeflash.h"

using namespace osbr;

static const uint32_t INDICATOR_CYCLE         = 1000;
static const uint32_t IMPACT_MIN_TIME         = 1000;    // Minimum time between impact sounds. FIXME: need adjusting.

uint32_t lastMotionTime = 0;    
uint32_t lastImpactTime = 0;
uint32_t lastLoopTime   = 0;

Adafruit_FlashTransport_SPI flashTransport(SS1, &SPI1);
Adafruit_SPIFlash spiFlash(&flashTransport);
Adafruit_ZeroDMA dma;

Manifest manifest;
Adafruit_ZeroI2S i2s(PIN_I2S_LRCLK, PIN_I2S_BITCLK, PIN_I2S_DATA, 2);
I2SAudioDriver i2sAudioDriver(&dma, &i2s, &spiFlash, manifest);
SFX sfx(&i2sAudioDriver, manifest);

ButtonCB    buttonA(PIN_SWITCH_A, SABER_BUTTON);
LEDManager  ledA(PIN_LED_A, false);

UIRenderData uiRenderData;
BladeState  bladeState;
UIModeUtil  uiMode;
SaberDB     saberDB;
Voltmeter   voltmeter;
BladeFlash  bladeFlash;
CMDParser   cmdParser(&saberDB, manifest);
BladePWM    bladePWM;
Timer2      vbatTimer(Voltmeter::SAMPLE_INTERVAL);
Tester      tester;
AverageSample<Vec3<int32_t>, Vec3<int32_t>, 8> averageAccel(Vec3<int32_t>(0, 0, 0));

#ifdef SABER_NUM_LEDS
#ifdef SABER_UI_START
DotStarUI dotstarUI;                // It is the DotStarUI regardless of physical pixel protocol
#endif
osbr::RGBA leds[SABER_NUM_LEDS];    // Color of the LEDs (brightness is separate)
DotStar dotstar;                    // Hardware controller.
#endif

GrinlizLSM303 accelMag;
Timer2 displayTimer(100);
Timer2 vbatPrintTimer(1000);

#if SABER_DISPLAY == SABER_DISPLAY_128_32
static const int OLED_WIDTH = 128;
static const int OLED_WIDTH_SHIFT = 7;
static const int OLED_HEIGHT = 32;
uint8_t oledBuffer[OLED_WIDTH * OLED_HEIGHT / 8] = {0};

OLED_SSD1306 display(PIN_OLED_DC, PIN_OLED_RESET, PIN_OLED_CS);
VRender    renderer;
int renderStage = 0;
#elif SABER_DISPLAY == SABER_DISPLAY_7_5
Pixel_7_5_UI display75;
ShiftedDotMatrix dotMatrix;
#define SHIFTED_OUTPUT
#elif SABER_DISPLAY == SABER_DISPLAY_SEGMENT
ShiftedSevenSegment shifted7;
Digit4UI digit4UI;
#define SHIFTED_OUTPUT
#endif

#if SABER_DISPLAY == SABER_DISPLAY_128_32
void BlockDrawOLED(const BlockDrawChunk* chunks, int n)
{
    static const int OLED_BYTES = OLED_WIDTH * OLED_HEIGHT / 8;

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
#endif

void setup() {
    #if defined(SHIFTED_OUTPUT)
        // I'm a little concerned about power draw on the display, in some states.
        // Do this first thing - set all the pins to ground so they don't short / draw.
        pinMode(PIN_LATCH, OUTPUT);
        pinMode(PIN_CLOCK, OUTPUT);
        pinMode(PIN_DATA, OUTPUT);

        digitalWrite(PIN_LATCH, HIGH);
        digitalWrite(PIN_CLOCK, LOW);
        digitalWrite(PIN_DATA, LOW);

        digitalWrite(PIN_LATCH, LOW);
        shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, 0);
        shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, 0);
        digitalWrite(PIN_LATCH, HIGH);
    #endif

    Serial.begin(19200);  // Still need to turn it on in case USB is connected later to configure or debug.
    #if SERIAL_DEBUG == 1
    {
        int nTries = 0;
        while (!Serial) {
            delay(200);
            ++nTries;
            ledA.set((nTries & 1) ? true : false);
        }
        Log.attachSerial(&Serial);
    }
    #endif

    Log.p("Setup:").eol(); 
    Log.p("Init flash memory.").eol();

    flashTransport.begin();
    spiFlash.begin();
    manifest.scan(&spiFlash);

    uint32_t dirHash = manifest.dirHash();
    saberDB.setPaletteFromDirHash(dirHash);

    Log.p("Init audio system.").eol();
    i2sAudioDriver.begin();

    Log.p("Init accelerometer and magnemometer.").eol();
    delay(10);
    accelMag.begin();
    Log.p("Min/Max=").v3(accelMag.getMagMin()).p(" ").v3(accelMag.getMagMax()).eol();
    delay(10);

    Log.p("Init systems.").eol();
    voltmeter.begin();
    delay(10);
    bladePWM.setRGB(RGB::BLACK);

    buttonA.setHoldHandler(buttonAHoldHandler);
    buttonA.setClickHandler(buttonAClickHandler);
    buttonA.setReleaseHandler(buttonAReleaseHandler);

    tester.attach(&buttonA);
    tester.attachDB(&saberDB, &bladePWM, &bladeFlash);

    Log.p("Average power: ").p(voltmeter.averagePower()).eol();
    bladePWM.setVoltage(voltmeter.averagePower());

    Log.p("Init display.").eol();

    #if SABER_DISPLAY == SABER_DISPLAY_128_32
        display.begin(OLED_WIDTH, OLED_HEIGHT, SSD1306_SWITCHCAPVCC);
        renderer.Attach(BlockDrawOLED);
        renderer.SetSize(OLED_WIDTH, OLED_HEIGHT);
        renderer.SetClip(VRender::Rect(0, 0, OLED_WIDTH, OLED_HEIGHT));
        renderer.Clear();
        display.display(oledBuffer);

        Log.p("OLED display connected.").eol();
    #elif SABER_DISPLAY == SABER_DISPLAY_7_5
        // No pin 6
        // Pin 7 - decimal point - not used
        const uint8_t cols[] = { 5, 1, 9, 15, 14};
        dotMatrix.attachCol(5, cols);
        const uint8_t rows[] = { 2, 13, 3, 4, 12, 11, 10};
        dotMatrix.attachRow(7, rows);

        Log.p("Shifted dot matrix 5x7 init.").eol();
    #elif SABER_DISPLAY == SABER_DISPLAY_SEGMENT
        const uint8_t digit[] = {10, 4, 13, 15};
        shifted7.attachDigit(4, digit);
        const uint8_t segment[] = {6, 5, 12, 2, 11, 3, 1, 14};
        shifted7.attachSegment(8, segment);

        Log.p("Shifted seven digit init.").eol();
    #endif

    #if defined(SABER_NUM_LEDS)
        for(int i=0; i<SABER_NUM_LEDS; ++i) {
            leds[i].set(2, 2, 2, 255);
        }
        #if SABER_UI_LED == SABER_LED_DOTSTAR
            dotstar.beginSPI(PIN_DOTSTAR_EN);
            dotstar.display(leds, SABER_NUM_LEDS);
            dotstar.display(leds, SABER_NUM_LEDS);
            Log.p("Dotstar initialized.").eol();
        #else
            neoPixels.begin();
            neoPixels.setBrightness(SABER_UI_BRIGHTNESS);
            Log.p("Neopixel initialized.").eol();
        #endif
    #endif

    syncToDB();
    ledA.set(true); // "power on" light

    buttonA.setHoldRepeats(true);  // everything repeats!!

    // EventQ.event("[saber start]");
    lastLoopTime = millis();    // so we don't get a big jump on the first loop()

    Log.p("Setup() done.").eol();
}

/*
   The saberDB is the source of truth. (The Model.)
   Bring other things in sync when it changes.
*/
void syncToDB()
{
    sfx.setFont(saberDB.soundFont());
    sfx.setVolume(saberDB.volume());

    uiRenderData.volume = saberDB.volume4();
    uiRenderData.color = BladePWM::convertRawToPerceived(saberDB.bladeColor());
    uiRenderData.palette = saberDB.paletteIndex();
    uiRenderData.mVolts = voltmeter.averagePower();
    uiRenderData.fontName = manifest.getUnit(sfx.currentFont()).getName();

    bladeFlash.setBladeColor(saberDB.bladeColor());
    bladeFlash.setImpactColor(saberDB.impactColor());
}

void buttonAReleaseHandler(const Button& b)
{
    ledA.blink(0, 0);
    ledA.set(true); // power is on.

    if (uiMode.mode() == UIMode::COLOR_WHEEL) {
        processColorWheel(true);
        bladePWM.setRGB(osbr::RGB(0));
    }
}

bool setVolumeFromHoldCount(int count)
{
    saberDB.setVolume4(count - 1);
    syncToDB();
    #ifdef SABER_AUDIO_UI
    SFX::instance()->playUISound(saberDB.volume4());
    #endif
    return count >= 0 && count <= 5;
}

void igniteBlade()
{
    if (bladeState.state() == BLADE_OFF) {
        bladeState.change(BLADE_IGNITE);
        sfx.playSound(SFX_POWER_ON, SFX_OVERRIDE);
    }
}

void retractBlade()
{
    if (bladeState.state() != BLADE_OFF && bladeState.state() != BLADE_RETRACT) {
        bladeState.change(BLADE_RETRACT);
        sfx.playSound(SFX_POWER_OFF, SFX_OVERRIDE);
    }
}

bool setPaletteFromHoldCount(int count)
{
    saberDB.setPalette(count - 1);
    syncToDB();
    #ifdef SABER_AUDIO_UI
    SFX::instance()->playUISound(saberDB.paletteIndex());
    #endif
    return count <= SaberDB::NUM_PALETTES;
}

// One button case.
void buttonAClickHandler(const Button&)
{
    uiMode.setActive();

    Log.p("buttonAClickHandler").eol();
    if (bladeState.bladeOff()) {
        uiMode.nextMode();
        // Turn off blinking so we aren't in a weird state when we change modes.
        ledA.blink(0, 0, 0);
        // Not the best indication: show power if
        // the modes are cycled. But haven't yet
        // figured out a better option.
        if (uiMode.mode() == UIMode::NORMAL) {
            int power = Voltmeter::vbatToPowerLevel(voltmeter.averagePower(), 4);
            ledA.blink(power, INDICATOR_CYCLE, 0, LEDManager::BLINK_TRAILING);
        }
    }
    else if (bladeState.state() == BLADE_ON) {
        bladeFlash.doFlash(millis());
        sfx.playSound(SFX_USER_TAP, SFX_GREATER_OR_EQUAL);
    }
}

void buttonAHoldHandler(const Button& button)
{
    uiMode.setActive();
    //Log.p("buttonAHoldHandler nHolds=").p(button.nHolds()).eol();
    
    if (bladeState.state() == BLADE_OFF) {
        bool buttonLEDOn = false;
        int cycle = button.cycle(&buttonLEDOn);
        //Log.p("button nHolds=").p(button.nHolds()).p(" cycle=").p(cycle).p(" on=").p(buttonOn).eol();

        if (uiMode.mode() == UIMode::NORMAL) {
            if (button.nHolds() == 1) {
                igniteBlade();
            }
        }
        else 
        {
            if (buttonLEDOn) {
                if (uiMode.mode() == UIMode::PALETTE) {
                    if (!setPaletteFromHoldCount(cycle))
                        buttonLEDOn = false;
                }
                else if (uiMode.mode() == UIMode::VOLUME) {
                    if (!setVolumeFromHoldCount(cycle))
                        buttonLEDOn = false;
                }
            }
        }
        ledA.set(buttonLEDOn);
    }
    else if (bladeState.state() != BLADE_RETRACT) {
        if (button.nHolds() == 1) {
            retractBlade();
        }
    }
}

void processColorWheel(bool commitChange)
{
    if (uiMode.mode() == UIMode::COLOR_WHEEL) {
        Vec3<int32_t> ave = averageAccel.average();
        FixedNorm x(ave.x, GrinlizLSM303::DIV);
        FixedNorm z(ave.z, GrinlizLSM303::DIV);
        osbr::RGB rgb = AccelToColor(x, z);
        osbr::RGB rgbInv = ColorInverse(rgb);

        bladeFlash.setBladeColor(rgb);
        bladeFlash.setImpactColor(rgbInv);
        if (commitChange) {
            saberDB.setBladeColor(rgb);
            saberDB.setImpactColor(rgbInv);
            syncToDB();
        }
    }
}

void processSerial() {
    while (Serial.available()) {
        int c = Serial.read();
        if (cmdParser.push(c)) {
            syncToDB();
        }
    }
}

void processAccel(uint32_t msec)
{
    static ProfileData profData("processAccel");
    ProfileBlock block(&profData);

    // Consistently process the accelerometer queue, even if we don't use the values.
    // Also look for stalls and hitches.
    static const int N_ACCEL = 4;
    Vec3<float> data[N_ACCEL];
    Vec3<int32_t> intData[N_ACCEL];

    #if SERIAL_DEBUG == 1
    uint32_t start = millis();
    #endif

    int available = accelMag.available();
    if (available > N_ACCEL) {
        accelMag.flush(available - N_ACCEL);
        Log.p("Accelerometer samples disposed=").p(available - N_ACCEL).eol();
    }

    int n = accelMag.readInner(intData, data, N_ACCEL);
    ASSERT(n <= N_ACCEL);

    for(int i=0; i<n; ++i) {
        averageAccel.push(intData[i]);
    }

    if (bladeState.state() == BLADE_ON) {
        for (int i = 0; i < n; ++i)
        {
            float g2Normal, g2;
            float ax = data[i].x;
            float ay = data[i].y;
            float az = data[i].z;
            calcGravity2(ax, ay, az, &g2, &g2Normal);

            float motion = saberDB.motion();
            float impact = saberDB.impact();

            // The extra check here for the motion time is because some
            // motion loops are tacked on to the beginning of the idle
            // loop...which makes sense. (Sortof). But leads to super-long
            // motion. So if time is above a threshold, allow replay.
            // Actual motion / impact sounds are usually less that 1 second.
#ifdef SABER_SOUND_ON
            bool sfxOverDue = ((msec - lastMotionTime) > 1500) &&
                              ((sfx.lastSFX() == SFX_MOTION) || (sfx.lastSFX() == SFX_IMPACT));
#else
            bool sfxOverDue = false;
#endif

            if ((g2Normal >= impact * impact))
            {
                bladeFlash.doFlash(millis());
                if ((msec - lastImpactTime) > IMPACT_MIN_TIME) {
                    bool sound = sfx.playSound(SFX_IMPACT, sfxOverDue ? SFX_OVERRIDE : SFX_GREATER_OR_EQUAL);
                    if (sound)
                    {
                        Log.p("Impact. g=").p(sqrt(g2)).eol();
                        lastImpactTime = msec;
                        lastMotionTime = msec;
                    }
                }
            }
            else if (g2 >= motion * motion)
            {
                bool sound = sfx.playSound(SFX_MOTION, sfxOverDue ? SFX_OVERRIDE : SFX_GREATER);
                if (sound)
                {
                    Log.p("Motion. g=").p(sqrt(g2)).eol();
                    lastMotionTime = msec;
                }
            }
        }
    }
}

void loop() {
    static ProfileData data("loop");
    ProfileBlock block(&data);

    // processSerial() doesn't seem to get called on M0 / ItsyBitsy. 
    // Not sure why.
    // processSerial() is intended to be "out of loop". Call it
    // first in the loop to try to preserve current behavior.
    processSerial();

    const uint32_t msec = millis();
    uint32_t delta = msec - lastLoopTime;
    lastLoopTime = msec;

    tester.process();

    buttonA.process();
    ledA.process();

    if (buttonA.held()) {
        processColorWheel(false);   
    }
    processAccel(msec);

    bladeFlash.tick(msec);
    bladeState.process(&bladePWM, bladeFlash, millis());
    sfx.process(bladeState.bladeOn());

    if (vbatTimer.tick(delta)) {
        voltmeter.takeSample();
        bladePWM.setVoltage(voltmeter.averagePower());
        uiRenderData.mVolts = voltmeter.averagePower();
    }
    
    #ifdef PROFILE
    if (vbatPrintTimer.tick(delta)) {
        Log.p(" ave power=").p(voltmeter.averagePower()).eol();
        DumpProfile();
    }
    #endif    

    loopDisplays(msec, delta);
}
 

void loopDisplays(uint32_t msec, uint32_t delta)
{
    static ProfileData data("loopDisplays");
    ProfileBlock block(&data);
    // General display state processing. Draw to the current supported display.

    #if SABER_DISPLAY == SABER_DISPLAY_128_32
    switch(renderStage) {
        case 0: 
            VectorUI::Draw(&renderer, msec, uiMode.mode(), !bladeState.bladeOff(), &uiRenderData);
            break;
        case 1:
            renderer.Render();
            break;
        case 2:
            display.display(oledBuffer);
            break;
    }
    renderStage++;
    if (renderStage == 3) renderStage = 0;
    #endif

    if (displayTimer.tick(delta)) {
        uiRenderData.color = BladePWM::convertRawToPerceived(saberDB.bladeColor());

        #if SABER_DISPLAY == SABER_DISPLAY_7_5
            display75.Draw(msec, uiMode.mode(), !bladeState.bladeOff(), &uiRenderData);
            dotMatrix.setFrom7_5(display75.Pixels());
        #elif SABER_DISPLAY == SABER_DISPLAY_SEGMENT
            digit4UI.Draw(uiMode.mode(), &uiRenderData);
            shifted7.set(digit4UI.Output().c_str());
        #endif
        #ifdef SABER_UI_START
            osbr::RGB rgb[SABER_UI_COUNT];

            #ifdef SABER_UI_IDLE_MEDITATION
                if (uiMode.mode() == UIMode::NORMAL && bladeState.state() == BLADE_OFF && uiMode.isIdle()) {
                    dotstarUI.Draw(rgb, SABER_UI_COUNT, msec, UIMode::MEDITATION, !bladeState.bladeOff(), uiRenderData);
                    rgb[SABER_UI_START + SABER_UI_COUNT - 1] = uiRenderData.color;
                }
                else {
                    dotstarUI.Draw(rgb, SABER_UI_COUNT, msec, uiMode.mode(), !bladeState.bladeOff(), uiRenderData);
                }
            #else
                dotstarUI.Draw(rgb, SABER_UI_COUNT, msec, uiMode.mode(), !bladeState.bladeOff(), uiRenderData);
            #endif

            // Copy back from Draw(RGB) to Dotstar(RGBA)
            for(int i=0; i<SABER_UI_COUNT; ++i) {
                #ifdef SABER_UI_REVERSE
                leds[SABER_UI_START + SABER_UI_COUNT - 1 - i].set(rgb[i], SABER_UI_BRIGHTNESS);
                #else
                leds[SABER_UI_START + i].set(rgb[i], SABER_UI_BRIGHTNESS);
                #endif
            }
        #endif
    }

    // Now loop() the specific display.
    #if SABER_DISPLAY == SABER_DISPLAY_7_5
        {
            uint8_t a=0, b=0;
            dotMatrix.loop(micros(), &a, &b);

            // For columns / anodes.
            //uint16_t bits = 0;  // all rows
            //bits |= (1<<5); // column 1
            //bits |= (1<<1); // column 2qq
            //bits |= (1<<9); // column 3 - note the bit skip!! pin 8
            //bits |= (1<<15); // column 4
            //bits |= (1<<14); // column 5

            /*
            // For rows / cathodes.
            uint16_t bits = ~((1<<1) | (1<<9) | (1<<15) | (1<<14));
            //bits ^= (1<<2);
            //bits ^= (1<<13);
            //bits ^= (1<<3);
            //bits ^= (1<<4);
            //bits ^= (1<<12);
            //bits ^= (1<<11);
            bits ^= (1<<10);
            */
            
            //a = bits & 0xff;
            //b = bits >> 8;  
           
            digitalWrite(PIN_LATCH, LOW);
            shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, a);
            shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, b);
            digitalWrite(PIN_LATCH, HIGH);

            // digitalWrite(PIN_LATCH, LOW);
            // digitalWrite(PIN_CLOCK, LOW);
            // digitalWrite(PIN_DATA, HIGH);
        }
    #elif SABER_DISPLAY == SABER_DISPLAY_SEGMENT
        {
            uint8_t a=0, b=0;
            shifted7.loop(micros(), &a, &b);

            digitalWrite(PIN_LATCH, LOW);
            shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, b);
            shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, a);
            digitalWrite(PIN_LATCH, HIGH);
        }
    #endif

    #if defined(SABER_CRYSTAL_START)
    {
        const RGB bladeColor = saberDB.bladeColor();
        if (bladeState.state() == BLADE_OFF && (uiMode.mode() == UIMode::NORMAL)) {
            osbr::RGB rgb;
            calcCrystalColorHSV(msec, bladeColor, &rgb);
            leds[SABER_CRYSTAL_START].set(rgb, SABER_CRYSTAL_BRIGHTNESS);
        }
        else {
            leds[SABER_CRYSTAL_START].set(bladeColor, SABER_CRYSTAL_BRIGHTNESS);
        }
    }
    #endif

    #if defined(SABER_FLASH_LED)
        // Flashes a secondary LED with the flash on clash color.
        RGB flashColor = saberDB.impactColor();
        leds[SABER_FLASH_LED] = ((bladeState.state() == BLADE_FLASH) ? flashColor : RGBA::BLACK, 255);
    #endif

    #if defined(SABER_BLACK_START)
        for(int i=SABER_BLACK_START; i<SABER_BLACK_START + SABER_BLACK_COUNT; ++i) {
            leds[i].set(0);
        }
    #endif

    #ifdef SABER_NUM_LEDS
        dotstar.display(leds, SABER_NUM_LEDS);
    #endif    
}
