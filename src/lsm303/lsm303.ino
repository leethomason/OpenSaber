#include <SPI.h>
#include <math.h>
#include "GrinlizLSM303.h"
#include "Grinliz_Util.h"
#include "fixed.h"
#include "swing.h"

uint32_t lastMillis = 0;
int nRead = 0;
GrinlizLSM303 accel;
Swing swing(10);

void setup() {
    Serial.begin(19200);
    while(!Serial) delay(200);
    Log.attachSerial(&Serial);

    Serial.println("Serial connected.");

    Serial.println("Starting Accel/Gyro in I2C mode.");
    if (!accel.begin()) {
        Serial.println("Accel error.");
        while(true) {}
    }
    Serial.println("Accel/Gyro success");
    accel.logMagStatus();

    TestFixed();
}

void loop()
{
    static bool printTime = false;

    GrinlizLSM303::Data data[8];
    GrinlizLSM303::RawData rawData[8];
    int n = accel.readInner(rawData, data, 8);
    nRead += n;

    float x, y, z;
    GrinlizLSM303::RawData magData;
    int nMag = accel.readMag(&magData, &x, &y, &z);

    #if true
    {
        if (nMag) {
            swing.push(millis(), x, y, z);
            float speed = swing.speed();
            int ticks = (int)(speed / 2);
            if (ticks < 0) ticks = 0;
            if (ticks >= 20) ticks = 19;
            for(int i=0; i<ticks; i++) Serial.print("#");
            for(int i=ticks; i<20; i++) Serial.print("-");
            Serial.println("");
        }
    }
    #endif 

    if (printTime) {
        printTime = false;

#if false
        if (nMag) {
            float heading = atan2(y, x) * 180.0f / 3.14159f;
            if (heading < 0) heading += 360;

            int x0, y0, z0, x1, y1, z1;
            accel.getMagCalibration(&x0, &y0, &z0, &x1, &y1, &z1);
            
            Log.v(x, y, z)
                .v(x0, x1, "[]")
                .v(y0, y1, "[]")
                .v(z0, z1, "[]")
                .p(" heading=").p(heading)
                .eol();
        }
#endif
#if false
        float ax = data[0].ax;
        float ay = data[0].ay;
        float az = data[0].az;
        Serial.print("a="); Serial.print(ax); Serial.print(" ");
        Serial.print(ay); Serial.print(" ");
        Serial.println(az);
#endif
    }

#if false
    uint32_t t = millis();
    if (t - lastMillis >= 1000) {
        Serial.print("Samples = ");
        Serial.println(nRead);
        lastMillis = t;
        nRead = 0;
        printTime = true;
    }
#endif
}
