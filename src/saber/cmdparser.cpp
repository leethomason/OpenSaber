#include <Arduino.h>

#include "cmdparser.h"
#include "saberdb.h"
#include "blade.h"
#include "electrical.h"

#include "../Grinliz_Arduino_Util/Grinliz_Arduino_Util.h"

CMDParser::CMDParser(SaberDB* _db) {
  database = _db;
  token[0] = 0;
}


void CMDParser::push(int c) {
  for(int i=0; i<TOKEN_LEN-1; ++i) {
    if (token[i] == 0) {
      token[i] = c;
      token[i+1] = 0;
      return;
    }
  }
}

void CMDParser::tokenize(char** action, char** value)
{
  *action = 0;
  *value = 0;
  if (token[0] == 0) return;

  *action = token;
  char* p = strstr(token, " ");
  if (!p) return;
  *p = 0;
  *value = p+1;
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
  char *action = 0, *value = 0;
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
  static const char VOLTS[]   = "vcc";
  static const char UTIL[]    = "util";
  static const char PWM[]     = "pwm";
  static const char MOTION[]  = "mot";
  static const char IMPACT[]  = "imp";
  static const char STATUS[]  = "stat";
  static const char RESET[]   = "reset";
  static const char LOG[]     = "log";
  static const char ID[]      = "id";

  tokenize(&action, &value);
  #if SERIAL_DEBUG == 1
  Serial.print("CMD:"); Serial.print(action); Serial.print(":"); Serial.println(value);
  #endif
  bool isSet = (value != 0);

  if (strEqual(action, BC)) {
    if (isSet) {
      parseHexColor(value + 1, c);
      database->setBladeColor(c);
    }
    printLead(action);
    printHexColor(database->bladeColor()); Serial.print('\n');
  }
  else if (strEqual(action, IC)) {
    if (isSet) {
      parseHexColor(value + 1, c);
      database->setImpactColor(c);
    }
    printLead(action);
    printHexColor(database->impactColor()); Serial.print('\n');
  }
  else if (strEqual(action, PAL)) {
    if (isSet) {
      int pal = atoi(value);
      database->setPalette(pal);
    }
    printLead(action);
    Serial.println(database->paletteIndex());
  }
  else if (strEqual(action, FONT)) {
    if (isSet) {
      int f = atoi(value);
      database->setSoundFont(f);
    }
    printLead(action);
    Serial.println(database->soundFont());
  }
  else if (strEqual(action, AUDIO)) {
    if (isSet) {
      int onOff = atoi(value);
      database->setSoundOn(onOff ? true : false);
    }
    printLead(action);
    Serial.println(database->soundOn());
  }
  else if (strEqual(action, VOL)) {
    if (isSet) {
      int volume = constrain(atoi(value), 0, 204);
      database->setVolume(volume);
    }
    printLead(action);
    Serial.println(database->volume());
  }
  else if (strEqual(action, AMPS)) {
    printLead(action);
    Serial.println(Blade::blade().power());
  }
  else if (strEqual(action, VOLTS)) {
    printLead(action);
    Serial.println(Blade::blade().voltage());
  }
  else if (strEqual(action, UTIL)) {
    printLead(action);
    for(int i=0; i<NCHANNELS; ++i) {
      Serial.print(Blade::blade().util(i));
      Serial.print(' ');
    }
    Serial.print('\n');
  }
  else if (strEqual(action, PWM)) {
    printLead(action);
    for(int i=0; i<NCHANNELS; ++i) {
      Serial.print(Blade::blade().pwmVal(i));
      Serial.print(' ');
    }
    Serial.print('\n');
  }
  else if (strEqual(action, MOTION)) {
    if (isSet) {
      float v = atof(value);
      database->setMotion(v);
    }
    printLead(action);
    Serial.println(database->motion());
  }
  else if (strEqual(action, IMPACT)) {
    if (isSet) {
      float v = atof(value);
      database->setImpact(v);
    }
    printLead(action);
    Serial.println(database->impact());
  }
  else if (strEqual(action, RESET)) {
    database->writeDefaults();
  }
  else if (strEqual(action, LOG)) {
    database->dumpLog();
  }
  else if (strEqual(action, ID)) {
    printLead(action);
    Serial.println(F(ID_STR));
  }
  else if (strEqual(action, STATUS)) {
    static const int DELAY = 20;  // don't saturate the serial line. Too much for SoftwareSerial.
    
    static const char* space = "-----------";
    delay(DELAY);
    Serial.println(space);
    printLead(ID);      Serial.println(F(ID_STR));
    delay(DELAY);
    printLead(PAL);     Serial.println(database->paletteIndex());
    delay(DELAY);
    printLead(FONT);    Serial.println(database->soundFont());
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
  token[0] = 0;
  return isSet;
}


