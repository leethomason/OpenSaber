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
#include <SoftwareSerial.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SPIFlash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Audio.h>
#include <Button.h>
#include <Adafruit_LIS3DH.h>
#include <Grinliz_Arduino_Util.h>

// Includes
#include "sfx.h"
#include "AudioPlayer.h"
#include "pins.h"
#include "electrical.h"
#include "saberdb.h"
#include "cmdparser.h"
#include "blade.h"
#include "sketcher.h"
#include "renderer.h"
#include "saberUtil.h"

static const uint8_t  BLADE_BLACK[NCHANNELS]  = {0};
static const uint32_t FLASH_TIME        = 120;
static const uint32_t VCC_TIME_INTERVAL = 4000;

static const uint32_t INDICATOR_TIME = 500;
static const uint32_t INDICATOR_CYCLE = INDICATOR_TIME * 2;
static const float GFORCE_RANGE = 4.0f;
static const float GFORCE_RANGE_INV = 1.0f / GFORCE_RANGE;

bool     paletteChange = false; // used to prevent sound fx on palette changes
uint32_t reflashTime = 0;
bool     flashOnClash = false;
bool     soundWasOff = true;
float    maxGForce2 = 0.0f;
int32_t  lastVCC = NOMINAL_VOLTAGE;

BladeState bladeState;
ButtonCB buttonA(PIN_SWITCH_A, Button::PULL_UP);
LEDManager ledA(PIN_LED_A);
#ifdef SABER_TWO_BUTTON
ButtonCB buttonB(PIN_SWITCH_B, Button::PULL_UP);
LEDManager ledB(PIN_LED_B);
#else
ButtonMode buttonMode;
#endif
SaberDB saberDB;

#ifdef SABER_ACCELEROMETER
Adafruit_LIS3DH accel;
#endif

#ifdef SABER_DISPLAY
Adafruit_SSD1306 display(PIN_OLED_DC, PIN_OLED_RESET, PIN_OLED_CS);
Timer displayTimer(100);
Sketcher sketcher;
Renderer renderer;
#endif

#ifdef SABER_SOUND_ON
AudioPlayer audioPlayer;
SFX sfx(&audioPlayer);
#endif

CMDParser cmdParser(&saberDB);
Blade blade;
Timer vccTimer(VCC_TIME_INTERVAL);
Timer gforceDataTimer(110);

void setup() {
  Serial.begin(19200);  // still need to turn it on in case a command line is connected.
  while (!Serial) {
    delay(100);
  }
#if SERIAL_DEBUG == 1
  Serial.println("setup()");
#endif

  //TCNT1 = 0x7FFF; // set blue & green channels out of phase
  // Database is the "source of truth".
  // Set it up first.
  saberDB.readData();

#ifdef SABER_ACCELEROMETER
  Serial.println("Starting Accel");
  if (!accel.begin()) {
#if SERIAL_DEBUG == 1
    Serial.println(F("ACCELEROMETER ERROR"));
#endif
  }
  else {
#if SERIAL_DEBUG == 1
    Serial.println(F("ACCELEROMETER open."));
#endif
    accel.setRange(LIS3DH_RANGE_4_G);
    accel.setDataRate(LIS3DH_DATARATE_100_HZ);
  }
#endif

#ifdef SABER_SOUND_ON
#endif

#ifdef SABER_VOLTMETER
  analogReference(INTERNAL);  // 1.1 volts
  analogRead(PIN_VMETER);     // warm up the ADC to avoid spurious initial value.
#endif

  blade.setRGB(BLADE_BLACK);

  buttonA.holdHandler(buttonAHoldHandler);
  buttonA.clickHandler(buttonAClickHandler);

#ifdef SABER_TWO_BUTTON
  buttonB.clickHandler(buttonBClickHandler);
  buttonB.releaseHandler(buttonBReleaseHandler);
  buttonB.holdHandler(buttonBHoldHandler);
  buttonB.pressHandler(buttonBPressHandler);
#endif

#ifdef SABER_SOUND_ON
  sfx.init();
#endif

  blade.setVoltage(readVcc());

#ifdef SABER_DISPLAY
  display.begin(SSD1306_SWITCHCAPVCC);
  display.fillScreen(0);
  display.display();
  renderer.Attach(128, 32, display.getBuffer());

#if SERIAL_DEBUG == 1
  Serial.println(F("OLED display connected."));
#endif
#endif

#if SERIAL_DEBUG == 1
  Serial.println(F("Setup complete."));
#endif

  ledA.set(true); // "power on" light

#ifdef SABER_CRYSTAL
  pinMode(PIN_CRYSTAL_R, OUTPUT);
  pinMode(PIN_CRYSTAL_G, OUTPUT);
  pinMode(PIN_CRYSTAL_B, OUTPUT);
#endif
 
  syncToDB();

#ifdef SABER_SOUND_ON
  Serial.print("Font: "); Serial.println(sfx.currentFontName());
#endif
}

uint32_t calcReflashTime() {
  return millis() + random(500) + 200;
}

int vccToPowerLevel(int32_t vcc)
{
  int level = 0;
  if      (vcc > 3950) level = 4;
  else if (vcc > 3800) level = 3;
  else if (vcc > 3650) level = 2;
  else if (vcc > LOW_VOLTAGE) level = 1;
  return level;
}

void buttonAClickHandler(const Button&) {
  int32_t vcc = readVcc();
  ledA.blink(vccToPowerLevel(vcc), INDICATOR_CYCLE);
}


void buttonAHoldHandler(const Button&) {
#if SERIAL_DEBUG == 1
  Serial.println("buttonAHoldHandler");
#endif
  if (bladeState.state() == BLADE_OFF) {
    bladeState.change(BLADE_IGNITE);
#ifdef SABER_SOUND_ON
    sfx.playSound(SFX_POWER_ON, SFX_OVERRIDE);
#endif
  }
  else if (bladeState.state() != BLADE_RETRACT) {
    bladeState.change(BLADE_RETRACT);
#ifdef SABER_SOUND_ON
    sfx.playSound(SFX_POWER_OFF, SFX_OVERRIDE);
#endif
  }
}

/*
   The saberDB is the source of true. (The Model.)
   Bring other things in sync when it changes.
*/
void syncToDB()
{
  sfx.setFont(saberDB.soundFont());
  sfx.mute(!saberDB.soundOn());
  sfx.setVolume204(saberDB.volume());

#ifdef SABER_DISPLAY
  sketcher.volume = saberDB.volume4();
  for (int i = 0; i < 3; ++i) {
    sketcher.color[i] = saberDB.bladeColor()[i];
  }
  sketcher.palette = saberDB.paletteIndex();
  sketcher.power = vccToPowerLevel(lastVCC);
#endif

  //digitalWrite(PIN_LED_B, saberDB.soundOn() ? HIGH : LOW);
  ledB.set(saberDB.soundOn());
}

#ifdef SABER_TWO_BUTTON
void buttonBPressHandler(const Button&) {
  paletteChange = false;
  soundWasOff = !saberDB.soundOn();
}

void buttonBHoldHandler(const Button&) {
#if SERIAL_DEBUG == 1
  Serial.println("buttonBHoldHandler");
#endif
  if (bladeState.state() != BLADE_OFF) {
    if (!paletteChange) {
      bladeState.change(BLADE_FLASH);
#ifdef SABER_SOUND_ON
      sfx.playSound(SFX_USER_HOLD, SFX_OVERRIDE);
      flashOnClash = true;
#endif
      reflashTime = calcReflashTime();
    }
  }
  else if (bladeState.state() == BLADE_OFF) {
    if (saberDB.soundOn()) {
      saberDB.setSoundOn(false);
      syncToDB();
    }
    else {
      ledB.blink(4, INDICATOR_CYCLE);
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
  if (bladeState.state() == BLADE_OFF && ledB.blinking()) {
    int nBlinks = ledB.numBlinks();
    saberDB.setVolume4(nBlinks);
    syncToDB();
  }
}

void buttonBClickHandler(const Button&) {
#if SERIAL_DEBUG == 1
  Serial.println("buttonBClickHandler");
#endif
  if (bladeState.state() == BLADE_ON) {
    if (!paletteChange) {
      bladeState.change(BLADE_FLASH);
#ifdef SABER_SOUND_ON
      sfx.playSound(SFX_USER_TAP, SFX_GREATER_OR_EQUAL);
#endif
    }
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
        bool done = blade.setInterp(millis() - bladeState.startTime(), sfx.getIgniteTime(), BLADE_BLACK, saberDB.bladeColor());
        if (done) {
          bladeState.change(BLADE_ON);
        }
      }
      break;

    case BLADE_RETRACT:
      {
        bool done = blade.setInterp(millis() - bladeState.startTime(), sfx.getRetractTime(), saberDB.bladeColor(), BLADE_BLACK);
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
      break;
  }
}

void serialEvent() {
  bool processed = false;
  uint8_t color[NCHANNELS] = {0};

  while (Serial.available()) {
    int c = Serial.read();
    if (c == '\n') {
#if 1
      Serial.print("event "); Serial.println(cmdParser.getBuffer());
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

  buttonA.process();
  ledA.process();
#ifdef SABER_TWO_BUTTON
  buttonB.process();
  ledB.process();
#endif

  if (bladeState.state() == BLADE_ON) {
#ifdef SABER_ACCELEROMETER
    accel.read();
    float g2_noZ = accel.x_g * accel.x_g + accel.y_g * accel.y_g;
    float g2 = g2_noZ + accel.z_g * accel.z_g;
    maxGForce2 = max(maxGForce2, g2);

    float motion = saberDB.motion();
    float impact = saberDB.impact();

    if ((g2_noZ >= impact * impact)) {
#ifdef SABER_SOUND_ON
      bool sound = sfx.playSound(SFX_IMPACT, SFX_GREATER_OR_EQUAL);
#else
      bool sound = true;
#endif
      if (sound) {
#if SERIAL_DEBUG == 1
        Serial.print(F("Impact. g=")); Serial.println(sqrt(g2));
#endif
        bladeState.change(BLADE_FLASH);
      }
    }
    else if ( g2 >= motion * motion) {
      bool sound = true;
#ifdef SABER_SOUND_ON
      sound = sfx.playSound(SFX_MOTION, SFX_GREATER);
#endif
      if (sound) {
#if SERIAL_DEBUG == 1
        Serial.print(F("Motion. g=")); Serial.println(sqrt(g2));
#endif
      }
    }
#endif
  }
  else {
    maxGForce2 = 1;
  }

#ifdef SABER_TWO_BUTTON
  // Special case: color switch.
  if (buttonA.press() && buttonB.isDown()) {
    saberDB.nextPalette();
    paletteChange = true;
    cmdParser.bringPaletteCurrent();
    syncToDB();
  }
 #endif
  
  processBladeState();

#ifdef SABER_SOUND_ON
  sfx.process();
#endif

  if (vccTimer.tick()) {
    blade.setVoltage(readVcc());
    //Serial.println("vcc");
  }

  if (gforceDataTimer.tick()) {
#ifdef SABER_DISPLAY
    maxGForce2 = constrain(maxGForce2, 0.1, 16);
    static const float MULT = 256.0f / GFORCE_RANGE;  // g=1 translates to uint8 64
    const uint8_t gForce = constrain(sqrtf(maxGForce2) * MULT, 0, 255);
    sketcher.Push(gForce);
    //Serial.print("gforce "); Serial.println(gForce);
#endif
    maxGForce2 = 0;
  }

  if (reflashTime && msec >= reflashTime) {
    reflashTime = 0;
    if (flashOnClash && bladeState.state() == BLADE_ON) {
      bladeState.change(BLADE_FLASH);
      reflashTime = calcReflashTime();
    }
  }

#ifdef SABER_DISPLAY
  if (displayTimer.tick()) {
    //Serial.println("display");
    sketcher.Draw(&renderer, displayTimer.period(), bladeState.state() == BLADE_OFF ? Sketcher::REST_MODE : Sketcher::BLADE_ON_MODE);
    // see: SerialFlashChip.cpp in the teensy hardware source.
    SPI.beginTransaction(SPISettings(50000000, MSBFIRST, SPI_MODE0));
    display.display();
    SPI.endTransaction();
  }
#endif

#ifdef SABER_CRYSTAL
  const uint8_t* rgb = saberDB.bladeColor();
  analogWrite(PIN_CRYSTAL_R, rgb[0]);
  analogWrite(PIN_CRYSTAL_G, rgb[1]);
  analogWrite(PIN_CRYSTAL_B, rgb[2]);
#endif
}

int32_t readVcc() {
#ifdef SABER_VOLTMETER
  int32_t analog = analogRead(PIN_VMETER);
  int32_t mV = analog * UVOLT_MULT / int32_t(1000);
  lastVCC = mV;
  return mV;
#else
  return NOMINAL_VOLTAGE;
#endif
}


