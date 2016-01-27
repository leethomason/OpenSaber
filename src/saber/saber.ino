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
//DO NOT INCLUDE: #include <EEPROM.h> has a static global class. (!?)
#include <SoftwareSerial.h>
#include <Wire.h>

// Libraries.
#include <Button.h>

#include <Adafruit_Soundboard.h>
#include <Adafruit_LIS3DH.h>
#include <Grinliz_Arduino_Util.h>

// Includes
#include "sfx.h"
#include "pins.h"
#include "electrical.h"
#include "saberdb.h"
#include "cmdparser.h"
#include "blade.h"

static const uint8_t  BLACK[NCHANNELS]  = {0};
static const uint32_t FLASH_TIME        = 120;
static const uint32_t VCC_TIME_INTERVAL = 4000;

enum {  BLADE_OFF,
        BLADE_IGNITE,
        BLADE_ON,
        BLADE_FLASH,
        BLADE_RETRACT
     };

static const uint32_t INDICATOR_TIME = 500;
uint8_t  currentState = BLADE_OFF;
uint32_t stateStartTime = 0;
uint32_t lastVccTime = 0;
bool     paletteChange = false; // used to prevent sound fx on palette changes
uint8_t  nIndicator = 0;
uint32_t indicatorStart = 0;
uint32_t reflashTime = 0;
bool     flashOnClash = false;
uint8_t  volumeRequest = 0;
bool     soundWasOff = true;

ButtonCB buttonA(PIN_SWITCH_A, Button::PULL_UP);
ButtonCB buttonB(PIN_SWITCH_B, Button::PULL_UP);
SaberDB saberDB;
#ifdef SABER_ACCELEROMETER
Adafruit_LIS3DH accel;
#endif

#ifdef SABER_SOUND_ON
SoftwareSerial softSer(PIN_SFX_TX, PIN_SFX_RX);
SFX sfx(&softSer);
#endif

CMDParser cmdParser(&saberDB);
Blade blade;

void setup() {
  pinMode(PIN_LED_LOW_POWER, OUTPUT);
  digitalWrite(PIN_LED_LOW_POWER, LOW);
  Serial.begin(19200);  // still need to turn it on in case a command line is connected.
#if SERIAL_DEBUG == 1
  Serial.println("setup()");
#endif
  saberDB.log("STR");

  TCNT1 = 0x7FFF; // set blue & green channels out of phase

  saberDB.log("ACC");
#ifdef SABER_ACCELEROMETER
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

  saberDB.log("SFX");
#ifdef SABER_SOUND_ON
  softSer.begin(9600);
  sfx.init();
#endif

  saberDB.log("VLT");
#ifdef SABER_VOLTMETER
  analogReference(INTERNAL);  // 1.1 volts
  analogRead(PIN_VMETER);        // warm up the DAC to avoid spurious initial value.
#endif

  saberDB.log("BTN");
  blade.setRGB(BLACK);

  buttonA.holdHandler(onOffHandler);
  buttonA.clickHandler(checkPowerHandler);

  buttonB.clickHandler(buttonBClickHandler);
  buttonB.releaseHandler(buttonBReleaseHandler);
  buttonB.holdHandler(buttonBHoldHandler);
  buttonB.pressHandler(buttonBPressHandler);

  saberDB.log("EEP");
  saberDB.readData();

#ifdef SABER_SOUND_ON
  sfx.setSoundOn(saberDB.soundOn());
  sfx.setVol(saberDB.volume());
#endif

  saberDB.log("VCC");
  blade.setVoltage(readVcc());
  lastVccTime = millis();

#if SERIAL_DEBUG == 1
  Serial.println(F("Setup complete."));
#endif

  // "power" lights
  pinMode(PIN_LED_A, OUTPUT);
  digitalWrite(PIN_LED_A, HIGH);
  pinMode(PIN_LED_B, OUTPUT);
  digitalWrite(PIN_LED_B, saberDB.soundOn() ? HIGH : LOW);
  saberDB.log("END");
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
  millis() + random(500) + 200;
}

void checkPowerHandler(const Button&) {
  int32_t vcc = readVcc();
  if      (vcc > 3950) nIndicator = 4;
  else if (vcc > 3800) nIndicator = 3;
  else if (vcc > 3650) nIndicator = 2;
  else if (vcc > LOW_VOLTAGE) nIndicator = 1;
  indicatorStart = millis();
}

void setSound() {
#ifdef SABER_SOUND_ON
  if (currentState == BLADE_OFF) {
    if (sfx.getVol() != saberDB.volume()) {
#if SERIAL_DEBUG == 1
      Serial.print("setSound cycle. on=");
      Serial.print(saberDB.soundOn() ? 1 : 0);
      Serial.print(" vol=");
      Serial.println(saberDB.volume());
#endif
      digitalWrite(PIN_LED_B, LOW);
      // Setting the volume after the blade has been used just...turns
      // the output to junk. And sets the volume to max. I think a bug
      // in the Audio FX?
      sfx.init();
      sfx.setVol(saberDB.volume());
      sfx.setSoundOn(saberDB.soundOn());
    }
    else if (sfx.isSoundOn() != saberDB.soundOn()) {
#if SERIAL_DEBUG == 1
      Serial.print("setSound on/off. on=");
      Serial.print(saberDB.soundOn() ? 1 : 0);
      Serial.print(" vol=");
      Serial.println(saberDB.volume());
#endif
      sfx.setSoundOn(saberDB.soundOn());
    }
    digitalWrite(PIN_LED_B, saberDB.soundOn());
  }
#endif
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
    setSound();
  }
}

void buttonBReleaseHandler(const Button& b) {
  if (flashOnClash && currentState != BLADE_OFF) {
    sfx.playSound(SFX_IDLE, SFX_OVERRIDE);
  }
  flashOnClash = false;
  if (currentState == BLADE_OFF) {
    if (soundWasOff && volumeRequest) {
      if (volumeRequest == 1)       saberDB.setVolume(160);
      else if (volumeRequest == 2)  saberDB.setVolume(170);
      else if (volumeRequest == 3)  saberDB.setVolume(185);
      else                          saberDB.setVolume(200);
      saberDB.setSoundOn(true);
    }
    setSound();
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
        bool done = blade.setInterp(millis() - stateStartTime, sfx.getIgniteTime(), BLACK, saberDB.bladeColor());
        if (done) {
          changeState(BLADE_ON);
        }
      }
      break;

    case BLADE_RETRACT:
      {
        bool done = blade.setInterp(millis() - stateStartTime, sfx.getRetractTime(), saberDB.bladeColor(), BLACK);
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

void readSerial() {
  // Flush the buffer, optionally echo.
#ifdef SABER_SOUND_ON
  while (softSer.available()) {
    // This read seems important to fixing random hangs in comm
    // to the AudioFX. Flushing a buffer? timing?
    int v = softSer.read();
    (void)v;
#if SERIAL_DEBUG == 1
    Serial.write(v);
#endif
  }
#endif
}

void serialEvent() {
  bool processed = false;
  uint8_t color[NCHANNELS] = {0};
  int savedVolume = saberDB.volume();

  while (Serial.available()) {
    int c = Serial.read();
    if (c == '\n') {
#if 0
      Serial.print("event "); Serial.println(cmdParser.getBuffer());
#endif
      if (*cmdParser.getBuffer() == '$') {
        // sound!
        softSer.println(cmdParser.getBuffer() + 1);
        cmdParser.clearBuffer();
      }
      else {
        processed = cmdParser.processCMD(color);
      }
    }
    else {
      cmdParser.push(c);
    }
  }
  if (processed) {
    setSound();
  }
}

void loop() {
  buttonA.process();
  buttonB.process();

  if (bladeOn()) {
#ifdef SABER_ACCELEROMETER
    accel.read();
    float g2_noZ = accel.x_g * accel.x_g + accel.y_g * accel.y_g;
    float g2 = g2_noZ + accel.z_g * accel.z_g;

    if (currentState == BLADE_ON) {
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
#ifdef SABER_SOUND_ON
        bool sound = sfx.playSound(SFX_MOTION, SFX_GREATER);
#else
        bool sound = true;
#endif
        if (sound) {
#if SERIAL_DEBUG == 1
          Serial.print(F("Motion. g=")); Serial.println(sqrt(g2));
#endif
        }
      }
    }
#endif
  }

  // Special case: color switch.
  if (buttonA.press() && buttonB.isDown()) {
    saberDB.nextPalette();
    paletteChange = true;
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

  readSerial();

  const uint32_t msec = millis();

  if (msec - lastVccTime > VCC_TIME_INTERVAL) {
    blade.setVoltage(readVcc());
    lastVccTime = msec;
  }
  if (nIndicator) {
    if (nIndicator && (msec - indicatorStart) >= INDICATOR_TIME) {
      nIndicator--;
      indicatorStart = msec;
    }
    uint32_t state = nIndicator && (msec - indicatorStart) < INDICATOR_TIME / 2 ? HIGH : LOW;
    digitalWrite(PIN_LED_LOW_POWER, state);
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
}


int32_t readVcc() {
#ifdef SABER_VOLTMETER
  int32_t analog = analogRead(PIN_VMETER);
  int32_t mV = analog * UVOLT_MULT / int32_t(1000);
  return mV;
#else
  return NOMINAL_VOLTAGE;
#endif
}

