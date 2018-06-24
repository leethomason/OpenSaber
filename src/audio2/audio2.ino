#include <stdint.h>

uint32_t startTime = 0;
uint32_t lastTime = 0;

void setup() {
    /*
    Serial.begin(19200);
    while (!Serial) {
      delay(500);
    }
    Serial.println("Hello2");
    */
    //pinMode(5, OUTPUT);
    analogWriteResolution(10);
    startTime = micros();
    delay(100);

    analogWrite(A0, 1023);
    pinMode(7, OUTPUT);
    digitalWrite(7, HIGH);
}

void loop() {
    uint32_t t = millis();
    if (lastTime != t) {
        if (t&1)
            analogWrite(A0, 510);
        else
            analogWrite(A0, 490);
        lastTime = t;
    }
    /*
    uint32_t time = micros();
    uint32_t delta = time - startTime;

    //Serial.print("delta="); Serial.println(delta);

    // 440Hz
    static const uint32_t SECOND =  1000000UL;
    static const uint32_t CYCLE = SECOND / 440UL;

    uint32_t subCycle = delta % CYCLE;
    //Serial.print("subCycle="); Serial.println(subCycle);

    float d = 6.283f * float(subCycle) / float(CYCLE);
    float v = sin(d) + 1.0f;
    //Serial.print("v="); Serial.println(v);
    const float VOL = 0.5;
    float vPrime = v * VOL;

    int32_t v16 = int32_t(1024.0 * vPrime);
    //Serial.print("v16="); Serial.println(v16);

    analogWrite(A0, v16);
    */
    //while(true) {}
}

