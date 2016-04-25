#include <FastLED.h>

#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

void setup() {
  delay(200);
  //FastLED.setBrightness(4);
  FastLED.addLeds<APA102, BGR>(leds, NUM_LEDS);
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);  // enable access to LEDs
}

void loop() {
  leds[0] = CRGB::Red;
  FastLED.show(); 
  delay(400);
  
  leds[0] = CRGB::Green;
  FastLED.show(); 
  delay(400);

  leds[0] = CRGB::Blue;
  FastLED.show(); 
  delay(400);
}
