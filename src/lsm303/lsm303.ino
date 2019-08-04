#include <SPI.h>
#include <math.h>
#include "GrinlizLSM303.h"
#include "Grinliz_Util.h"

uint32_t lastMillis = 0;
int nRead = 0;

GrinlizLSM303 accel;

void setup() {
    Serial.begin(19200);
    while(!Serial) delay(200);
    Log.attachSerial(&Serial);

    Serial.println("Serial connected.");

    Serial.println("Starting Accel/Gyro in I2S mode.");
    if (!accel.begin()) {
        Serial.println("Accel error.");
        while(true) {}
    }
    Serial.println("Accel/Gyro success");
}


void loop()
{
    static bool printTime = false;

    GrinlizLSM303::Data data[8];
    GrinlizLSM303::RawData rawData[8];
    int n = accel.readInner(rawData, data, 8);
    nRead += n;

    if (n && printTime) {
        printTime = false;
#if true
        float ax = data[0].ax;
        float ay = data[0].ay;
        float az = data[0].az;
        Serial.print("a="); Serial.print(ax); Serial.print(" ");
        Serial.print(ay); Serial.print(" ");
        Serial.println(az);
#else
        Serial.print("a="); Serial.print(rawData[0].x); Serial.print(" ");
        Serial.print(rawData[0].y); Serial.print(" ");
        Serial.println(rawData[0].z);
#endif
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
