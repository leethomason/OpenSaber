#include <stdint.h>

#include "shiftedreg.h"

// Weird pin assignment from the pins available on Itsy V4c
static constexpr int clockPin = 7;
static constexpr int latchPin = 2;
static constexpr int dataPin = 18; // A4

static constexpr uint32_t DELAY = 500;
#define ORDER MSBFIRST

ShiftedReg shiftedReg;

void setup()
{
    // set pins to output because they are addressed in the main loop
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

    shiftedReg.begin(latchPin, clockPin, dataPin);
}

void loop()
{
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

    shiftedReg.set(0xffff);
    delay(DELAY);
    for(int i=0; i<16; i++) {
        shiftedReg.set(1 << i);
        delay(DELAY);
    }    
    /*
    for(int i=0; i<8; i++) {
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, LSBFIRST, (1 << i));
        digitalWrite(latchPin, HIGH);
        delay(DELAY);
    }
    */

}