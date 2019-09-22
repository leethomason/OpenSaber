#include <SPI.h>
#include <math.h>
#include "GrinlizLSM303.h"
#include "Grinliz_Util.h"
#include "fixed.h"

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
    //accel.setMagCalibration(-431, -505, -421, 259, 90, 258);
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
    accel.readMag(0, 0, 0, 0);

    if (n && printTime) {
        printTime = false;

        GrinlizLSM303::RawData magData;
        float x, y, z;
        while(true) {
            if (accel.readMag(&magData, &x, &y, &z)) {
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
                break;
            }
        }

        //float ax = data[0].ax;
        //float ay = data[0].ay;
        //float az = data[0].az;
        //Serial.print("a="); Serial.print(ax); Serial.print(" ");
        //Serial.print(ay); Serial.print(" ");
        //Serial.println(az);
    }

    uint32_t t = millis();
    if (t - lastMillis >= 1000) {
        //Serial.print("Samples = ");
        //Serial.println(nRead);
        lastMillis = t;
        nRead = 0;
        printTime = true;
    }
}
