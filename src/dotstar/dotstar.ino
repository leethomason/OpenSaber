#include <DotStar.h>
#define NUM_LEDS 4
RGB leds[NUM_LEDS];
DotStar dotstar;

void setup() 
{
    
  Serial.begin(19200);
  while(!Serial) {
    delay(100);
  }
  Serial.println("Hello DotStar!!");

  leds[0].set(0xff0000);
  #if NUM_LEDS > 1
    leds[1].set(0x00ff00);
  #endif
  #if NUM_LEDS > 2
    leds[2].set(0x0000ff);
  #endif
  #if NUM_LEDS > 3
    leds[3].set(0x00ffff);
  #endif

 // dotstar.beginSW(5, 4);
 dotstar.beginSPI(7);   // enable pin
 
  /*
  digitalWrite(dotstar.swClockPin(), LOW);
  digitalWrite(dotstar.swDataPin(), LOW);
  */
  dotstar.setBrightness(10);
  dotstar.attachLEDs(leds, NUM_LEDS);
  dotstar.display();
  delay(1000);
  dotstar.display();
 // dotstar.display();
}


void loop() 
{
  uint32_t t = millis();

  static const RGB rgb[7] = {
      RGB::RED,
      RGB::ORANGE,
      RGB::YELLOW,
      RGB::GREEN,
      RGB::BLUE,
      RGB::INDIGO,
      RGB::VIOLET
  };

  uint32_t index = (t/uint32_t(500)) % 7;
  if (leds[3] != rgb[index]) {
      //Serial.println(index);
      leds[3] = rgb[index];
  }

  dotstar.display();
  delay(10);
}
