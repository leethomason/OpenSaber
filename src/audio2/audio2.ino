#include <stdint.h>

uint32_t startTime = 0;

void setup() {
    Serial.begin(19200);
    while (!Serial) {
      delay(500);
    }
    Serial.println("Hello");
    pinMode(7, OUTPUT);
    digitalWrite(7, LOW);
    analogWriteResolution(16);
    startTime = micros();
}

void loop() {
    uint32_t time = micros();
    uint32_t delta = time - startTime;

    // 440Hz
    static const uint32_t SECOND =  1000000;
    static const uint32_t CYCLE = SECOND / 440;

    uint32_t subCycle = delta % CYCLE;

    float d = 6.283f * float(subCycle) / float(CYCLE);
    float v = sin(d);

    const float VOL = 0.2;
    float vPrime = v * VOL;

    int32_t v16 = int32_t(32768) + int32_t(vPrime * 32768.0f);

    analogWrite(A0, v16);
}

