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

#include <Arduino.h>

#include "./src/lsm303/grinliz_LSM303.h"
#include "./src/lsm6d/grinliz_lsm6d.h"

#include "cmdparser.h"
#include "saberdb.h"
#include "bladePWM.h"
#include "SFX.h"
#include "Tester.h"
#include "saberUtil.h"
#include "voltmeter.h"

#include "src/wav12util/manifest.h"
#include "src/util/linear.h"

using namespace osbr;

extern ACCELEROMETER accelMag;

CMDParser::CMDParser(SaberDB& _db, BladeColor& _bladeColor, const Manifest& _manifest) : database(_db), bladeColor(_bladeColor), manifest(_manifest) {}

void CMDParser::tokenize()
{
    action.clear();
    value.clear();

    if (token.empty()) return;

    const char *p = token.c_str();
    while(*p && *p != ' ') {
        action.append(*p);
        ++p;
    }
    while(*p && *p == ' ') {
        ++p;
    }
    while(*p) {
        value.append(*p);
        ++p;
    }
}

void CMDParser::printHexColor(const RGB& color) {
    Serial.print('#');
    for (int i = 0; i < NCHANNELS; ++i) {
        Serial.print(color[i] / 16, HEX);
        Serial.print(color[i] & 0xf, HEX);
    }
}

void CMDParser::printAllColor(const osbr::RGB& color)
{
    Serial.print('#');
    for (int i = 0; i < NCHANNELS; ++i) {
        Serial.print(color[i] / 16, HEX);
        Serial.print(color[i] & 0xf, HEX);
    }
    Serial.print(" %(");
    for (int i = 0; i < NCHANNELS; ++i) {
        Serial.print(color[i] * 100 / 255);
        if (i < NCHANNELS - 1)
            Serial.print(' ');
    }
    Serial.print(") [");
    for (int i = 0; i < NCHANNELS; ++i) {
        Serial.print(color[i]);
        if (i < NCHANNELS - 1)
            Serial.print(' ');
    }
    Serial.print("]");
}


void CMDParser::printMAmps(const RGB& c) {
    Serial.print("(");
    uint32_t amps =   LinearToPWM1024(c[0]) * RED_I / uint32_t(1023) 
                    + LinearToPWM1024(c[1]) * GREEN_I / uint32_t(1023) 
                    + LinearToPWM1024(c[2]) * BLUE_I / uint32_t(1023);
    Serial.print(amps);
    Serial.print(" mAmps)");
}

void CMDParser::printLead(const char* str) {
    static const char* PREFIX = "[-]";
    Serial.print(PREFIX);
    Serial.print(str);
    Serial.print(' ');
}

bool CMDParser::push(int c)
{
    bool processed = false;
    if (c == '\n') {
        processed = processCMD();
    }
    else {
        token.append(c);
    }
    return processed;
}

bool CMDParser::processCMD() 
{
    static const char BC[]      = "bc";
    static const char IC[]      = "ic";
    static const char PAL[]     = "pal";
    static const char FONT[]    = "font";
    static const char VOL[]     = "vol";
    static const char VOLTS[]   = "vbat";
    static const char UTIL[]    = "util";
    static const char PWM[]     = "pwm";
    static const char MOTION[]  = "mot";
    static const char IMPACT[]  = "imp";
    static const char STATUS[]  = "stat";
    static const char ID[]      = "id";
    static const char TEST[]    = "test";
    static const char ACCEL[]   = "accel";
    static const char PLAY[]    = "play";

    static const int DELAY = 20;  // don't saturate the serial line. Too much for SoftwareSerial.

    RGB c(0);
    SerialLog serialLog;    // guarantees log is attached to Serial

    tokenize();
    bool isSet = !value.empty();

    if (action == BC) {
        if (isSet) {
            parseAllColor(value.c_str(), &c.r);
            bladeColor.setBladeColor(c);
        }
        printLead(action.c_str());
        printAllColor(bladeColor.getBladeColor());
        Serial.print(" ");
        printMAmps(bladeColor.getBladeColor());
        Serial.print('\n');
    }
    else if (action == IC) {
        if (isSet) {
            parseAllColor(value.c_str(), &c.r);
            bladeColor.setImpactColor(c);
        }
        printLead(action.c_str());
        printAllColor(bladeColor.getImpactColor());
        Serial.print(" ");
        printMAmps(bladeColor.getImpactColor());
        Serial.print('\n');
    }
    else if (action == PAL) {
        if (isSet) {
            int pal = atoi(value.c_str());
           changePalette(pal);
        }
        printLead(action.c_str());
        Serial.println(database.paletteIndex());
    }
    else if (action == FONT) {
        if (isSet) {
            int f = atoi(value.c_str());
            database.setSoundFont(f);
        }
        printLead(action.c_str());
        const MemUnit& memUnit = manifest.getUnit(database.soundFont());
        CStr<MemUnit::NAME_ALLOC> name = memUnit.name;
        Serial.println(name.c_str());
    }
    else if (action == VOL) {
        SFX* sfx = SFX::instance();

        if (isSet) {
            sfx->setVolume(atoi(value.c_str()));
        }
        printLead(action.c_str());
        Serial.println(sfx->getVolume());
    }
    else if (action == VOLTS) {
        printLead(action.c_str());
        Serial.println(Voltmeter::instance()->averagePower());
    }
    else if (action == UTIL) {
        printLead(action.c_str());
        for (int i = 0; i < NCHANNELS; ++i) {
            Serial.print(BladePWM::bladePWM().util(i));
            Serial.print("/100 ");
        }
        Serial.print('\n');
    }
    else if (action == PWM) {
        printLead(action.c_str());
        for (int i = 0; i < NCHANNELS; ++i) {
            Serial.print(BladePWM::bladePWM().pwmVal(i));
            Serial.print("/255 ");
        }
        Serial.print('\n');
    }
    else if (action == ID) {
        printLead(action.c_str());
        Serial.println(F(ID_STR));
    }
    else if (action == ACCEL) {
        accelMag.log(8);
    }
    else if (action == PLAY) {
        printLead(action.c_str());        
        SFX* sfx = SFX::instance();
        bool okay = sfx->playSound(value.c_str());
        Serial.print("Play returned ");
        Serial.println(okay ? "okay" : "error");
    }
    else if (action == TEST) {
        Tester* tester = Tester::instance();
        tester->runTests();
    }
    else if (action == STATUS) {
        static const char* space = "-----------";
        delay(DELAY);
        Serial.println(space);
        printLead(ID); Serial.println(F(ID_STR));
        Serial.println(desc.c_str());

        static const char* BUILD_ID = "SABER_MODEL: " STRINGIZE(SABER_MODEL) " SABER_SUB_MODEL: " STRINGIZE(SABER_SUB_MODEL) " PCB:" STRINGIZE(PCB_VERSION) " SERIAL_DEBUG: " STRINGIZE(SERIAL_DEBUG) " " __DATE__;
        Serial.println(BUILD_ID);
        
        delay(DELAY);
        printLead(PAL);
        Serial.println(database.paletteIndex());
        
        delay(DELAY);
        printLead(FONT);
        Serial.println(database.soundFont());
        
        delay(DELAY);
        printLead(VOL);
        SFX* sfx = SFX::instance();
        Serial.println(sfx->getVolume());
        
        delay(DELAY);
        printLead(MOTION);
        Serial.println(DEFAULT_G_FORCE_MOTION);
        
        delay(DELAY);
        printLead(IMPACT);
        Serial.println(DEFAULT_G_FORCE_IMPACT);

        delay(DELAY);
        printLead(VOLTS);
        Serial.println(Voltmeter::instance()->averagePower());

        printLead(UTIL);
        
        delay(DELAY);
        for (int i = 0; i < NCHANNELS; ++i) {
            Serial.print(BladePWM::bladePWM().util(i));
            Serial.print(' ');
        }
        Serial.print('\n');
        
        delay(DELAY);
        printLead(PWM);
        for (int i = 0; i < NCHANNELS; ++i) {
            Serial.print(BladePWM::bladePWM().pwmVal(i));
            Serial.print(' ');
        }
        Serial.print('\n');
        
        delay(DELAY);
        // font
        printLead(BC);
        printHexColor(bladeColor.getBladeColor());
        Serial.print(" ");
        printMAmps(bladeColor.getBladeColor());
        Serial.print('\n');

        delay(DELAY);
        printLead(IC);
        printHexColor(bladeColor.getImpactColor());
        Serial.print(" ");
        printMAmps(bladeColor.getImpactColor());
        Serial.print('\n');
        Serial.println(space);
    }
    token.clear();
    return isSet;
}
