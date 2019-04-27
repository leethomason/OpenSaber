#include <Adafruit_LIS3DH.h>
#include <SPI.h>
#include <math.h>

#include "accelspeed.h"
#include "GrinlizLIS3DH.h"

//#define LOG_MODE
//#define SOUND_TEST
#define PIN_ENABLE 2
#define TONE_PIN A2

#ifdef SOUND_TEST
AccelSpeed accelSpeed;
uint32_t lastMicros = 0;
uint32_t lastTone = 0;
#endif
uint32_t lastMillis = 0;
int nRead = 0;

GrinlizLIS3DH accel(PIN_ENABLE, LIS3DH_RANGE_8_G, LIS3DH_DATARATE_100_HZ);

void setup() {
    Serial.begin(115200);
    while(!Serial) delay(200);
    Serial.println("Serial connected.");

    Serial.println("LIS3DH Accelerometer starting: SPI mode");
    digitalWrite(PIN_ENABLE, HIGH);
    pinMode(PIN_ENABLE, OUTPUT);
    SPI.begin();

    bool success = false;
    for(int i=0; i<5; ++i) {
        delay(20);
        if (accel.begin()) {
            Serial.print("Accelerometer open on attempt: "); Serial.println(i);
            success = true;
            break;
        }   
    }

    #ifdef SOUND_TEST
    //tone(TONE_PIN, 440, 1000);
    lastMicros = micros();
    lastTone = millis();
    #endif
    Serial.println("Go!");
    //lastMillis = millis();

    GrinlizLIS3DH::AccelData data[8];
    int16_t div = 0;
    int n = accel.readRaw(data, 8, &div);
    for(int i=0; i<n; ++i) {
        float ax = (float)data[i].x / div;
        float ay = (float)data[i].y / div;
        float az = (float)data[i].z / div;
        Serial.print("a="); Serial.print(ax); Serial.print(" ");
        Serial.print(ay); Serial.print(" ");
        Serial.println(az);
    }
}


void loop()
{
    static bool printTime = false;

    GrinlizLIS3DH::AccelData data[8];
    int16_t div = 0;
    int n = accel.readRaw(data, 8, &div);
    nRead += n;

    if (n && printTime) {
        printTime = false;
        float ax = (float)data[0].x / div;
        float ay = (float)data[0].y / div;
        float az = (float)data[0].z / div;
        Serial.print("a="); Serial.print(ax); Serial.print(" ");
        Serial.print(ay); Serial.print(" ");
        Serial.println(az);
    }

    uint32_t t = millis();
    if (t - lastMillis >= 1000) {
        Serial.print("Samples = ");
        Serial.println(nRead);
        lastMillis = t;
        nRead = 0;
        printTime = true;
    }
}