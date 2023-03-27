#include "DotStar.h"
#define NUM_LEDS 2

osbr::RGB leds[NUM_LEDS];
DotStar dotstar;

void setup()
{
	Serial.begin(19200);
	while (!Serial)
	{
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
	for (int i = 4; i < NUM_LEDS; ++i)
	{
		leds[i].set(0x446688);
	}

	dotstar.beginSPI(A3);
	dotstar.display(leds, NUM_LEDS, 8);
}

void loop()
{
	uint32_t f = millis() % 1000;
	uint32_t ch = (millis() / 1000) % 3;

	for (int i = 0; i < NUM_LEDS; i++) {
		leds[i].set(0);
		leds[i][ch] = f * 255 / 1000;
	}
	dotstar.display(leds, NUM_LEDS, 8);
}
