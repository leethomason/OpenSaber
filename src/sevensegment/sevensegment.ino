#include "ShiftedSevenSeg.h"

ShiftedSevenSegment shifted7;

static const uint8_t LATCH = 0;
static const uint8_t CLOCK = 0;
static const uint8_t DATA = 0;

void setup() {
  Serial.begin(19200);
  while(!Serial) {
    delay(100);
  }
  
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);

  shifted7.attachDigit(0, 0);
  shifted7.attachDigit(1, 1);
  shifted7.attachDigit(2, 2);
  shifted7.attachDigit(3, 3);

  shifted7.attachSegment(0, 6);
  shifted7.attachSegment(1, 7);
  shifted7.attachSegment(2, 8);
  shifted7.attachSegment(3, 9);
  shifted7.attachSegment(4, 10);
  shifted7.attachSegment(5, 11);
  shifted7.attachSegment(6, 12);
  shifted7.attachSegment(7, 13);

  shifted7.set("1234");

  Serial.println("shifted7 setup complete.");
}

void loop() {
  uint8_t a=0, b=0;
  shifted7.loop(micros(), &a, &b);

  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, a);
  shiftOut(DATA, CLOCK, MSBFIRST, b);
  digitalWrite(LATCH, HIGH);
}
