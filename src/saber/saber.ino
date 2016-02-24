/*  sketcher.palette = saberDB.currentPalette();ard Software

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
#include <SerialFlash.h>
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

static const uint8_t  BLADE_BLACK[NCHANNELS]  = {0};
static const uint32_t FLASH_TIME        = 120;
static const uint32_t VCC_TIME_INTERVAL = 4000;

enum {  BLADE_OFF,
        BLADE_IGNITE,
        BLADE_ON,
        BLADE_FLASH,
        BLADE_RETRACT
     };

static const uint32_t INDICATOR_TIME = 500;
static const float GFORCE_RANGE = 4.0f;
static const float GFORCE_RANGE_INV = 1.0f / GFORCE_RANGE;

uint8_t  currentState = BLADE_OFF;
uint32_t stateStartTime = 0;
uint32_t lastLoopTime = 0;
bool     paletteChange = false; // used to prevent sound fx on palette changes
uint8_t  nIndicator = 0;
uint32_t indicatorStart = 0;
uint32_t reflashTime = 0;
bool     flashOnClash = false;
uint8_t  volumeRequest = 0;
bool     soundWasOff = true;
float    maxGForce2 = 0.0f;
int32_t  lastVCC = 3700;

ButtonCB buttonA(PIN_SWITCH_A, Button::PULL_UP);
ButtonCB buttonB(PIN_SWITCH_B, Button::PULL_UP);
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
Timer gforceDataTimer(100);

void setup() {
  Serial.begin(19200);  // still need to turn it on in case a command line is connected.
  while (!Serial) {
    delay(100);
  }
#if SERIAL_DEBUG == 1
  Serial.println("setup()");
#endif

  //TCNT1 = 0x7FFF; // set blue & green channels out of phase

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

  saberDB.readData();

#ifdef SABER_SOUND_ON
#endif

#ifdef SABER_VOLTMETER
  analogReference(INTERNAL);  // 1.1 volts
  analogRead(PIN_VMETER);     // warm up the ADC to avoid spurious initial value.
#endif

  blade.setRGB(BLADE_BLACK);

  buttonA.holdHandler(onOffHandler);
  buttonA.clickHandler(checkPowerHandler);

  buttonB.clickHandler(buttonBClickHandler);
  buttonB.releaseHandler(buttonBReleaseHandler);
  buttonB.holdHandler(buttonBHoldHandler);
  buttonB.pressHandler(buttonBPressHandler);

#ifdef SABER_SOUND_ON
  sfx.init();
  Serial.print("Font: "); Serial.println(sfx.currentFontName());
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

  // "power" lights
  pinMode(PIN_LED_A, OUTPUT);
  digitalWrite(PIN_LED_A, HIGH);
  pinMode(PIN_LED_B, OUTPUT);
  syncToDB();

  lastLoopTime = millis();
}

void changeState(uint8_t state)
{
  currentState   = state;
  stateStartTime = millis();
}

void onOffHandler(const Button&) {
#if SERIAL_DEBUG == 1
  Serial.println("onOffHandler");
#endif
  if (currentState == BLADE_OFF) {
    changeState(BLADE_IGNITE);
#ifdef SABER_SOUND_ON
    sfx.playSound(SFX_POWER_ON, SFX_OVERRIDE);
#endif
  }
  else if (currentState != BLADE_RETRACT) {
    changeState(BLADE_RETRACT);
#ifdef SABER_SOUND_ON
    sfx.playSound(SFX_POWER_OFF, SFX_OVERRIDE);
#endif
  }
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

void checkPowerHandler(const Button&) {
  int32_t vcc = readVcc();
  nIndicator = vccToPowerLevel(vcc);
  indicatorStart = millis();
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

  sketcher.volume = saberDB.volume4();
  for (int i = 0; i < 3; ++i) {
    sketcher.color[i] = saberDB.bladeColor()[i];
  }
  sketcher.palette = saberDB.paletteIndex();
  sketcher.power = vccToPowerLevel(lastVCC);

  digitalWrite(PIN_LED_B, saberDB.soundOn() ? HIGH : LOW);
}

void buttonBPressHandler(const Button&) {
  paletteChange = false;
  volumeRequest = 0;
  soundWasOff = !saberDB.soundOn();
}

void buttonBHoldHandler(const Button&) {
#if SERIAL_DEBUG == 1
  Serial.println("buttonBHoldHandler");
#endif
  if (currentState != BLADE_OFF) {
    if (!paletteChange) {
      changeState(BLADE_FLASH);
#ifdef SABER_SOUND_ON
      sfx.playSound(SFX_USER_HOLD, SFX_OVERRIDE);
      flashOnClash = true;
#endif
      reflashTime = calcReflashTime();
    }
  }
  else if (currentState == BLADE_OFF && saberDB.soundOn()) {
    saberDB.setSoundOn(false);
    syncToDB();
  }
}

void buttonBReleaseHandler(const Button& b) {
  if (flashOnClash && currentState != BLADE_OFF) {
#ifdef SABER_SOUND_ON
    sfx.playSound(SFX_IDLE, SFX_OVERRIDE);
#endif
  }
  flashOnClash = false;
  if (currentState == BLADE_OFF) {
    if (soundWasOff && volumeRequest) {
      if (volumeRequest == 1)       saberDB.setVolume4(1);
      else if (volumeRequest == 2)  saberDB.setVolume4(2);
      else if (volumeRequest == 3)  saberDB.setVolume4(3);
      else                          saberDB.setVolume4(4);
    }
    syncToDB();
  }
}

void buttonBClickHandler(const Button&) {
#if SERIAL_DEBUG == 1
  Serial.println("buttonBClickHandler");
#endif
  if (currentState == BLADE_ON) {
    if (!paletteChange) {
      changeState(BLADE_FLASH);
#ifdef SABER_SOUND_ON
      sfx.playSound(SFX_USER_TAP, SFX_GREATER_OR_EQUAL);
#endif
    }
  }
}

bool bladeOn() {
  return currentState >= BLADE_ON && currentState < BLADE_RETRACT;
}

void processBladeState()
{
  switch (currentState) {
    case BLADE_OFF:
      break;

    case BLADE_ON:
      blade.setRGB(saberDB.bladeColor());
      break;

    case BLADE_IGNITE:
      {
#ifdef SABER_SOUND_ON
        bool done = blade.setInterp(millis() - stateStartTime, sfx.getIgniteTime(), BLADE_BLACK, saberDB.bladeColor());
#else
        bool done = blade.setInterp(millis() - stateStartTime, 1000, BLADE_BLACK, saberDB.bladeColor());
#endif
        if (done) {
          changeState(BLADE_ON);
        }
      }
      break;

    case BLADE_RETRACT:
      {
#ifdef SABER_SOUND_ON
        bool done = blade.setInterp(millis() - stateStartTime, sfx.getRetractTime(), saberDB.bladeColor(), BLADE_BLACK);
#else
        bool done = blade.setInterp(millis() - stateStartTime, 1000, saberDB.bladeColor(), BLADE_BLACK);
#endif
        if (done) {
          changeState(BLADE_OFF);
        }
      }
      break;

    case BLADE_FLASH:
      // interpolate?
      {
        uint32_t delta = millis() - stateStartTime;
        if (delta > FLASH_TIME) {
          blade.setRGB(saberDB.bladeColor());
          changeState(BLADE_ON);
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
  uint32_t deltaTime = msec - lastLoopTime;
  lastLoopTime = msec;

  buttonA.process();
  buttonB.process();

  if (bladeOn()) {
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
        changeState(BLADE_FLASH);
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

  // Special case: color switch.
  if (buttonA.press() && buttonB.isDown()) {
    saberDB.nextPalette();
    paletteChange = true;
    cmdParser.bringPaletteCurrent();
    sketcher.palette = saberDB.paletteIndex();
  }
  processBladeState();
#ifdef SABER_SOUND_ON
  sfx.process();
#endif

  // Special case: turn on/off volume.
  if (currentState == BLADE_OFF && buttonB.isDown() && soundWasOff) {
    uint32_t thresh = buttonB.holdThreshold();
    if (buttonB.holdTime() >= thresh) {
      uint32_t onTime = buttonB.holdTime() - thresh;
      volumeRequest  = onTime / (thresh * 2) + 1;
      uint32_t state = onTime / (thresh);
      digitalWrite(PIN_LED_B, (state & 1) ? LOW : HIGH);
    }
  }

  if (vccTimer.delta(deltaTime)) {
    blade.setVoltage(readVcc());
  }

  if (gforceDataTimer.delta(deltaTime)) {
    const float xm1 = constrain(maxGForce2, 0, 16);
    const uint8_t gForce = uint8_t(255.5f * (1.0f + 0.4 * xm1 - 0.0425 * xm1 * xm1 + 0.0015 * xm1 * xm1 * xm1));
    sketcher.Push(gForce);
    maxGForce2 = 0;
  }

  if (nIndicator) {
    if (nIndicator && (msec - indicatorStart) >= INDICATOR_TIME) {
      nIndicator--;
      indicatorStart = msec;
    }
    uint32_t state = nIndicator && (msec - indicatorStart) < INDICATOR_TIME / 2 ? HIGH : LOW;
    digitalWrite(PIN_LED_A, state);
    if (nIndicator == 0) {
      digitalWrite(PIN_LED_A, HIGH);
    }
  }
  if (reflashTime && msec >= reflashTime) {
    reflashTime = 0;
    if (flashOnClash && currentState == BLADE_ON) {
      changeState(BLADE_FLASH);
      reflashTime = calcReflashTime();
    }
  }

#ifdef SABER_DISPLAY
  if (displayTimer.delta(deltaTime)) {
    sketcher.Draw(&renderer, msec, !bladeOn());
    // see: SerialFlashChip.cpp in the teensy hardware source.
    SPI.beginTransaction(SPISettings(50000000, MSBFIRST, SPI_MODE0));
    display.display();
    SPI.endTransaction();
  }
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


