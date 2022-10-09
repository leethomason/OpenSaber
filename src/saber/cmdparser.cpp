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

using namespace osbr;

extern ACCELEROMETER accelMag;

CMDParser::CMDParser(SaberDB& _db, BladeColor& _bladeColor, const Manifest& _manifest) : database(_db), bladeColor(_bladeColor), manifest(_manifest) {}

void CMDParser::tokenize()
{
    action.clear();
    value.clear();
    value2.clear();

    if (token.empty()) return;

    CStr<ALLOCATE>* inputs[3] = { &action, &value, &value2 };
    token.tokenize(' ', inputs, 3);
}

void CMDParser::printHexColor(const RGB& color) {
    Serial.print('#');
    for (int i = 0; i < NCHANNELS; ++i) {
        Serial.print(color[i] / 16, HEX);
        Serial.print(color[i] & 0xf, HEX);
    }
}

void CMDParser::printMAmps(const RGB& c) {
    Serial.print("(");
    uint32_t amps =   uint32_t(c[0]) * RED_I / uint32_t(255) 
                    + uint32_t(c[1]) * GREEN_I / uint32_t(255) 
                    + uint32_t(c[2]) * BLUE_I / uint32_t(255);
    Serial.print(amps);
    Serial.print(" mAmps)");
}

void CMDParser::parseHexColor(const char* str, RGB* c) {
    if (str[3] == 0) {
        CStr<4> cstr = str;
        parseHex(cstr, &c->r);
    }
    else {
        CStr<7> cstr = str;
        parseHex(cstr, &c->r);
    }
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
        //Serial.println("process");
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
    static const char LSPAL[]   = "lspal";
    static const char PAL[]     = "pal";
    static const char FONT[]    = "font";
    static const char FONTS[]   = "fonts";
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
    //Serial.print("CMD:"); Serial.print(action.c_str()); Serial.print(":"); Serial.println(value.c_str());
    bool isSet = !value.empty();

    if (action == BC) {
        if (isSet) {
            parseHexColor(value.c_str() + 1, &c);
            bladeColor.setBladeColor(c);
        }
        printLead(action.c_str());
        printHexColor(bladeColor.getBladeColor());
        Serial.print(" ");
        printMAmps(bladeColor.getBladeColor());
        Serial.print('\n');
    }
    else if (action == IC) {
        if (isSet) {
            parseHexColor(value.c_str() + 1, &c);
            bladeColor.setImpactColor(c);
        }
        printLead(action.c_str());
        printHexColor(bladeColor.getImpactColor());
        Serial.print(" ");
        printMAmps(bladeColor.getImpactColor());
        Serial.print('\n');
    }
    else if (action == LSPAL) {
        for(int i=0; i<SaberDB::NUM_PALETTES; ++i) {
            const SaberDB::Palette* pal = database.getPalette(i);

            const MemUnit& memUnit = manifest.getUnit(database.soundFont());
            CStr<MemUnit::NAME_ALLOC> name = memUnit.name;

            Log.p(i).p(": ")
               .p(name.c_str())
               .p(" ")
               .ptc(pal->bladeColor)
               .p(" ")
               .ptc(pal->impactColor)
               .eol();
        }
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
    else if (action == FONTS) {
#ifdef SABER_SOUND_ON
        //uint32_t dirHash = manifest.dirHash();
        //Serial.print("dirHash=");
        //Serial.println(dirHash, HEX);
        for (int i = 0; i < MemImage::NUM_DIR; ++i) {
            Serial.print(i);
            Serial.print(": ");

            CStr<MemUnit::NAME_ALLOC> name = manifest.getUnit(i).name;
            Serial.println(name.c_str());
        }
#endif
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
        printLead(ID);
        Serial.println(F(ID_STR));

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
