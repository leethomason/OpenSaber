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
#include "electrical.h"
#include "SFX.h"

CMDParser::CMDParser(SaberDB* _db) {
  database = _db;
}

void CMDParser::tokenize()
{
//  Serial.print("token ");  Serial.println(token.c_str());  
  action.clear();
  value.clear();
  if (token.empty()) return;

//  Serial.println("not empty");

  int i = 0;
  //Serial.println(i);
  for( ; token[i] && token[i] != ' '; i++) {
    action.append(token[i]);
  }
  //Serial.println(i);
  if (token[i] == ' ') {
    i++;
    for( ; token[i]; i++) {
      value.append(token[i]);
    }
  }
//  Serial.println(i);
//  Serial.print("action "); Serial.println(action.c_str());
//  Serial.print("value  "); Serial.println(value.c_str());
}

void CMDParser::printHexColor(const uint8_t* color) {
  Serial.print('#');
  for (int i = 0; i < NCHANNELS; ++i) {
    Serial.print(color[i] / 16, HEX);
    Serial.print(color[i] & 0xf, HEX);
  }
}

void CMDParser::parseHexColor(const char* str, uint8_t* c) {
  return parseNHex(str, c, NCHANNELS);
}

void CMDParser::printLead(const char* str) {
  static const char* PREFIX = "[-]";
  Serial.print(PREFIX); Serial.print(str); Serial.print(' '); 
}

bool CMDParser::processCMD(uint8_t* c) {
  for (int i = 0; i < NCHANNELS; ++i) {
    c[i] = 0;
  }

  static const char BC[]      = "bc";
  static const char IC []     = "ic";
  static const char PAL []    = "pal";
  static const char FONT []   = "font";
  static const char AUDIO[]   = "aud";
  static const char VOL[]     = "vol";
  static const char AMPS[]    = "amp";
  static const char VOLTS[]   = "vbat";
  static const char UTIL[]    = "util";
  static const char PWM[]     = "pwm";
  static const char MOTION[]  = "mot";
  static const char IMPACT[]  = "imp";
  static const char STATUS[]  = "stat";
  static const char RESET[]   = "reset";
  static const char LOG[]     = "log";
  static const char ID[]      = "id";

  tokenize();
  #if SERIAL_DEBUG == 1
  Serial.print("CMD:"); Serial.print(action.c_str()); Serial.print(":"); Serial.println(value.c_str());
  #endif
  bool isSet = !value.empty();

  if (action == BC) {
    if (isSet) {
      parseHexColor(value.c_str() + 1, c);
      database->setBladeColor(c);
    }
    printLead(action.c_str());
    printHexColor(database->bladeColor()); Serial.print('\n');
  }
  else if (action == IC) {
    if (isSet) {
      parseHexColor(value.c_str() + 1, c);
      database->setImpactColor(c);
    }
    printLead(action.c_str());
    printHexColor(database->impactColor()); Serial.print('\n');
  }
  else if (action == PAL) {
    if (isSet) {
      int pal = atoi(value.c_str());
      database->setPalette(pal);
      CMDParser::bringPaletteCurrent();
    }
    printLead(action.c_str());
    Serial.println(database->paletteIndex());
  }
  else if (action == FONT) {
    if (isSet) {
      int f = atoi(value.c_str());
      database->setSoundFont(f);
      CMDParser::bringPaletteCurrent();
    }
    printLead(action.c_str());
    Serial.print(database->soundFont());
    Serial.print(" ");
    Serial.println(SFX::instance()->currentFontName());
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
  else if (action == AMPS) {
    printLead(action.c_str());
    Serial.println(Blade::blade().power());
  }
  else if (action == VOLTS) {
    printLead(action.c_str());
    Serial.println(Blade::blade().voltage());
  }
  else if (action == UTIL) {
    printLead(action.c_str());
    for(int i=0; i<NCHANNELS; ++i) {
      Serial.print(Blade::blade().util(i));
      Serial.print(' ');
    }
    Serial.print('\n');
  }
  else if (action == PWM) {
    printLead(action.c_str());
    for(int i=0; i<NCHANNELS; ++i) {
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
  }
  else if (action == LOG) {
    database->dumpLog();
  }
  else if (action == ID) {
    printLead(action.c_str());
    Serial.println(F(ID_STR));
  }
  else if (action == STATUS) {
    static const int DELAY = 20;  // don't saturate the serial line. Too much for SoftwareSerial.
    
    static const char* space = "-----------";
    delay(DELAY);
    Serial.println(space);
    printLead(ID);      Serial.println(F(ID_STR));
    delay(DELAY);
    printLead(PAL);     Serial.println(database->paletteIndex());
    delay(DELAY);
    printLead(FONT);    
    Serial.print(database->soundFont());
    Serial.print(" ");
    Serial.println(SFX::instance()->currentFontName());
    delay(DELAY);
    printLead(AUDIO);   Serial.println(database->soundOn());
    delay(DELAY);
    printLead(VOL);     Serial.println(database->volume());
    delay(DELAY);
    printLead(MOTION);  Serial.println(database->motion());
    delay(DELAY);
    printLead(IMPACT);  Serial.println(database->impact());
    delay(DELAY);
    printLead(AMPS);    Serial.println(Blade::blade().power());
    delay(DELAY);
    printLead(VOLTS);   Serial.println(Blade::blade().voltage());
    delay(DELAY);
    printLead(UTIL);    
    delay(DELAY);
    for(int i=0; i<NCHANNELS; ++i) {
      Serial.print(Blade::blade().util(i));
      Serial.print(' ');
    }
    Serial.print('\n');
    delay(DELAY);

    printLead(PWM);
    for(int i=0; i<NCHANNELS; ++i) {
      Serial.print(Blade::blade().pwmVal(i));
      Serial.print(' ');
    }
    Serial.print('\n');
    delay(DELAY);

    // font
    printLead(BC);      printHexColor(database->bladeColor()); Serial.print('\n');
    delay(DELAY);
    printLead(IC);      printHexColor(database->impactColor()); Serial.print('\n');
    Serial.println(space);
  }
  token.clear();
  return isSet;
}


void CMDParser::bringPaletteCurrent()
{
  const uint8_t* bladeColor  = database->bladeColor();
  uint8_t soundFont          = database->soundFont();
  SFX* sfx = SFX::instance();
  bool bladeOn               = sfx->bladeOn();
  
  if (bladeOn) {
    Blade::blade().setRGB(bladeColor);
  }
  sfx->setFont(soundFont);
}

