#include <stdint.h>

uint32_t startTime = 0;
uint32_t lastTime = 0;

static const uint32_t SECOND =  1000000UL;
static const uint32_t CYCLE = SECOND / 440UL;
const float VOLUME = 0.4f;
uint16_t buffer[CYCLE];

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

    for(int x=0; x<CYCLE; ++x) {
        float d = 6.283f * float(x) / float(CYCLE);
        float v = sin(d) * VOLUME + 1.0f;

        buffer[x] = uint32_t(512.0 * v);
    }
}

void loop() {
    /*
    uint32_t t = millis();
    if (lastTime != t) {
        if (t&1)
            analogWrite(A0, 510);
        else
            analogWrite(A0, 490);
        lastTime = t;
    }*/
   
    uint32_t time = micros();
    uint32_t delta = time - startTime;
    uint32_t subCycle = delta % CYCLE;
    analogWrite(A0, buffer[subCycle]);
}

