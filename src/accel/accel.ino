#include <Adafruit_LIS3DH.h>
#include <SPI.h>
#include <math.h>

#include "accelspeed.h"

//#define LOG_MODE
#define SOUND_TEST
#define PIN_ENABLE 2
#define TONE_PIN A2

#ifdef SOUND_TEST
AccelSpeed accelSpeed;
uint32_t lastMicros = 0;
uint32_t lastTone = 0;
#endif

Adafruit_LIS3DH accel(PIN_ENABLE);

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
        if (accel.begin(0)) {
            Serial.print("Accelerometer open on attempt: "); Serial.println(i);
            success = true;
            break;
        }   
    }

    if (!success) {
        Serial.println("Accelerometer ERROR.");
    }
    else {
        bool success = false;
        int n=0;
        for(; n<4; n++) {
            accel.setRange(LIS3DH_RANGE_4_G);
            accel.setDataRate(LIS3DH_DATARATE_100_HZ);
            delay(20);
            lis3dh_range_t range = accel.getRange();
            lis3dh_dataRate_t rate = accel.getDataRate();

            if (range == LIS3DH_RANGE_4_G && rate == LIS3DH_DATARATE_100_HZ) {
                success = true;
                break;
            }
            delay(100);
        }
        Serial.print("Accelerometer open. Configuration success=");
        Serial.print(success ? "true" : "false");
        Serial.print(" on attempt=");
        Serial.println(n);
    }

    #ifdef SOUND_TEST
    //tone(TONE_PIN, 440, 1000);
    lastMicros = micros();
    lastTone = millis();
    #endif
}


void loop()
{
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
}