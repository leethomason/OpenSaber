#include <stdint.h>

#include "shiftedreg.h"
#include "sevenseg.h"

// Weird pin assignment from the pins available on Itsy V4c
static constexpr int clockPin = 7;
static constexpr int latchPin = 2;
static constexpr int dataPin = 18; // A4

static constexpr uint32_t DELAY = 500;
#define ORDER MSBFIRST

ShiftedReg shiftedReg;
SevenSeg sevenSeg;

void setup()
{
    // set pins to output because they are addressed in the main loop
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

    shiftedReg.begin(latchPin, clockPin, dataPin);
    sevenSeg.begin(latchPin, clockPin, dataPin);
}

void loop()
{
    uint32_t now = millis();
    switch((now / 1000) % 4) {
        case 0: sevenSeg.set("8888"); break;
        case 1: sevenSeg.set("2468"); break;    
        case 2: sevenSeg.set("ABCD"); break;    
        default: sevenSeg.set("efgh"); break;
    }
    uint8_t dp = (now / 1000) % 5;
    sevenSeg.setDP(dp);

    sevenSeg.loop(millis());

    /*
    static constexpr uint16_t C1 = (1 << 8);
    static constexpr uint16_t C2 = (1 << 3);
    static constexpr uint16_t C3 = (1 << 11);
    static constexpr uint16_t C4 = (1 << 13);

    uint16_t mask = C2 | C3 | C4;
    shiftedReg.set(mask | 512);
    */
    /*
    shiftedReg.set(0xffff);
    delay(DELAY);
    for(int i=0; i<16; i++) {
        shiftedReg.set(1 << i);
        delay(DELAY);
    }    
    */

    /*
    digitalWrite(latchPin, LOW);
    // Blink the low bit leds
    //shiftOut(dataPin, clockPin, ORDER, 2);
    //shiftOut(dataPin, clockPin, ORDER, 2);
    // Blink the high bit leds
    shiftOut(dataPin, clockPin, ORDER, 128);
    shiftOut(dataPin, clockPin, ORDER, 128);
    digitalWrite(latchPin, HIGH);
    delay(DELAY);
    */
    
    /*
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, ORDER, 0xff);
    shiftOut(dataPin, clockPin, ORDER, 0xff);
    digitalWrite(latchPin, HIGH);
    delay(DELAY);
    */

    /*
    for(int i=0; i<8; i++) {
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, LSBFIRST, (1 << i));
        digitalWrite(latchPin, HIGH);
        delay(DELAY);
    }
    */

}