#include <FastLED.h>

#define NUM_LEDS 4
CRGB leds[NUM_LEDS];

void setup() {
  delay(200);
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
