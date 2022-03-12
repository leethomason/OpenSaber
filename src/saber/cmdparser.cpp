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
#include "./src/lis3dh/grinliz_lis3dh.h"
#include "./src/lsm6d/grinliz_lsm6d.h"

#include "cmdparser.h"
#include "saberdb.h"
#include "bladePWM.h"
#include "SFX.h"
#include "Tester.h"
#include "saberUtil.h"
#include "manifest.h"
#include "voltmeter.h"

using namespace osbr;

extern ACCELEROMETER accelMag;

CMDParser::CMDParser(SaberDB* _db, const Manifest& _manifest) : manifest(_manifest) {
    database = _db;
}

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
    else if (action == LSPAL) {
        for(int i=0; i<SaberDB::NUM_PALETTES; ++i) {
            const SaberDB::Palette* pal = database->getPalette(i);

            CStr<10> name;
            const MemUnit& memUnit = manifest.getUnit(database->soundFont());
            memUnit.name.toStr(&name);

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
            database->setPalette(pal);
        }
        printLead(action.c_str());
        Serial.println(database->paletteIndex());
    }
    else if (action == FONT) {
        if (isSet) {
            int f = atoi(value.c_str());
            database->setSoundFont(f);
        }
        printLead(action.c_str());
        CStr<10> name;
        const MemUnit& memUnit = manifest.getUnit(database->soundFont());
        memUnit.name.toStr(&name);
        Serial.println(name.c_str());
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
        uint32_t dirHash = manifest.dirHash();
        Serial.print("dirHash=");
        Serial.println(dirHash, HEX);
        for (int i = 0; i < MEM_IMAGE_NUM_DIR; ++i) {
            Serial.print(i);
            Serial.print(": ");

            CStr<10> name = manifest.getUnit(i).getName();
            Serial.println(name.c_str());
        }
#endif
    }
    else if (action == ACCEL) {
        accelMag.log(8);
        /*
        accelMag.flushAccel(0);

        uint32_t start = millis();
        static const int N = 10;
        int n = 0;
        Vec3<Fixed115> data[N];
        while(n < N) {
            if(accelMag.sampleAccel(data + n))
                n++;
        }
        float samplesPerSecond = N * 1000.0f / (millis() - start);

        for(int i=0; i<N; ++i) {
            Fixed115 g2, g2n;
            calcGravity2(data[i].x, data[i].y, data[i].z, &g2, &g2n);

            Log.v3(data[i]).p(" g2=").p(g2).p(" g2N=").p(g2n).p(" g=").p(g2.sqrt()).eol();
        }
        Log.p("Samples per second: ").p(samplesPerSecond).eol();

        delay(20);
        Vec3<int32_t> mag;
        if (!accelMag.hasMag()) {
            Log.p("No magnetometer.").eol();
        }
        else {
            if (!accelMag.sampleMag(&mag)) {
                Log.p("No magnetometer data.").eol();
            }
            else {
                int count = 0;
                uint32_t start = millis();
                while(count < 10) {
                    if (accelMag.sampleMag(&mag)) {
                        count++;

                        Vec3<int32_t> magMin = accelMag.getMagMin();
                        Vec3<int32_t> magMax = accelMag.getMagMax();
                        Vec3<int32_t> range = magMax - magMin;
                        Log.p("Mag=").v3(mag).p(" range=").v3(range).eol();
                    }
                }
                uint32_t end = millis();

                Log.p("Mag: ").v3(mag).eol();

                Vec3<int32_t> vMin = accelMag.getMagMin();
                Vec3<int32_t> vMax = accelMag.getMagMax();
                Vec3<int32_t> vDelta = vMax - vMin;

                Log.p("Mag delta: ").v3(vDelta).eol();
                Log.p("Mag/s: ").p(10 * 1000.0f / (end - start)).eol();
            }
        }*/
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
        Serial.println(database->paletteIndex());
        
        delay(DELAY);
        printLead(FONT);
        Serial.println(database->soundFont());
        
        delay(DELAY);
        printLead(VOL);
        Serial.print(database->volume());
        Serial.print(" audioOn=");
        Serial.println(database->soundOn());
        
        delay(DELAY);
        printLead(MOTION);
        Serial.println(DEFAULT_G_FORCE_MOTION);
        
        delay(DELAY);
        printLead(IMPACT);
        Serial.println(DEFAULT_G_FORCE_IMPACT);

        delay(DELAY);
        printLead(VOLTS);
        Serial.println(Voltmeter::instance()->averagePower());

        #if 0
        ComRF24* com = ComRF24::instance();
        if (com) {
            delay(DELAY);
            printLead("RF24");
            Serial.print("inUse="); 
            Serial.print(com->inUse());
            Serial.print(" isConnected=");
            Serial.print(com->isConnected());
            Serial.print(" role=");
            Serial.println(com->role());
        }
        
        delay(DELAY);
        #endif 
        
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
