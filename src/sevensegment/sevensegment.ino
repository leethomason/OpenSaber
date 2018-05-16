#include "ShiftedSevenSeg.h"

ShiftedSevenSegment shifted7;

static const uint8_t LATCH = 28;
static const uint8_t CLOCK = 29;
static const uint8_t DATA  = 27;

void setup() {
  Serial.begin(19200);
  while(!Serial) {
    delay(100);
  }
  Serial.println("Rock and roll.");
  
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);

  shifted7.attachDigit(0, 10);
  shifted7.attachDigit(1, 4);
  shifted7.attachDigit(2, 13);
  shifted7.attachDigit(3, 15);

  shifted7.attachSegment(0, 6);   // a
  shifted7.attachSegment(1, 5);  // b
  shifted7.attachSegment(2, 12);  // c
  shifted7.attachSegment(3, 2);  // d
  shifted7.attachSegment(4, 11);   // e
  shifted7.attachSegment(5, 3);  // f
  shifted7.attachSegment(6, 1);   // g
  shifted7.attachSegment(7, 14);  // h-dp

  //shifted7.set("1.2.3.4.");
  shifted7.set("523.7");

  Serial.println("shifted7 setup complete.");
}

void loop() {
  uint8_t a=0, b=0;
  shifted7.loop(micros(), &a, &b);

  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, b);
  shiftOut(DATA, CLOCK, MSBFIRST, a);
  digitalWrite(LATCH, HIGH);

  /*
  static const int NUM_PATTERN = 7; 
  int n = (millis() / 2000) % NUM_PATTERN;
  const char* PAT[NUM_PATTERN] = {
    "1234",
    "5678",
    "9-0",
    "8.8.8.8",
    "1.2.bc",
    "  8.9",
    "0. 1.b"
  };
  shifted7.set(PAT[n]);
  */
/*
  const char* TEST = "0123456789 -abcdefghijklmnopqrstuvwxyz"; // 26 + 10 + 2 = 38
  int n = (millis() / 1000);
  char str[5];
  str[0] = TEST[(n+0)%38];
  str[1] = TEST[(n+1)%38];
  str[2] = TEST[(n+2)%38];
  str[3] = TEST[(n+3)%38];
  str[4] = 0;
  shifted7.set(str);
  */
 }
  
