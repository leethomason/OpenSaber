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

#include <Arduino.h>

#include "cmdparser.h"
#include "saberdb.h"
#include "blade.h"
#include "SFX.h"
#include "Tester.h"
#include "saberUtil.h"

CMDParser::CMDParser(SaberDB* _db) {
    database = _db;
}

void CMDParser::tokenize()
{
    action.clear();
    value.clear();
    if (token.empty()) return;

    int i = 0;
    for ( ; token[i] && token[i] != ' '; i++) {
        action.append(token[i]);
    }
    if (token[i] == ' ') {
        i++;
        for ( ; token[i]; i++) {
            value.append(token[i]);
        }
    }
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
    uint32_t amps = uint32_t(c[0]) * RED_I / uint32_t(255) + uint32_t(c[1]) * GREEN_I / uint32_t(255) + uint32_t(c[2]) * BLUE_I / uint32_t(255);
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

bool CMDParser::processCMD() 
{
    static const char BC[]      = "bc";
    static const char IC[]      = "ic";
    static const char PAL[]     = "pal";
    static const char FONT[]    = "font";
    static const char FONTS[]   = "fonts";
    static const char AUDIO[]   = "aud";
    static const char VOL[]     = "vol";
    static const char VOLTS[]   = "vbat";
    static const char UTIL[]    = "util";
    static const char PWM[]     = "pwm";
    static const char MOTION[]  = "mot";
    static const char IMPACT[]  = "imp";
    static const char STATUS[]  = "stat";
    static const char RESET[]   = "reset";
    static const char ID[]      = "id";
    static const char LIST[]    = "list";
    static const char TEST[]    = "test";
    static const char ACCEL[]   = "accel";
    static const char CRYSTAL[] = "crys";
    static const char PLAY[]    = "play";

    static const int DELAY = 20;  // don't saturate the serial line. Too much for SoftwareSerial.

    RGB c(0);

    tokenize();
    //Serial.print("CMD:"); Serial.print(action.c_str()); Serial.print(":"); Serial.println(value.c_str());
    bool isSet = !value.empty();

    if (action == BC) {
        if (isSet) {
            parseHexColor(value.c_str() + 1, &c);
            database->setBladeColor(c);
        }
        printLead(action.c_str());
        RGB c = database->bladeColor();
        printHexColor(c);
        Serial.print(" ");
        printMAmps(c);
        Serial.print('\n');
    }
    else if (action == IC) {
        if (isSet) {
            parseHexColor(value.c_str() + 1, &c);
            database->setImpactColor(c);
        }
        printLead(action.c_str());
        printHexColor(database->impactColor());
        Serial.print(" ");
        printMAmps(database->impactColor());
        Serial.print('\n');
    }
    else if (action == PAL) {
        if (isSet) {
            int pal = atoi(value.c_str());
            database->setPalette(pal);
        }
        printLead(action.c_str());
        Serial.println(database->paletteIndex());
    }
    else if (action == FONT) {
        if (isSet) {
            int f = atoi(value.c_str());
            const char* fontName = SFX::instance()->fontName(f);
            database->setSoundFont(fontName);
        }
        printLead(action.c_str());
        Serial.println(database->soundFont());
    }
    else if (action == AUDIO) {
        if (isSet) {
            int onOff = atoi(value.c_str());
            database->setSoundOn(onOff ? true : false);
        }
        printLead(action.c_str());
        Serial.println(database->soundOn());
    }
    else if (action == VOL) {
        if (isSet) {
            int volume = constrain(atoi(value.c_str()), 0, 204);
            database->setVolume(volume);
        }
        printLead(action.c_str());
        Serial.println(database->volume());
    }
    else if (action == VOLTS) {
        printLead(action.c_str());
        Serial.println(Blade::blade().voltage());
    }
    else if (action == UTIL) {
        printLead(action.c_str());
        for (int i = 0; i < NCHANNELS; ++i) {
            Serial.print(Blade::blade().util(i));
            Serial.print(' ');
        }
        Serial.print('\n');
    }
    else if (action == PWM) {
        printLead(action.c_str());
        for (int i = 0; i < NCHANNELS; ++i) {
            Serial.print(Blade::blade().pwmVal(i));
            Serial.print(' ');
        }
        Serial.print('\n');
    }
    else if (action == MOTION) {
        if (isSet) {
            float v = atof(value.c_str());
            database->setMotion(v);
        }
        printLead(action.c_str());
        Serial.println(database->motion());
    }
    else if (action == IMPACT) {
        if (isSet) {
            float v = atof(value.c_str());
            database->setImpact(v);
        }
        printLead(action.c_str());
        Serial.println(database->impact());
    }
    else if (action == RESET) {
        database->writeDefaults();
        Serial.println("reset complete.");
    }
    else if (action == ID) {
        printLead(action.c_str());
        Serial.println(F(ID_STR));
    }
    else if (action == FONTS) {
#ifdef SABER_SOUND_ON
        const SFX* sfx = SFX::instance();
        for (int i = 0; i < sfx->numFonts(); ++i) {
            Serial.print(i);
            Serial.print(": ");
            const char* name = sfx->fontName(i);
            Serial.println(name);
        }
#endif
    }
    else if (action == LIST) {
        File root = SD.open("/");
        while (true) {
            File entry =  root.openNextFile();
            if (!entry) {
                // no more files
                break;
            }
            if (entry.isDirectory()) {
                Serial.print("d: ");
            }
            Serial.println(entry.name());
            entry.close();
        }
        root.close();
    }
    else if (action == ACCEL) {
        float ax, ay, az, g2, g2n;
        // 0 value sometimes. Flush.
        Accelerometer::instance().read(&ax, &ay, &az, &g2, &g2n);        
        delay(20);
        Accelerometer::instance().read(&ax, &ay, &az, &g2, &g2n);
        Serial.print( "x="); Serial.print(ax);
        Serial.print(" y="); Serial.print(ay);
        Serial.print(" z="); Serial.print(az);
        Serial.print(" g="); Serial.print(sqrt(g2));
        Serial.print(" gN="); Serial.println(sqrt(g2n));
    }
    else if (action == CRYSTAL) {
        if (isSet) {
            parseHexColor(value.c_str() + 1, &c);
            database->setCrystalColor(c);
        }
        printLead(action.c_str());
        RGB c = database->crystalColor();
        printHexColor(c);
        Serial.println("");
    }
    else if (action == PLAY) {
        printLead(action.c_str());
        SFX* sfx = SFX::instance();
        sfx->playSound(value.c_str());
    }
    else if (action == TEST) {
        Tester* tester = Tester::instance();
        int count = 1;
        bool longTest = false;
        if (!value.empty()) {
            const char* p = value.c_str();
            if (*p == 'l') {
                p++;
                longTest = true;
            }
            if (*p) {
                count = atoi(p);
            }
        }
        tester->runTests(count, longTest);
    }
    else if (action == STATUS) {
        static const char* space = "-----------";
        delay(DELAY);
        Serial.println(space);
        printLead(ID);
        Serial.println(F(ID_STR));
        Serial.print("# setup() ");
        Serial.println(database->numSetupCalls());
        delay(DELAY);
        printLead(PAL);
        Serial.println(database->paletteIndex());
        delay(DELAY);
        printLead(FONT);
        Serial.println(database->soundFont());
        delay(DELAY);
        printLead(AUDIO);
        Serial.println(database->soundOn());
        delay(DELAY);
        printLead(VOL);
        Serial.println(database->volume());
        delay(DELAY);
        printLead(MOTION);
        Serial.println(database->motion());
        delay(DELAY);
        printLead(IMPACT);
        Serial.println(database->impact());
        delay(DELAY);
        printLead(VOLTS);
        Serial.println(Blade::blade().voltage());
        delay(DELAY);
        printLead(UTIL);
        delay(DELAY);
        for (int i = 0; i < NCHANNELS; ++i) {
            Serial.print(Blade::blade().util(i));
            Serial.print(' ');
        }
        Serial.print('\n');
        delay(DELAY);

        printLead(PWM);
        for (int i = 0; i < NCHANNELS; ++i) {
            Serial.print(Blade::blade().pwmVal(i));
            Serial.print(' ');
        }
        Serial.print('\n');
        delay(DELAY);

        // font
        printLead(BC);
        printHexColor(database->bladeColor());
        Serial.print(" ");
        printMAmps(database->bladeColor());
        Serial.print('\n');
        delay(DELAY);

        printLead(IC);
        printHexColor(database->impactColor());
        Serial.print(" ");
        printMAmps(database->impactColor());
        Serial.print('\n');
        Serial.println(space);
    }
    token.clear();
    return isSet;
}

