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
#include <OLED_SSD1306.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Adafruit_LIS3DH.h>
#include <NXPMotionSense.h>

#include "Button.h"
#include "Grinliz_Arduino_Util.h"
#include "DotStar.h"

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

static const uint32_t VBAT_TIME_INTERVAL      = 500;

static const uint32_t INDICATOR_TIME          = 500;
static const uint32_t INDICATOR_CYCLE         = INDICATOR_TIME * 2;
static const float    GFORCE_RANGE            = 4.0f;
static const float    GFORCE_RANGE_INV        = 1.0f / GFORCE_RANGE;

bool     paletteChange  = false;    // used to prevent sound fx on palette changes
uint32_t reflashTime    = 0;
bool     flashOnClash   = false;
float    maxGForce2     = 0.0f;
uint32_t lastMotionTime = 0;    
uint32_t meditationTimer = 0;  
uint32_t lastLoopTime   = 0;

#ifdef SABER_SOUND_ON
// First things first: disable that audio to avoid clicking.
AudioPlayer audioPlayer;
SFX sfx(&audioPlayer);
#endif

BladeState  bladeState;
ButtonCB    buttonA(PIN_SWITCH_A, SABER_BUTTON);
LEDManager  ledA(PIN_LED_A, false);
LEDManager  ledB(PIN_LED_B, false);  // Still have LEDB support, even if 1 button.

UIRenderData uiRenderData;

#ifdef SABER_TWO_BUTTON
ButtonCB    buttonB(PIN_SWITCH_B);
#else
UIModeUtil  uiMode;
#endif
SaberDB     saberDB;
Voltmeter voltmeter;

#ifdef SABER_NUM_LEDS
RGB leds[SABER_NUM_LEDS];
DotStar dotstar;
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

Tester tester;
uint32_t unlocked = 0;
bool soundOk = false;

void setupSD(int logCount)
{
    SPI.setMOSI(PIN_SABER_MOSI);
    SPI.setSCK(PIN_SABER_CLOCK);
    #if (SABER_SOUND_ON == SABER_SOUND_SD)
        #ifdef SABER_INTEGRATED_SD
            Log.p("Connecting to built in SD...").eol();
            soundOk = SD.begin(BUILTIN_SDCARD);
        #else
            Log.p("Connecting to SPI SD...").eol();
            soundOk = SD.begin(PIN_SDCARD_CS);
        #endif
        if (!soundOk) {
            Log.p("Unable to access the SD card").eol();
        }
    #elif (SABER_SOUND_ON == SABER_SOUND_FLASH)
        while(!(SerialFlash.begin(PIN_MEMORY_CS))) {
            Log.p("Unable to access SerialFlash memory.").eol();
            delay(500);
        }
        soundOk = true;
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
    #endif

    Log.p("setup()").eol();

    accel.begin();

    voltmeter.begin();

    blade.setRGB(RGB::BLACK);

    buttonA.setHoldHandler(buttonAHoldHandler);
    buttonA.setClickHandler(buttonAClickHandler);
    buttonA.setReleaseHandler(buttonAReleaseHandler);

    #ifdef SABER_TWO_BUTTON
        buttonB.setClickHandler(buttonBClickHandler);
        buttonB.setReleaseHandler(buttonBReleaseHandler);
        buttonB.setHoldHandler(buttonBHoldHandler);
        buttonB.setPressHandler(buttonBPressHandler);
    #endif

    #ifdef SABER_TWO_BUTTON
        tester.attach(&buttonA, &buttonB);
    #else
        tester.attach(&buttonA, 0);
    #endif
    #ifdef SABER_UI_START
        tester.attachUI(leds + SABER_UI_START);
    #endif

    #ifdef SABER_SOUND_ON
        if (soundOk) {
            sfx.init();
            Log.p("sfx initialized.").eol();
        }
        sfx.setEnabled(soundOk);
    #endif

    blade.setVoltage(voltmeter.averagePower());

    #ifdef SABER_DISPLAY
        display.begin(OLED_WIDTH, OLED_HEIGHT, SSD1306_SWITCHCAPVCC);
        renderer.Attach(OLED_WIDTH, OLED_HEIGHT, oledBuffer);
        renderer.Fill(0);
        display.display(oledBuffer);

        Log.p("OLED display connected.").eol();
    #endif

    #if defined(SABER_NUM_LEDS)
        dotstar.beginSPI(PIN_DOTSTAR_EN);
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

    #ifdef SABER_TWO_BUTTON
    buttonB.setHoldRepeats(true);  // volume repeats
    #else
    buttonA.setHoldRepeats(true);  // everything repeats!!
    #endif

    #ifdef SABER_UI_BRIGHTNESS
        dotstarUI.SetBrightness(SABER_UI_BRIGHTNESS);
    #endif
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
   The saberDB is the source of truth. (The Model.)
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
    uiRenderData.power = vbatToPowerLevel(voltmeter.averagePower());
    uiRenderData.mVolts = voltmeter.averagePower();
    #ifdef SABER_SOUND_ON
        uiRenderData.fontName = sfx.currentFontName();
    #endif

    // Only set ledB if not being used as UI
    if (buttonsReleased()) {
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

#ifdef MEDITATION_MODE
    if (uiMode.mode() == UIMode::MEDITATION && meditationTimer) {
        Log.p("med start").eol();
        #ifdef SABER_SOUND_ON
        sfx.playSound(SFX_SPIN, SFX_OVERRIDE, true);
        #endif
    }
#endif
}

bool setVolumeFromHoldCount(int count)
{
    saberDB.setVolume4(count - 1);
    syncToDB();
    return count >= 0 && count <= 5;
}

#ifdef SABER_TWO_BUTTON

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

void buttonBHoldHandler(const Button& button) {
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
        bool on = setVolumeFromHoldCount(button.nHolds());
        ledB.set(on);
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

bool setPaletteFromHoldCount(int count)
{
    saberDB.setPalette(count - 1);
    syncToDB();
    return count <= SaberDB::NUM_PALETTES;
}

bool setMeditationFromHoldCount(int count)
{
    switch(count) {
        case 1: meditationTimer = 1000 * 60 * 1; break;
        case 2: meditationTimer = 1000 * 60 * 2; break;
        case 3: meditationTimer = 1000 * 60 * 5; break;
        case 4: meditationTimer = 1000 * 60 * 10; break;
    }
    syncToDB();
    return count >= 1 && count <= 4;
}


// One button case.
void buttonAClickHandler(const Button&)
{
    Log.p("buttonAClickHandler").eol();
    if (bladeState.bladeOff()) {
        uiMode.nextMode();
    }
    else if (bladeState.state() == BLADE_ON) {
        bladeState.change(BLADE_FLASH);
        #ifdef SABER_SOUND_ON
            sfx.playSound(SFX_USER_TAP, SFX_GREATER_OR_EQUAL);
        #endif
    }
    meditationTimer = 0;
}

void buttonAHoldHandler(const Button& button)
{
    Log.p("buttonAHoldHandler nHolds=").p(button.nHolds()).eol();
    
    meditationTimer = 0;

    if (bladeState.state() == BLADE_OFF) {
        bool buttonOn = false;
        int cycle = button.cycle(&buttonOn);

        if (uiMode.mode() == UIMode::NORMAL) {
            if (button.nHolds() == 1) {
                bladeState.change(BLADE_IGNITE);
                #ifdef SABER_SOUND_ON
                    sfx.playSound(SFX_POWER_ON, SFX_OVERRIDE);
                #endif
            }
        }
        else 
        {
            if (uiMode.mode() == UIMode::PALETTE) {
                if (!setPaletteFromHoldCount(cycle))
                    buttonOn = false;
            }
            else if (uiMode.mode() == UIMode::VOLUME) {
                if (!setVolumeFromHoldCount(cycle))
                    buttonOn = false;
            }
            else if (uiMode.mode() == UIMode::MEDITATION) {
                if (!setMeditationFromHoldCount(cycle))
                    buttonOn = false;
            }
        }
        ledA.set(buttonOn);
    }
    else if (bladeState.state() != BLADE_RETRACT) {
        if (button.nHolds() == 1) {
            bladeState.change(BLADE_RETRACT);
            #ifdef SABER_SOUND_ON
                sfx.playSound(SFX_POWER_OFF, SFX_OVERRIDE);
            #endif
        }
    }
}

#endif

bool buttonsReleased()
{
    #ifdef SABER_TWO_BUTTON
        return !buttonA.isDown() && !buttonB.isDown();
    #else
        return !buttonA.isDown();
    #endif
}

void serialEvent() {
    bool processed = false;

    while (Serial.available()) {
        int c = Serial.read();
        if (c == '\n') {
            #if 0
            Serial.print("event ");
            Serial.println(cmdParser.getBuffer());
            #endif
            processed = cmdParser.processCMD();
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

    if (meditationTimer && buttonsReleased()) {
        uint32_t delta = msec - lastLoopTime;
        if (delta >= meditationTimer) {
            meditationTimer = 0;
            #ifdef SABER_SOUND_ON
            sfx.playSound(SFX_SPIN, SFX_OVERRIDE, true);            
            #endif
        }
        else {
            meditationTimer -= delta;
        }
    }

    tester.process();
    if (bladeState.state() == BLADE_ON) {
        float g2Normal = 1.0f;
        float g2 = 1.0f;
        float ax=0, ay=0, az=0;
        accel.read(&ax, &ay, &az, &g2, &g2Normal);

        maxGForce2 = max(maxGForce2, g2);
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
        #endif

        if ((g2Normal >= impact * impact)) {
            bool sound = false;
            #ifdef SABER_SOUND_ON
                sound = sfx.playSound(SFX_IMPACT, sfxOverDue ? SFX_OVERRIDE : SFX_GREATER_OR_EQUAL);
            #endif

            if (sound) {
                Log.p("Impact. g=").p(sqrt(g2)).eol();
                bladeState.change(BLADE_FLASH);
                lastMotionTime = msec;
            }
        }
        else if ( g2 >= motion * motion) {
            bool sound = false;
            #ifdef SABER_SOUND_ON
                sound = sfx.playSound(SFX_MOTION, sfxOverDue ? SFX_OVERRIDE : SFX_GREATER);
            #endif
            if (sound) {
                Log.p("Motion. g=").p(sqrt(g2)).eol();
                lastMotionTime = msec;
            }
        }
    }
    else {
        maxGForce2 = 1;
    }

    bladeState.process(&blade, saberDB, millis());
    tester.process();
    #ifdef SABER_SOUND_ON
        sfx.process();
    #endif
    tester.process();

    if (vbatTimer.tick()) {
        voltmeter.takeSample();
        //Log.p("power ").p(averagePower.power()).eol();
        blade.setVoltage(voltmeter.averagePower());
        uiRenderData.mVolts = voltmeter.averagePower();
        uiRenderData.power = vbatToPowerLevel(voltmeter.averagePower());
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
        uiRenderData.color = saberDB.bladeColor();
        uiRenderData.meditationTimeRemain = meditationTimer;

        #ifdef SABER_DISPLAY
            sketcher.Draw(&renderer, displayTimer.period(), uiMode.mode(), !bladeState.bladeOff(), &uiRenderData);
            display.display(oledBuffer);
        #endif
        #ifdef SABER_UI_START
            dotstarUI.Draw(leds + SABER_UI_START, uiMode.mode(), !bladeState.bladeOff(), uiRenderData);
        #endif
        //Log.p("crystal: ").p(leds[0].r).p(" ").p(leds[0].g).p(" ").p(leds[0].b).eol();
    }

    #if defined(SABER_CRYSTAL)
    {
        if (saberDB.crystalColor().get()) {
            // It has been set on the command line for testing.
            leds[0] = saberDB.crystalColor();
        }
        else {
            // Use the blade color or the breathing color.
            const RGB bladeColor = saberDB.bladeColor();
            if (bladeState.state() == BLADE_OFF && (uiMode.mode() == UIMode::NORMAL || uiMode.mode() == UIMode::MEDITATION)) {
                calcCrystalColor(msec, SABER_CRYSTAL_LOW, SABER_CRYSTAL, bladeColor, &leds[0]);
                //OSASSERT(leds[0].get());
                //OSASSERT(dotstar.brightness() == 256);
            }
            else {
                leds[0] = bladeColor;
            }
        }
    }
    #endif

    #ifdef SABER_NUM_LEDS
        dotstar.display();
    #endif
    lastLoopTime = msec;
}
 