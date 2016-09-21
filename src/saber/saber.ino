/*
  Copyright (c) 2016 Lee Thomason, Grinning Lizard Software

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

// Arduino Libraries
#include <EEPROM.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SPIFlash.h>
#include <OLED_SSD1306.h>

#include <Audio.h>
#include <Button.h>
#include <Adafruit_LIS3DH.h>
#include <NXPMotionSense.h>
#include <Grinliz_Arduino_Util.h>
#include <DotStar.h>

// Includes
// -- Must be first. Has configuration. -- //
#include "pins.h"

#ifdef SABER_SOUND_ON
#include "sfx.h"
#include "AudioPlayer.h"
#endif
#include "saberdb.h"
#include "cmdparser.h"
#include "blade.h"
#include "sketcher.h"
#include "renderer.h"
#include "saberUtil.h"
#include "tester.h"

static const uint8_t  BLADE_BLACK[NCHANNELS]  = {0};
static const uint32_t FLASH_TIME              = 120;
static const uint32_t VBAT_TIME_INTERVAL      = 500;

static const uint32_t INDICATOR_TIME          = 500;
static const uint32_t INDICATOR_CYCLE         = INDICATOR_TIME * 2;
static const float    GFORCE_RANGE            = 4.0f;
static const float    GFORCE_RANGE_INV        = 1.0f / GFORCE_RANGE;

bool     paletteChange  = false; // used to prevent sound fx on palette changes
uint32_t reflashTime    = 0;
bool     flashOnClash   = false;
float    maxGForce2     = 0.0f;

#ifdef SABER_SOUND_ON
// First things first: disable that audio to avoid clicking.
AudioPlayer audioPlayer;
SFX sfx(&audioPlayer);
#endif

BladeState  bladeState;
ButtonCB    buttonA(PIN_SWITCH_A, Button::PULL_UP);
LEDManager  ledA(PIN_LED_A, false);
LEDManager  ledB(PIN_LED_B, false);  // Still have LEDB support, even if 1 button.

UIRenderData uiRenderData;

#ifdef SABER_TWO_BUTTON
ButtonCB    buttonB(PIN_SWITCH_B, Button::PULL_UP);
#else
ButtonMode  buttonMode;
#endif
SaberDB     saberDB;
AveragePower averagePower;

#ifdef SABER_NUM_LEDS
DotStar::RGB leds[SABER_NUM_LEDS];
DotStar dotstar(PIN_DOTSTAR_EN);
DotStarUI dotstarUI;
#endif

Accelerometer accel;

Timer displayTimer(100);
#ifdef SABER_DISPLAY
static const int OLED_WIDTH = 128;
static const int OLED_HEIGHT = 32;
uint8_t oledBuffer[OLED_WIDTH * OLED_HEIGHT / 8] = {0};

OLED_SSD1306 display(PIN_OLED_DC, PIN_OLED_RESET, PIN_OLED_CS);
Sketcher sketcher;
Renderer renderer;
#endif

CMDParser cmdParser(&saberDB);
Blade blade;
Timer vbatTimer(VBAT_TIME_INTERVAL);
Timer gforceDataTimer(110);
File logFile;

Tester tester;
uint32_t unlocked = 0;

void setupSD(int logCount)
{
    SPI.setMOSI(PIN_SABER_MOSI);
    SPI.setSCK(PIN_SABER_CLOCK);
    #ifdef SABER_SOUND_ON
        if (!(SD.begin(PIN_SDCARD_CS))) {
            // stop here, but print a message repetitively
            while (1) {
                Serial.println("Unable to access the SD card");
                delay(500);
            }
        }
        SD.mkdir("logs");
        char path[] = "logs/log00.txt";
        path[8] = ((logCount / 10) % 10) + '0';
        path[9] = (logCount % 10) + '0';
        logFile = SD.open(path, FILE_WRITE);
        logFile.print("Log open. Instance=");
        logFile.println(logCount);
    #endif
}

void setup() {
    Serial.begin(19200);  // still need to turn it on in case a command line is connected.
    #if SERIAL_DEBUG == 1
        while (!Serial) {
            delay(100);
        }
    #endif
    //TCNT1 = 0x7FFF; // set blue & green channels out of phase
    // Database is the "source of truth".
    // Set it up first.
    saberDB.readData();

    #if SERIAL_DEBUG == 1
        Log.attachSerial(&Serial);
    #endif
    #ifdef SABER_SOUND_ON
        setupSD(saberDB.numSetupCalls());
        // Log.attachLog(&logFile);
    #endif

    Log.p("setup()").eol();

    accel.begin();

    #ifdef SABER_VOLTMETER
        analogReference(INTERNAL);  // 1.1 volts
        analogRead(PIN_VMETER);     // warm up the ADC to avoid spurious initial value.
        Log.p("Voltmeter open.").eol();
        delay(50);
        for(int i=0; i<AveragePower::NUM_SAMPLES; ++i) {
            delay(10);
            averagePower.push(readVbat());
        }
        Log.p("Voltmeter initialized.").eol();
    #endif

    blade.setRGB(BLADE_BLACK);

    buttonA.holdHandler(buttonAHoldHandler);
    buttonA.clickHandler(buttonAClickHandler);
    buttonA.releaseHandler(buttonAReleaseHandler);

    #ifdef SABER_TWO_BUTTON
        buttonB.clickHandler(buttonBClickHandler);
        buttonB.releaseHandler(buttonBReleaseHandler);
        buttonB.holdHandler(buttonBHoldHandler);
        buttonB.pressHandler(buttonBPressHandler);
    #endif

    #ifdef SABER_TWO_BUTTON
        tester.attach(&buttonA, &buttonB);
    #else
        tester.attach(&buttonA, 0);
    #endif

    #ifdef SABER_SOUND_ON
        sfx.init();
        Log.p("sfx initialized.").eol();
    #endif

    blade.setVoltage(averagePower.power());

    #ifdef SABER_DISPLAY
        display.begin(OLED_WIDTH, OLED_HEIGHT, SSD1306_SWITCHCAPVCC);
        renderer.Attach(OLED_WIDTH, OLED_HEIGHT, oledBuffer);
        renderer.Fill(0);
        display.display(oledBuffer);

        Log.p("OLED display connected.").eol();
    #endif

    #if defined(SABER_NUM_LEDS)
        #if defined(SABER_LEDS)
            dotstar.setBrightness(4);
        #endif
        dotstar.attachLEDs(leds, SABER_NUM_LEDS);
        for(int i=0; i<SABER_NUM_LEDS; ++i) {
            leds[i].set(0x010101);
        }
        dotstar.display();
        dotstar.display();
        Log.p("Crystal Dotstart initialized.").eol();
    #endif

    syncToDB();
    ledA.set(true); // "power on" light
    Log.event("[saber start]");
}

uint32_t calcReflashTime() {
    return millis() + random(500) + 200;
}

int vbatToPowerLevel(int32_t vbat)
{
    int level = 0;
    if      (vbat > 3950) level = 4;
    else if (vbat > 3800) level = 3;
    else if (vbat > 3650) level = 2;
    else if (vbat > LOW_VOLTAGE) level = 1;
    return level;
}

/*
   The saberDB is the source of true. (The Model.)
   Bring other things in sync when it changes.
*/
void syncToDB()
{
    #ifdef SABER_SOUND_ON
        sfx.setFont(saberDB.soundFont());
        sfx.mute(!saberDB.soundOn());
        sfx.setVolume204(saberDB.volume());
    #endif

    uiRenderData.volume = saberDB.volume4();
    for (int i = 0; i < 3; ++i) {
        uiRenderData.color[i] = saberDB.bladeColor()[i];
    }
    uiRenderData.palette = saberDB.paletteIndex();
    uiRenderData.power = vbatToPowerLevel(averagePower.power());
    #ifdef SABER_SOUND_ON
        uiRenderData.fontName = sfx.currentFontName();
    #endif

    if (!ledB.blinking()) {   // If blinking, then the LED is being used as UI.
        ledB.set(saberDB.soundOn());
    }
}

void buttonAReleaseHandler(const Button& b)
{
    ledA.blink(0, 0);
    #ifdef SABER_LOCK
        if (bladeState.bladeOn()) {
            uint32_t holdTime = millis() - b.pressedTime();
            Log.p("holdTime: ").p(holdTime).eol();
            if (holdTime >= b.holdThreshold()) {
                unlocked = millis();
                Log.event("[unlocked]");
            }
        }
    #endif
}

#ifdef SABER_TWO_BUTTON
void blinkVolumeHandler(const LEDManager& manager)
{
    saberDB.setVolume4(manager.numBlinks());
    syncToDB();
}

void buttonAClickHandler(const Button&)
{
    Log.p("buttonAClickHandler...");
    // Special case: color switch.
    if (bladeState.state() == BLADE_ON && buttonB.isDown()) {
        Log.p("palette change.").eol();
        saberDB.nextPalette();
        paletteChange = true;
        syncToDB();
    }
    else {
        #ifdef SABER_LOCK
            if (millis() - unlocked <= Button::DEFAULT_HOLD_TIME) {
                bladeState.change(BLADE_RETRACT);
                #ifdef SABER_SOUND_ON
                    sfx.playSound(SFX_POWER_OFF, SFX_OVERRIDE);
                #endif
            }
            else 
        #endif
            {
                int32_t vbat = averagePower.power();
                Log.event("[Vbat]", vbat);
                ledA.blink(vbatToPowerLevel(vbat), INDICATOR_CYCLE, 0, LEDManager::BLINK_TRAILING);
            }
    }
}


void buttonAHoldHandler(const Button&) {
    Log.p("buttonAHoldHandler").eol();
    if (bladeState.state() == BLADE_OFF) {
        bladeState.change(BLADE_IGNITE);
        #ifdef SABER_SOUND_ON
            sfx.playSound(SFX_POWER_ON, SFX_OVERRIDE);
        #endif
    }
    else if (bladeState.state() != BLADE_RETRACT) {
        #ifdef SABER_LOCK
            // see buttonAReleaseHandler
        #else
            bladeState.change(BLADE_RETRACT);
            #ifdef SABER_SOUND_ON
                sfx.playSound(SFX_POWER_OFF, SFX_OVERRIDE);
            #endif
        #endif
    }
}

void buttonBPressHandler(const Button&) {
    paletteChange = false;
}

void buttonBHoldHandler(const Button&) {
    Log.p("buttonBHoldHandler").eol();
    if (bladeState.state() != BLADE_OFF) {
        if (!paletteChange) {
            bladeState.change(BLADE_FLASH);
            #ifdef SABER_SOUND_ON
                sfx.playSound(SFX_USER_HOLD, SFX_OVERRIDE);
            #endif
            flashOnClash = true;
            reflashTime = calcReflashTime();
        }
    }
    else if (bladeState.state() == BLADE_OFF) {
        if (saberDB.soundOn()) {
            saberDB.setSoundOn(false);
            syncToDB();
        }
        else {
            ledB.blink(4, INDICATOR_CYCLE, blinkVolumeHandler);
        }
    }
}

void buttonBReleaseHandler(const Button& b) {
    if (flashOnClash && bladeState.state() != BLADE_OFF) {
        #ifdef SABER_SOUND_ON
            sfx.playSound(SFX_IDLE, SFX_OVERRIDE);
        #endif
    }
    flashOnClash = false;
    if (ledB.blinking()) {
        ledB.blink(0, 0);
    }
    syncToDB();
}

void buttonBClickHandler(const Button&) {
    Log.p("buttonBClickHandler").eol();
    if (bladeState.state() == BLADE_ON) {
        if (!paletteChange) {
            bladeState.change(BLADE_FLASH);
            #ifdef SABER_SOUND_ON
                sfx.playSound(SFX_USER_TAP, SFX_GREATER_OR_EQUAL);
            #endif
        }
    }
}

#else

void blinkVolumeHandler(const LEDManager& manager)
{
    saberDB.setVolume4(manager.numBlinks() - 1);
    syncToDB();
}

void blinkPaletteHandler(const LEDManager& manager)
{
    saberDB.setPalette(manager.numBlinks() - 1);
    syncToDB();
}

// One button case.
void buttonAClickHandler(const Button&)
{
    Log.p("buttonAClickHandler").eol();
    if (bladeState.bladeOff()) {
        buttonMode.nextMode();
    }
    else if (bladeState.state() == BLADE_ON) {
        bladeState.change(BLADE_FLASH);
        #ifdef SABER_SOUND_ON
            sfx.playSound(SFX_USER_TAP, SFX_GREATER_OR_EQUAL);
        #endif
    }
}

void buttonAHoldHandler(const Button&)
{
    Log.p("buttonAHoldHandler").eol();
    if (bladeState.state() == BLADE_OFF) {

        if (buttonMode.mode() == BUTTON_MODE_NORMAL) {
            bladeState.change(BLADE_IGNITE);
            #ifdef SABER_SOUND_ON
                sfx.playSound(SFX_POWER_ON, SFX_OVERRIDE);
            #endif
        }
        else if (buttonMode.mode() == BUTTON_MODE_PALETTE) {
            saberDB.setPalette(0);
            ledA.blink(SaberDB::NUM_PALETTES, INDICATOR_CYCLE, blinkPaletteHandler);
        }
        else if (buttonMode.mode() == BUTTON_MODE_VOLUME) {
            ledA.blink(5, INDICATOR_CYCLE, blinkVolumeHandler);
        }

    }
    else if (bladeState.state() != BLADE_RETRACT) {
        bladeState.change(BLADE_RETRACT);
        #ifdef SABER_SOUND_ON
            sfx.playSound(SFX_POWER_OFF, SFX_OVERRIDE);
        #endif
    }
}


#endif

void processBladeState()
{
    switch (bladeState.state()) {
    case BLADE_OFF:
        break;

    case BLADE_ON:
        blade.setRGB(saberDB.bladeColor());
        break;

    case BLADE_IGNITE:
    {
        uint32_t igniteTime = 1000;
        #ifdef SABER_SOUND_ON
            igniteTime = sfx.getIgniteTime();
        #endif
        bool done = blade.setInterp(millis() - bladeState.startTime(), igniteTime, BLADE_BLACK, saberDB.bladeColor());
        if (done) {
            bladeState.change(BLADE_ON);
        }
    }
    break;

    case BLADE_RETRACT:
    {
        uint32_t retractTime = 1000;
        #ifdef SABER_SOUND_ON
            retractTime = sfx.getRetractTime();
        #endif
        bool done = blade.setInterp(millis() - bladeState.startTime(), retractTime, saberDB.bladeColor(), BLADE_BLACK);
        if (done) {
            bladeState.change(BLADE_OFF);
        }
    }
    break;

    case BLADE_FLASH:
        // interpolate?
    {
        uint32_t delta = millis() - bladeState.startTime();
        if (delta > FLASH_TIME) {
            blade.setRGB(saberDB.bladeColor());
            bladeState.change(BLADE_ON);
        }
        else {
            blade.setRGB(saberDB.impactColor());
        }
    }
    break;

    default:
        ASSERT(false);
        break;
    }
}

void serialEvent() {
    bool processed = false;
    uint8_t color[NCHANNELS] = {0};

    while (Serial.available()) {
        int c = Serial.read();
        if (c == '\n') {
            #if 0
            Serial.print("event ");
            Serial.println(cmdParser.getBuffer());
            #endif
            processed = cmdParser.processCMD(color);
        }
        else {
            cmdParser.push(c);
        }
    }
    if (processed) {
        syncToDB();
    }
}

void loop() {
    const uint32_t msec = millis();
    tester.process();

    buttonA.process();
    ledA.process();
    #ifdef SABER_TWO_BUTTON
        buttonB.process();
        ledB.process();
    #endif

    tester.process();
    if (bladeState.state() == BLADE_ON) {
        float g2Normal = 1.0f;
        float g2 = 1.0f;
        float ax=0, ay=0, az=0;
        accel.read(&ax, &ay, &az, &g2, &g2Normal);

        maxGForce2 = max(maxGForce2, g2);
        float motion = saberDB.motion();
        float impact = saberDB.impact();

        if ((g2Normal >= impact * impact)) {
            bool sound = false;
            #ifdef SABER_SOUND_ON
                sound = sfx.playSound(SFX_IMPACT, SFX_GREATER_OR_EQUAL);
            #endif

            if (sound) {
                Log.p("Impact. g=").p(sqrt(g2)).eol();
                bladeState.change(BLADE_FLASH);
            }
        }
        else if ( g2 >= motion * motion) {
            bool sound = false;
            #ifdef SABER_SOUND_ON
                sound = sfx.playSound(SFX_MOTION, SFX_GREATER);
            #endif
            if (sound) {
                Log.p("Motion. g=").p(sqrt(g2)).eol();
            }
        }
    }
    else {
        maxGForce2 = 1;
    }

    processBladeState();
    tester.process();
    #ifdef SABER_SOUND_ON
        sfx.process();
    #endif
    tester.process();

    if (vbatTimer.tick()) {
        averagePower.push(readVbat());
        blade.setVoltage(averagePower.power());
    }

    if (gforceDataTimer.tick()) {
        #ifdef SABER_DISPLAY
            maxGForce2 = constrain(maxGForce2, 0.1, 16);
            static const float MULT = 256.0f / GFORCE_RANGE;  // g=1 translates to uint8 64
            const uint8_t gForce = constrain(sqrtf(maxGForce2) * MULT, 0, 255);
            sketcher.Push(gForce);
        #endif
        maxGForce2 = 0;
    }

    if (reflashTime && msec >= reflashTime) {
        reflashTime = 0;
        if (flashOnClash && bladeState.state() == BLADE_ON) {
            Log.event("[FlashOnClash]");
            bladeState.change(BLADE_FLASH);
            reflashTime = calcReflashTime();
        }
    }

    if (displayTimer.tick()) {
        int sketcherMode = Sketcher::BLADE_ON_MODE;
        (void)sketcherMode; // If no display, won't be used, but don't need a warning.

        #ifdef SABER_TWO_BUTTON
            if (bladeState.state() == BLADE_OFF) {
                sketcherMode = Sketcher::REST_MODE;
            }
        #else
            if (bladeState.state() == BLADE_OFF) {
                switch (buttonMode.mode()) {
                case BUTTON_MODE_PALETTE:
                    sketcherMode = Sketcher::PALETTE_MODE;
                    break;
                case BUTTON_MODE_VOLUME:
                    sketcherMode = Sketcher::VOLUME_MODE;
                    break;
                default:
                    sketcherMode = Sketcher::REST_MODE;
                    break;
                }
            }
        #endif
        #ifdef SABER_DISPLAY
            sketcher.Draw(&renderer, displayTimer.period(), sketcherMode, &uiRenderData);
            display.display(oledBuffer);
        #endif
        #ifdef SABER_LEDS
            dotstarUI.Draw(leds, sketcherMode, &uiRenderData);
            dotstar.display();
        #endif
    }

    #if defined(SABER_CRYSTAL)
        {
            const uint8_t* rgb = saberDB.bladeColor();
            if (bladeState.state() == BLADE_OFF) {
                uint8_t outColor[3];
                calcCrystalColor(msec, rgb, outColor);
                leds[0].set(outColor[0], outColor[1], outColor[2]);
            }
            else {
                leds[0].red   = rgb[0];
                leds[0].green = rgb[1];
                leds[0].blue  = rgb[2];
            }
            dotstar.display();
        }
    #endif
}
 

int32_t readVbat() {
    #ifdef SABER_VOLTMETER
        int32_t analog = analogRead(PIN_VMETER);
        int32_t mV = analog * UVOLT_MULT / int32_t(1000);
        return mV;
    #else
        return NOMINAL_VOLTAGE;
    #endif
}

