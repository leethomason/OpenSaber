/*
#include <FastLED.h>

#define NUM_LEDS 4
CRGB leds[NUM_LEDS];

void setup() {
  delay(2000);
  FastLED.setBrightness(20);
  FastLED.addLeds<APA102, BGR>(leds, NUM_LEDS);
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);  // enable access to LEDs

  leds[0] = CRGB::Red;
  leds[1] = CRGB::Green;
  leds[2] = CRGB::Blue;
  leds[3] = CRGB::Yellow;
  FastLED.show(); 
}

void loop() {
}
*/

#include "dotstar.h"
#define NUM_LEDS 4
DotStar::RGB leds[NUM_LEDS];
DotStar dotstar(7);

void setup() 
{
  Serial.begin(19200);
  while(!Serial) {
    delay(100);
  }
  Serial.println("Hello DotStar!!");

  leds[0].set(0xff0000);
  leds[1].set(0x00ff00);
  leds[2].set(0x0000ff);
  leds[3].set(0x00ffff);

  leds[0].red = 0xff;

  delay(2000);
  
  dotstar.begin();
  dotstar.setBrightness(10);
  dotstar.attachLEDs(leds, NUM_LEDS);
  dotstar.display();
  dotstar.display();

  delay(2000);
}


void loop() 
{
  uint32_t t = millis();
  t = t / 1000;

  for(int i=0; i<NUM_LEDS; ++i) {
    leds[i].red   = ((i+t) & 1) ? 0xff : 0x10;
    leds[i].green = ((i+t) & 2) ? 0xff : 0;
    leds[i].blue  = ((i+t) & 4) ? 0xff : 0;
  }
  dotstar.display();
}
