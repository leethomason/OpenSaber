#include <Arduino.h>

#include "fixed.h"

void print(const char* name, uint32_t start, uint32_t end, int r)
{
    Serial.print(name);
    Serial.print(" r=");
    Serial.print(r);
    Serial.print(" time=");
    Serial.println(end - start);
}

void setup() 
{
    Serial.begin(19200);
    while (!Serial) {}
    Serial.println("Hello test");

    {
        uint32_t start = micros();
        int r = SpeedTestFloat();
        uint32_t end = micros();
        print("float", start, end, r);
    }
    {
        uint32_t start = micros();
        int r = SpeedTestFixed16();
        uint32_t end = micros();
        print("fixed16", start, end, r);
    }
    {
        uint32_t start = micros();
        int r = SpeedTestFixed88();
        uint32_t end = micros();
        print("fixed88", start, end, r);
    }
    {
        uint32_t start = micros();
        int r = SpeedTestFPM();
        uint32_t end = micros();
        print("fpm", start, end, r);
    }
    {
        uint32_t start = micros();
        int r = SpeedTestFPM8();
        uint32_t end = micros();
        print("fpm8", start, end, r);
    }
}

void loop()
{

}