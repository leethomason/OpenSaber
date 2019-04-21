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

//Adafruit_LIS3DH accel(PIN_ENABLE);
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
        // Buggy buggy buggy SPI code.
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
}


void loop()
{
    GrinlizLIS3DH::AccelData data[8];
    int16_t div = 0;
    int n = accel.readRaw(data, 8, &div);
    nRead += n;

    uint32_t t = millis();
    if (t - lastMillis >= 1000) {
        Serial.print("Samples = ");
        Serial.println(nRead);
        lastMillis = t;
        nRead = 0;
    }

/*
    uint32_t t = millis();
    uint32_t t_micros = micros();

    accel.read();
    float ax = accel.x_g;
    float ay = accel.y_g;
    float az = accel.z_g;
    float g = sqrt(ax*ax + ay*ay + az*az);

#ifdef SOUND_TEST
    (void) t;
    (void) g;
    uint32_t delta = t_micros - lastMicros;
    lastMicros = t_micros;

    if (delta > 100) {
        accelSpeed.push(ax, ay, az, delta);
    }
    if (t - lastTone > 50) {
        lastTone = t;
        tone(TONE_PIN, 440 + int32_t(440 * accelSpeed.mix()));

        Serial.print("speed="); Serial.print(accelSpeed.speed());
        Serial.print(" mix="); Serial.print(accelSpeed.mix());
        Serial.print(" rising="); Serial.println(accelSpeed.risingEdge());
    }
#else
    Serial.print("accel x="); Serial.print(ax);
    Serial.print(" y="); Serial.print(ay);
    Serial.print(" z="); Serial.print(az);
    Serial.print(" g="); Serial.print(g);
    Serial.print(" t="); Serial.println(t);

#   ifdef LOG_MODE
    delay(10);
#   else
    delay(1000);
#   endif
#endif
*/
}