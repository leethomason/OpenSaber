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
//#include <Adafruit_ZeroDMA.h>

#include "./src/nada_flashmem/Nada_SPIFLash.h"
#include "./src/nada_flashmem/Nada_FlashTransport_SPI.h"
#include "./src/util/Grinliz_Arduino_Util.h"
#include "./src/lsm303/grinliz_LSM303.h"
#include "./src/lis3dh/grinliz_lis3dh.h"
#include "./src/lsm6d/grinliz_lsm6d.h"

#include <Wire.h>
#include <SPI.h>

#include "OLED_SSD1306.h"
#include "Button.h"

// Includes
// -- Must be first. Has configuration. -- //
#include "pins.h"

#include "DotStar.h"
#include "voltmeter.h"
#include "sfx.h"
#include "saberdb.h"
#include "cmdparser.h"
#include "bladePWM.h"
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
#include "swing.h"

using namespace osbr;

static const uint32_t INDICATOR_CYCLE         = 1000;
static const uint32_t IMPACT_MIN_TIME         =  600;   // was 1000

uint32_t lastImpactTime = 0;
uint32_t lastLoopTime   = 0;
uint32_t lastDisplayLoopTime = 0;
bool colorWasProcessed = false;

Nada_FlashTransport_SPI flashTransport(SS1, &SPI1);
Nada_SPIFlash spiFlash(&flashTransport);
Adafruit_ZeroDMA dma;

Manifest manifest;
Adafruit_ZeroI2S i2s(PIN_I2S_LRCLK, PIN_I2S_BITCLK, PIN_I2S_DATA, 2);
I2SAudioDriver i2sAudioDriver(&dma, &i2s, &spiFlash, manifest);
SFX sfx(&i2sAudioDriver, manifest);

ButtonCB    buttonA(PIN_SWITCH_A, Button::INTERNAL_PULLUP);
LEDManager  ledA(PIN_LED_A, false);
AnimateProp ledProp;

BladeState  bladeState;
UIModeUtil  uiMode;
SaberDB     saberDB;
Voltmeter   voltmeter(1650, 10000, 47000, 4095, VOLTMETER_TUNE);
BladeFlash  bladeFlash;
CMDParser   cmdParser(&saberDB, manifest);
BladePWM    bladePWM;
Tester      tester;
bool        lockOn = false;
LockTimer   lockTimer;
int         swingVol = 0;

#if SABER_ACCELEROMETER == SABER_ACCELEROMETER_LSM303
Swing       swing;
MagFilter   magFilter;
#endif

#ifdef SABER_NUM_LEDS
DotStar dotstar;                    // Hardware controller.
#endif
#ifdef SABER_UI_START
DotStarUI dotstarUI;                // It is the DotStarUI regardless of physical pixel protocol
#endif

ACCELEROMETER accelMag(PIN_ACCEL_EN);

#if SABER_DISPLAY == SABER_DISPLAY_128_32
static const int DISPLAY_TIMER = 23;
#else
static const int DISPLAY_TIMER = 67;
#endif

Timer2 displayTimer(DISPLAY_TIMER);       // distribute time to minimize "slow ticks"
Timer2 vbatPrintTimer(1217);
Timer2 audioPrintTimer(2000);

#if SABER_DISPLAY == SABER_DISPLAY_128_32
static const int OLED_WIDTH = 128;
static const int OLED_WIDTH_SHIFT = 7;
static const int OLED_HEIGHT = 32;
uint8_t oledBuffer[OLED_WIDTH * OLED_HEIGHT / 8] = {0};

OLED_SSD1306 display(PIN_OLED_DC, PIN_OLED_RESET, PIN_OLED_CS);
VRender    vRender;
Renderer   renderer;
VectorUI   vectorUI;

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
    // The renderer assumes clear to black beforehand.
    // But does still support drawing in black.
    for (int i = 0; i < n; ++i) {
        const BlockDrawChunk& chunk = chunks[i];

        // Simple for the single row.
        int row = chunk.y >> 3;
        int bit = chunk.y - (row << 3);
        uint8_t mask = 1 << bit;
        uint8_t* p = oledBuffer + (row << OLED_WIDTH_SHIFT) + chunk.x0;

        if (chunk.color) {
            for (int nPix = chunk.x1 - chunk.x0; nPix > 0; nPix--, p++) {
                *p |= mask;
            }
        }
        else {
            for (int nPix = chunk.x1 - chunk.x0; nPix > 0; nPix--, p++) {
                *p &= ~mask;
            }
        }
    }
}
#endif

void setup() 
{
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
    accelMag.begin(&SPI);
    accelMag.flushAccel(0);
    //Log.p("Min/Max=").v3(accelMag.getMagMin()).p(" ").v3(accelMag.getMagMax()).eol();
    delay(10);

    Log.p("Init systems.").eol();
    analogReadResolution(12);
    analogReference(AR_INTERNAL1V65);
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
    {
        display.begin(OLED_WIDTH, OLED_HEIGHT, SSD1306_SWITCHCAPVCC);
        vRender.Attach(BlockDrawOLED);
        vRender.SetSize(OLED_WIDTH, OLED_HEIGHT);
        vRender.SetClip(VRender::Rect(0, 0, OLED_WIDTH, OLED_HEIGHT));
        vRender.Clear();

        renderer.Attach(OLED_WIDTH, OLED_HEIGHT, oledBuffer);
        Log.p("OLED display connected.").eol();
    }
    #elif SABER_DISPLAY == SABER_DISPLAY_7_5
    {
        // No pin 6
        // Pin 7 - decimal point - not used
        const uint8_t cols[] = { 5, 1, 9, 15, 14};
        dotMatrix.attachCol(5, cols);
        const uint8_t rows[] = { 2, 13, 3, 4, 12, 11, 10};
        dotMatrix.attachRow(7, rows);

        Log.p("Shifted dot matrix 5x7 init.").eol();
    }
    #elif SABER_DISPLAY == SABER_DISPLAY_SEGMENT
    {
        const uint8_t digit[] = {10, 4, 13, 15};
        shifted7.attachDigit(4, digit);
        const uint8_t segment[] = {6, 5, 12, 2, 11, 3, 1, 14};
        shifted7.attachSegment(8, segment);

        Log.p("Shifted seven digit init.").eol();
    }
    #endif

    #if defined(SABER_NUM_LEDS)
    {
        osbr::RGBA leds[SABER_NUM_LEDS] = {0x01020304};
        #if SABER_UI_LED == SABER_LED_DOTSTAR
        {
#if ITSY_DOTSTAR_UI
            dotstar.beginSW(40, 41);
#else
            dotstar.beginSPI(PIN_DOTSTAR_EN);
#endif            
            delay(2);
            dotstar.display(leds, SABER_NUM_LEDS);
            delay(2);
            dotstar.display(leds, SABER_NUM_LEDS);
            Log.p("Dotstar initialized.").eol();
        }
        #endif
    }
    #endif

    syncToDB();
    ledA.set(true); // "power on" light

    buttonA.setHoldRepeats(true);  // everything repeats!!
    lastLoopTime = millis();    // so we don't get a big jump on the first loop()
    lastDisplayLoopTime = millis();

    Log.p("Setup() done.").eol();
}

/*
   The saberDB is the source of truth. (The Model.)
   Bring other things in sync when it changes.
   This made...more sense when the EEPROM was supported.
   All the modern sabers just reset on power on.
*/
void syncToDB()
{
    const SaberDB::Palette *palette = saberDB.getPalette();

    sfx.setFont(saberDB.soundFont());
    sfx.setVolume(saberDB.volume());
    for(int i=0; i<SaberDB::Palette::NAUDIO; ++i) {
        sfx.setBoost(palette->channelBoost[i], i);
    }
    bladeFlash.setBladeColor(palette->bladeColor);
    bladeFlash.setImpactColor(palette->impactColor);
}

void buttonAReleaseHandler(const Button&)
{
    ledA.blink(0, 0);
    ledA.set(true); // power is on.
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

bool setVolumeFromHoldCount(int count)
{
    //Log.p("cycle=").p(count).eol();
    saberDB.setVolume4(count - 1);
    syncToDB();
    #ifdef SABER_AUDIO_UI
    SFX::instance()->playUISound(saberDB.volume4());
    #endif
    return count >= 0 && count <= 5;
}

bool setLockFromHoldCount(int count)
{
    lockOn = count > 1;
    return count <= 2;
}

void igniteBlade()
{
    if (bladeState.state() == BLADE_OFF) {
        bladeState.change(BLADE_IGNITE);
        if (sfx.smoothMode())
            sfx.sm_ignite();
        else
            sfx.playSound(SFX_POWER_ON, SFX_OVERRIDE);
    }
}

void retractBlade()
{
    if (bladeState.state() != BLADE_OFF && bladeState.state() != BLADE_RETRACT) {
        bladeState.change(BLADE_RETRACT);
        if (sfx.smoothMode())
            sfx.sm_retract();
        else
            sfx.playSound(SFX_POWER_OFF, SFX_OVERRIDE);
    }
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
            int power = UIRenderData::powerLevel(voltmeter.averagePower(), 4);
            ledA.blink(power, INDICATOR_CYCLE, 0, LEDManager::BLINK_TRAILING);
        }
    }
    else if (bladeState.state() == BLADE_ON) {
        if (lockTimer.enabled()) {
            if (lockTimer.click())
                retractBlade();
        }
        else {
            bladeFlash.doFlash(millis());
            if (sfx.smoothMode())
                sfx.sm_playEvent(SFX_USER_TAP);
            else
                sfx.playSound(SFX_USER_TAP, SFX_GREATER_OR_EQUAL);
        }    
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
#if SABER_LOCK()
                else if (uiMode.mode() == UIMode::LOCK) {
                    if (!setLockFromHoldCount(cycle))
                        buttonLEDOn = false;
                }
#endif
            }
        }
        ledA.set(buttonLEDOn);
    }
    else if (bladeState.state() != BLADE_RETRACT) {
        if (button.nHolds() == 1) {
            if (lockOn)
                lockTimer.start();
            else
                retractBlade();
        }
    }
}


void processSerial() 
{
    while (Serial.available()) {
        int c = Serial.read();
        if (cmdParser.push(c)) {
            syncToDB();
        }
    }
}

void processAccel(uint32_t msec, uint32_t)
{
    static ProfileData profData("processAccel");
    ProfileBlock block(&profData);

    // Consistently process the accelerometer queue, even if we don't use the values.
    // Also look for stalls and hitches.
    accelMag.flushAccel(4);

#if SABER_ACCELEROMETER == SABER_ACCELEROMETER_LSM303
    Vec3<int32_t> magData;
    // readMag() should only return >0 if there is new data from the hardware.
    if (accelMag.hasMag() && accelMag.sampleMag(&magData)) {
        
        static const uint32_t MAG_INTERVAL = 16;
        static uint32_t lastMagData = 0;
        int nMagData = 1;
        if (lastMagData && (msec - lastMagData) >= MAG_INTERVAL * 2) {
            // use a little less than 10ms for error correction
            nMagData = (msec - lastMagData) / MAG_INTERVAL;
        }
        lastMagData = msec;

        // The accelerometer and magnemometer are both clocked at 100Hz.
        // The swing is set up for constant data; assume n is the same for both.
        // Hopefully we keep a constant enough rate this doesn't matter.
        // int nMag = n > 0 ? n : 1;
        // Keep waffling on this...assuming when blade is lit this will pretty
        // consistently get hit every 10ms.

        for (int i = 0; i < nMagData; ++i)
            magFilter.push(magData);

        Vec3<int32_t> magMin = accelMag.getMagMin();
        Vec3<int32_t> magMax = accelMag.getMagMax();
        swing.push(magFilter.average(), magMin, magMax);

        float dot = swing.dotOrigin();
        float speed = swing.speed();
        //vectorUI.PushTestData(speed, 0.0f, SWING_MAX, msec, 4.0f);
        sfx.sm_setSwing(speed, (int)((1.0f + dot)*128.0f));

#if false && (SERIAL_DEBUG == 1)
        static const int BURST = 5;
        static int32_t lastLog = 0;
        static int burstLog = 0;

        if (msec - lastLog > 500)
            burstLog = BURST;
        if (burstLog) {
            Vec3<int32_t> range = magMax - magMin;
            Log.p(burstLog == BURST ? "--" : "  ")
                .p("t=").p(millis()%1000)
                .p(" swing=").p(swing.speed())
                .p(" swingVol=").p(swingVol)
                .p(" range=").v3(range)
                //.p(" pos=").v3(swing.pos()).p(" origin=").v3(swing.origin())
                //.p(" dot=").p(dot)
                //.p(" magfilter val/min/max ").v3(magFilter.average()).v3(magMin).v3(magMax)
                //.p(" val/range ").v3(magFilter.average() - magMin).v3(range)
                //.p(" accel=").p(fastG2.average())
                .eol();
            lastLog = msec;
            burstLog--;
        }
#endif        
    }
#endif

    Vec3<Fixed115> accelData;
    Vec3<int32_t> gyroData;
    while(accelMag.sampleAccel(&accelData, &gyroData))
    {
#if SABER_ACCELEROMETER == SABER_ACCELEROMETER_LSM6D
        {
            static Vec3<Fixed16> bladeRotation(0, 0, 0);    // 0-360
            static Vec3<FixedNorm> bladeOrigin(1, 0, 0);    // normalized, -1 to 1
            static int logDiv = 0;
            const static int LOG_DIV = 20;

            // Some notes: this math is only correct if you move
            // one axis independentantly. But...that's okay.
            // We mostly want rates.
            // Ignore the "twist" axis.

            // 100 hz. (Not delta as first attempted. Oops.)
            // Blade rotation is in dps - can feed that directly to the sound system.
            // BUT! We also need a normal origin position to blend the sounds.
            Vec3<FixedNorm> bladeOrientation;
            const Fixed16 dt(10, 1000);

            for (int i = 0; i < 3; ++i) {
                bladeRotation[i] += dt * (int)gyroData[i];
                while(bladeRotation[i] < 0)
                    bladeRotation[i] += 360;
                while (bladeRotation[i] >= 360)
                    bladeRotation[i] -= 360;

                bladeOrientation[i] = iSin(FixedNorm(bladeRotation[i].scale(8), 360*8));
            }

            if (swingVol == 0) {
                bladeOrigin = bladeOrientation;
            }
            int32_t dps2 = gyroData.x * gyroData.x + gyroData.y * gyroData.y + gyroData.z * gyroData.z;
            int32_t dps = intSqrt(dps2);
            FixedNorm dot = bladeOrigin.dot(bladeOrientation);
            int32_t dotScaled = dot.scale(128); // -128 to 128

            if(++logDiv == LOG_DIV) {
                logDiv = 0;
                Log.p("dps=").p(dps).p(" rot=").v3(bladeRotation).p(" orient=").v3(bladeOrientation).eol();
            };
            sfx.sm_setSwing(dps * 3.14f / 180.0f, dotScaled + 128);
        }
#endif

        Fixed115 g2Normal, g2;
        calcGravity2(accelData.x, accelData.y, accelData.z, &g2, &g2Normal);

        if (bladeState.state() == BLADE_ON)
        {
            static const Fixed115 motion = DEFAULT_G_FORCE_MOTION;
            //static const Fixed115 impact = DEFAULT_G_FORCE_IMPACT;
            static const Fixed115 impact2 = DEFAULT_G_FORCE_IMPACT * DEFAULT_G_FORCE_IMPACT;

            // This workaround no longer makes sense with smoothswing,
            // and the much more compressed storage.
            //
            // The extra check here for the motion time is because some
            // motion loops are tacked on to the beginning of the idle
            // loop...which makes sense. (Sortof). But leads to super-long
            // motion. So if time is above a threshold, allow replay.
            // Actual motion / impact sounds are usually less that 1 second.
            // TODO this needs a different workaround.

            //#ifdef SABER_SOUND_ON
            //            bool sfxOverDue = ((msec - lastMotionTime) > 1500) &&
            //                              ((sfx.lastSFX() == SFX_MOTION) || (sfx.lastSFX() == SFX_IMPACT));
            //#else
            //            bool sfxOverDue = false;
            //#endif

            if ((g2Normal >= impact2))
            {
                // Always flash the blade. Maybe play impact sounds.
                // It doesn't sound good to have too many impacts chained together.
                bladeFlash.doFlash(millis());
                if ((msec - lastImpactTime) > IMPACT_MIN_TIME)
                {
                    bool sound = false;
                    if (sfx.smoothMode())
                        sound = sfx.sm_playEvent(SFX_IMPACT);
                    else
                        sound = sfx.playSound(SFX_IMPACT, SFX_GREATER_OR_EQUAL);

                    if (sound)
                    {
                        Log.p("Impact. g=").p(g2.sqrt()).eol();
                        lastImpactTime = msec;
                    }
                }
            }
            else if (!sfx.smoothMode() && g2 >= motion * motion)
            {
                bool sound = sfx.playSound(SFX_MOTION, SFX_GREATER);
                if (sound) {
                    Log.p("Motion. g=").p(g2.sqrt()).eol();
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
    lockTimer.tick(delta);

#ifdef SABER_UI_IDLE_MEDITATION_LED_OFF
    if (uiMode.mode() == UIMode::NORMAL && bladeState.state() == BLADE_OFF && uiMode.isIdle()) {
        int pwm = 255;
        ledProp.tick(delta, &pwm);
        analogWrite(ledA.pin(), (uint8_t)pwm);
    } 
    else {
        ledProp.start(1000, 255, 0);
        ledA.process();
    }
    #else
    ledA.process();
    #endif

    processAccel(msec, delta);

    bladeFlash.tick(msec);
    bladeState.process(&bladePWM, bladeFlash, millis());
    
    sfx.process(bladeState.state(), delta, &swingVol);
    if (swingVol == 0) {
        #if SABER_ACCELEROMETER == SABER_ACCELEROMETER_LSM303
        accelMag.recalibrateMag();
        swing.setOrigin();
        #endif
    }

    // Timing / skip is in the voltmeter class.
    voltmeter.takeSample();
    // setVoltage() will do nothing if the averagePower() is the same
    bladePWM.setVoltage(voltmeter.averagePower());
    
    #ifdef PROFILE
    if (vbatPrintTimer.tick(delta)) {
        Log.p(" ave power=").p(voltmeter.averagePower()).eol();
        DumpProfile();
    }
    #endif    
    loopDisplays(msec, delta);
}

void loopDisplays(uint32_t msec, uint32_t mainDelta)
{
    int dTick = displayTimer.tick(mainDelta);
    if (dTick == 0)
        return;

    static ProfileData data("loopDisplays");
    ProfileBlock block(&data);

    // This was an ugly bug that's been here...forever.
    // The delta for the displays is different from 
    // the "main" delta.
    uint32_t delta = msec - lastDisplayLoopTime;
    lastDisplayLoopTime = msec;

#ifdef SABER_NUM_LEDS
    osbr::RGBA leds[SABER_NUM_LEDS] = {0};
#endif

    UIRenderData uiRenderData;
    uiRenderData.volume = saberDB.volume4();
    uiRenderData.color = BladePWM::convertRawToPerceived(saberDB.bladeColor());
    uiRenderData.palette = saberDB.paletteIndex();
    uiRenderData.mVolts = voltmeter.easedPower();
    uiRenderData.fontName = manifest.getUnit(sfx.currentFont()).getName();
    uiRenderData.soundBank = sfx.currentFont();
    uiRenderData.locked = lockOn;
    uiRenderData.lockFlash = lockTimer.dark();

#if SABER_DISPLAY == SABER_DISPLAY_128_32
    {
        switch (renderStage)
        {
        case 0:
            memset(oledBuffer, 0, OLED_HEIGHT * OLED_WIDTH / 8);
            vRender.Clear();
            vRender.ClearTransform();
            break;
        case 1:
            vectorUI.Draw(&vRender, &renderer,
                msec, delta, uiMode.mode(), 
                !bladeState.bladeOff(), &uiRenderData);
            break;
        case 2:
            display.display(oledBuffer);
            break;
        }
        renderStage++;
        if (renderStage == 3)
            renderStage = 0;
    }
#endif

#if SABER_DISPLAY == SABER_DISPLAY_7_5
    {
        display75.Draw(msec, uiMode.mode(), !bladeState.bladeOff(), &uiRenderData);
        dotMatrix.setFrom7_5(display75.Pixels());
    }
#elif SABER_DISPLAY == SABER_DISPLAY_SEGMENT
    {
        digit4UI.Draw(uiMode.mode(), &uiRenderData);
        shifted7.set(digit4UI.Output().c_str());
    }
#endif
#ifdef SABER_UI_START
    {
        osbr::RGB rgb[SABER_UI_COUNT];

#ifdef SABER_UI_IDLE_MEDITATION
        if (uiMode.mode() == UIMode::NORMAL && bladeState.state() == BLADE_OFF && uiMode.isIdle())
        {
            dotstarUI.Draw(rgb, SABER_UI_COUNT, msec, delta, UIMode::MEDITATION, !bladeState.bladeOff(), uiRenderData);
            rgb[SABER_UI_START + SABER_UI_COUNT - 1] = uiRenderData.color;
        }
        else
        {
            dotstarUI.Draw(rgb, SABER_UI_COUNT, msec, delta, uiMode.mode(), !bladeState.bladeOff(), uiRenderData);
        }
#else
        dotstarUI.Draw(rgb, SABER_UI_COUNT, msec, delta, uiMode.mode(), !bladeState.bladeOff(), uiRenderData);
#endif

        // Copy back from Draw(RGB) to Dotstar(RGBA)
        for (int i = 0; i < SABER_UI_COUNT; ++i)
        {
#ifdef SABER_UI_REVERSE
            leds[SABER_UI_START + SABER_UI_COUNT - 1 - i].set(rgb[i], SABER_UI_BRIGHTNESS);
#else
            leds[SABER_UI_START + i].set(rgb[i], SABER_UI_BRIGHTNESS);
#endif
        }
    }
#endif  // SABER_UI_START

// Now loop() the specific display.
#if SABER_DISPLAY == SABER_DISPLAY_7_5
    {
        uint8_t a = 0, b = 0;
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
        uint8_t a = 0, b = 0;
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
        if (bladeState.state() == BLADE_OFF && (uiMode.mode() == UIMode::NORMAL))
        {
            osbr::RGB rgb;
            calcCrystalColorHSV(msec, bladeColor, &rgb);
            leds[SABER_CRYSTAL_START].set(rgb, SABER_CRYSTAL_BRIGHTNESS);
        }
        else
        {
            leds[SABER_CRYSTAL_START].set(bladeColor, SABER_CRYSTAL_BRIGHTNESS);
        }
    }
#endif

#if defined(SABER_FLASH_LED)
    {
        // Flashes a secondary LED with the flash on clash color.
        RGB flashColor = saberDB.impactColor();
        leds[SABER_FLASH_LED] = ((bladeState.state() == BLADE_FLASH) ? flashColor : RGBA::BLACK, 255);
    }
#endif

#if defined(SABER_BLACK_START)
    {
        for (int i = SABER_BLACK_START; i < SABER_BLACK_START + SABER_BLACK_COUNT; ++i)
        {
            leds[i].set(0);
        }
    }
#endif

#ifdef SABER_NUM_LEDS
    {
        dotstar.display(leds, SABER_NUM_LEDS);
    }
#endif
}
