#include <stdint.h>
#include <Grinliz_Arduino_Util.h>

static const int PIN_R = 8;
static const int PIN_G = 6;
static const int PIN_B = 5;

#define PIN_LED_0 A0
#define PIN_LED_1 A1
#define PIN_LED_2 A2
#define PIN_LED_3 A3

CStr<40> serialBuffer;

//#define DEBUG_DEEP

template< typename T > void Swap(T& a, T& b) {
  T t = a;
  a = b;
  b = t;
}

void setup() 
{
  Serial.begin(19200);

  digitalWrite(PIN_R, HIGH);
  digitalWrite(PIN_G, HIGH);
  digitalWrite(PIN_B, HIGH);
  
  digitalWrite(PIN_LED_0, LOW);
  digitalWrite(PIN_LED_1, HIGH);
  digitalWrite(PIN_LED_2, HIGH);
  digitalWrite(PIN_LED_3, HIGH);

  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);

  pinMode(PIN_LED_0, OUTPUT);
  pinMode(PIN_LED_1, OUTPUT);
  pinMode(PIN_LED_2, OUTPUT);
  pinMode(PIN_LED_3, OUTPUT);

  Serial.println("setup()");
  multiplex();
}

struct RGB {
  uint8_t r, g, b;
};

/*
static const RGB rgb[4] = {
  { 255, 0, 0 },
  { 0, 255, 0 },
  { 0, 0, 255 },
  { 255, 0, 255 }
};
*/

RGB rgb[4] = {
  {255, 0, 0},
  {160, 0, 80},
  {80, 0, 160},
  {0, 0, 255}
};

/*
static const RGB rgb[4] = {
  {255, 255, 255},
  {130, 130, 130},
  {60, 60, 60},
  {30, 30, 30}
};
*/

struct Channel {
  int c;
  uint32_t time;
};

void loop() {
#ifndef DEBUG_DEEP
  multiplex();
#endif
}


void multiplex() 
{
  for(int i=0; i<4; ++i) {
#ifdef DEBUG_DEEP
    Serial.print("--- "); Serial.println(i);
#endif
    // Turn off previous
    int prev = (i+7)%4;
    digitalWrite(PIN_LED_0 + prev, HIGH);

    // set call
    digitalWrite(PIN_R, rgb[i].r ? HIGH : LOW);
    digitalWrite(PIN_G, rgb[i].g ? HIGH : LOW);
    digitalWrite(PIN_B, rgb[i].b ? HIGH : LOW);

    // turn on
    digitalWrite(PIN_LED_0 + i, LOW);

    static const uint32_t MULT = 5;
    static const uint32_t DELAY = MULT * 255;

    Channel channel[3] = {
      { 0, rgb[i].r * MULT },
      { 1, rgb[i].g * MULT },
      { 2, rgb[i].b * MULT }
    };

    if (channel[0].time > channel[1].time) Swap(channel[0], channel[1]);
    if (channel[1].time > channel[2].time) Swap(channel[1], channel[2]);
    if (channel[0].time > channel[1].time) Swap(channel[0], channel[1]);

#ifdef DEBUG_DEEP
    Serial.print(channel[0].c); Serial.print(" "); Serial.println(channel[0].time);
    Serial.print(channel[1].c); Serial.print(" "); Serial.println(channel[1].time);
    Serial.print(channel[2].c); Serial.print(" "); Serial.println(channel[2].time);
#endif

    int idx = 0;

    // skip zeros.
    while(idx < 3 && channel[idx].time == 0) ++idx;

    uint32_t accum = 0;
    while(idx < 3) {
      if (channel[idx].time == DELAY) {
        // full on!
        break;
      }
      uint32_t t = channel[idx].time;
      delayMicroseconds(t - accum);
#ifdef DEBUG_DEEP
      Serial.print("delay us "); Serial.println(t - accum);
#endif

      accum = t;
      while(idx < 3 && channel[idx].time == t) {
        digitalWrite(PIN_R + channel[idx].c, LOW);
#ifdef DEBUG_DEEP
        Serial.print("channel "); Serial.print(channel[idx].c); Serial.println(" off");
#endif
        idx++;
      }
    }
#ifdef DEBUG_DEEP
    Serial.print("accum "); Serial.println(accum);
    Serial.print("final delay us "); Serial.println(DELAY - accum);
#endif
    delayMicroseconds(DELAY - accum);
  }
}

void serialEvent()
{
/*    while (Serial.available()) {
    int c = Serial.read();
    if (c == '\n') {
      int n = serialBuffer.size() / 2;
      n = constrain(n, 0, 12);
      if (n) {
        parseNHex(serialBufer.c_str(), &rgb[0].r, 
      }
    }
    else {
      serialBuffer.append(c);
    }*/
}

