#include <Grinliz_Arduino_Util.h>
#include <Button.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include "widget.h"

#define SERIAL_DEBUG

//              rs, enable, d0, d1, d2, d3
// ON LCD:        4,  6,  11, 12, 13, 14
LiquidCrystal lcd(10, 11, A1, A0, 13, 12);

CStr<24> serialBuffer;
bool connectionOK = true;
uint32_t lastConnect = 0;

enum {
  WELCOME_SCREEN,
  CONFIG_SCREEN,
  PALETTE_SCREEN,
  SENSOR_SCREEN,
  NUM_SCREENS
};

int currentScreen = WELCOME_SCREEN;
int focus = 0;

TextWidget welcomeText(         WELCOME_SCREEN, 0, 0, "Grinning Lizard ");
TextWidget companyText(         WELCOME_SCREEN, 0, 1, "   Open Saber"   );

TextNumWidget audio(            CONFIG_SCREEN,  0,  0, "audio:", 1, "aud", Widget::TYPE_INT);
TextNumWidget volume(           CONFIG_SCREEN,  9,  0, "vol:", 3, "vol", Widget::TYPE_INT);
TextNumWidget motion(           CONFIG_SCREEN,  0,  1, "m:",   4, "mot", Widget::TYPE_FLOAT);
TextNumWidget impact(           CONFIG_SCREEN,  10, 1, "i:",   4, "imp", Widget::TYPE_FLOAT);

TextNumWidget   volts(          SENSOR_SCREEN,  8,  0, "vcc:", 4, "vcc", Widget::TYPE_FLOAT);
PercentX3Widget utilization(    SENSOR_SCREEN,  0,  1, "util:", "util");

TextNumWidget palette(          PALETTE_SCREEN, 0,  0, "P:",   1, "pal", Widget::TYPE_INT);
TextNumWidget power(            PALETTE_SCREEN, 9,  0, "mA:",  4, "amp", Widget::TYPE_INT);
TextNumWidget bladeColorRed(    PALETTE_SCREEN, 0,  1, "#",    2, 0,     Widget::TYPE_HEX);
TextNumWidget bladeColorGreen(  PALETTE_SCREEN, 3,  1, 0,      2, 0,     Widget::TYPE_HEX);
TextNumWidget bladeColorBlue(   PALETTE_SCREEN, 5,  1, 0,      2, 0,     Widget::TYPE_HEX);
TextNumWidget impactColorRed(   PALETTE_SCREEN, 9,  1, "#",    2, 0,     Widget::TYPE_HEX);
TextNumWidget impactColorGreen( PALETTE_SCREEN, 12, 1, 0,      2, 0,     Widget::TYPE_HEX);
TextNumWidget impactColorBlue(  PALETTE_SCREEN, 14, 1, 0,      2, 0,     Widget::TYPE_HEX);

Widget* widgetArr[] = {
  &welcomeText,
  &companyText,

  &audio,
  &volume,
  &motion,
  &impact,

  &volts,
  &utilization,

  &palette,
  &power,
  &bladeColorRed, &bladeColorGreen, &bladeColorBlue,
  &impactColorRed, &impactColorGreen, &impactColorBlue,
};

static const int NUM_WIDGETS = sizeof(widgetArr) / sizeof(widgetArr[0]);

SoftwareSerial softSerial(12, 13);

Stream* debugSerial = 0;
Stream* saberSerial = 0;

Button buttonUp(3);
Button buttonDown(4);
Button buttonLeft(5);
Button buttonRight(6);
Button buttonInc(A5);
Button buttonDec(A4);

void setup() {
  lcd.begin(16, 2);
  lcd.print("Initializing...");

  Serial.begin(19200);
  softSerial.begin(19200);

  debugSerial = &Serial;
  saberSerial = &softSerial;

  lcd.setCursor(7, 0);
  lcd.noAutoscroll();

  focus = 0;
  drawScreen(currentScreen);

  delay(100);
  if (debugSerial) {
    debugSerial->println("controller setup");
  }
  lastConnect = millis();
}

void connect() {
  uint32_t delta = millis() - lastConnect;
  if (delta > 400) {
    if (debugSerial) {
      debugSerial->println("trying to connect...");
    }
    saberSerial->print("stat\n");
    lastConnect = millis();
  }
}

uint32_t lastTime = 0;

void loop() {
  buttonUp.process();
  buttonDown.process();
  buttonLeft.process();
  buttonRight.process();
  buttonInc.process();
  buttonDec.process();

  bool processed = flushSerial();
  if (processed) {
    drawScreen(currentScreen);
  }

  if (!connectionOK) {
    connect();
  }
  if (!connectionOK) {
    return;
  }

  if (buttonDown.press()) {
    debugSerial->println("downPress");
    focusDelta(1);
  }
  if (buttonUp.press()) {
    debugSerial->println("upPress");
    focusDelta(-1);
  }
}

bool flushSerial() {
  bool processed = false;
  uint8_t color[3];

  while (saberSerial->available()) {
    int c = saberSerial->read();
    if (debugSerial) {
      debugSerial->write(c);
    }
    if (c == '\n') {
      static const int PREFIX_LEN = 3;
      static const char* PREFIX = "[-]";
      if (memcmp(serialBuffer.c_str(), PREFIX, PREFIX_LEN) == 0) {
        if (!connectionOK) {
          connectionOK = true;
        }
        const char* cmd = serialBuffer.c_str() + PREFIX_LEN;

        for (int i = 0; i < NUM_WIDGETS; ++i) {
          widgetArr[i]->handleInput(cmd);
        }
        if (memcmp(cmd, "bc #", 4) == 0) {
          parseNHex(cmd + 4, color, 3);
          if (debugSerial) {
            debugSerial->print("bc ");
            debugSerial->print(color[0], HEX);
            debugSerial->print(" ");
            debugSerial->print(color[1], HEX);
            debugSerial->print(" ");
            debugSerial->println(color[2], HEX);
          }
          bladeColorRed.setIVal(color[0]);
          bladeColorGreen.setIVal(color[1]);
          bladeColorBlue.setIVal(color[2]);
        }
        if (memcmp(cmd, "ic #", 4) == 0) {
          parseNHex(cmd + 4, color, 3);
          if (debugSerial) {
            debugSerial->print("ic ");
            debugSerial->print(color[0], HEX);
            debugSerial->print(" ");
            debugSerial->print(color[1], HEX);
            debugSerial->print(" ");
            debugSerial->println(color[2], HEX);
          }
          impactColorRed.setIVal(color[0]);
          impactColorGreen.setIVal(color[1]);
          impactColorBlue.setIVal(color[2]);
        }
      }
      serialBuffer.clear();
      processed = true;
    }
    else {
      serialBuffer.append(c);
    }
  }
  return processed;
}

int findFocusWidget(int y) {
  for (int i = 0; i < NUM_WIDGETS; ++i) {
    Widget* it = widgetArr[i];
    if (    it->screen == currentScreen 
         && it->canFocus()
         && ((y < 0) || (it->y == y)))
    {
      return i;
    }
  }
  return 0;
}

void changeScreen(int delta) {
  currentScreen = (currentScreen + NUM_SCREENS + delta) % NUM_SCREENS;

  if (delta > 0)
    focus = findFocusWidget(0);
  else if (delta < 0)
    focus = findFocusWidget(1);
    
  if (!focus && currentScreen)
    focus = findFocusWidget(-1);

  if (debugSerial) {
    debugSerial->print("changeScreen screen "); debugSerial->println(currentScreen);
  }
  drawScreen(currentScreen);
}

void focusDelta(int dy) {
  if (debugSerial) {
    debugSerial->print("focusDelta "); debugSerial->println(dy);
  }
  int x = 0, y = 0;
  if (widgetArr[focus]->canFocus()) {
    widgetArr[focus]->getFocus(&x, &y);
    int nextY = y + dy;
    if (inRange(nextY, 0, 1)) {
      focus = findFocusWidget(nextY);
      if (focus >= 0) {
        drawScreen(currentScreen);
        return;
      }
    }
  }
  changeScreen(dy);
}

void drawScreen(int screen) {
  lcd.clear();

  for (int i = 0; i < NUM_WIDGETS; ++i) {
    Widget* w = widgetArr[i];
    if (w->screen == screen) {
      w->render(lcd);
    }
  }

  if (focus >= 0) {
    Widget* w = widgetArr[focus];
    if (w->screen == currentScreen && w->canFocus()) {
      int x = 0, y = 0;
      w->getFocus(&x, &y);
      lcd.setCursor(x, y);
      lcd.cursor();
    }
    else {
      lcd.noCursor();
    }
  }
}

void valueChange(int delta) {
  if (focus < 0) return;
  char cStr[7] = {0};
  uint8_t color[3] = {0};

  Widget* w = widgetArr[focus];
  if (w->screen == currentScreen && w->canFocus()) {
    w->valueChange(delta);
    if (w == &bladeColorRed || w == &bladeColorGreen || w == &bladeColorBlue) {
      saberSerial->print("bc #");
      color[0] = bladeColorRed.getIVal();
      color[1] = bladeColorGreen.getIVal();
      color[2] = bladeColorBlue.getIVal();
      writeNHex(cStr, color, 3);
      debugSerial->println(cStr);
      saberSerial->println(cStr);
    }
    else if (w == &impactColorRed || w == &impactColorGreen || w == &impactColorBlue) {
      saberSerial->print("ic #");
      color[0] = impactColorRed.getIVal();
      color[1] = impactColorGreen.getIVal();
      color[2] = impactColorBlue.getIVal();
      writeNHex(cStr, color, 3);
      debugSerial->println(cStr);
      saberSerial->println(cStr);
    }
  }
  drawScreen(currentScreen);
}

