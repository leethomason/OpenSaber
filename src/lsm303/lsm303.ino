#include <SPI.h>
#include <math.h>
#include "GrinlizLSM303.h"
#include "Grinliz_Util.h"

uint32_t lastMillis = 0;
int nRead = 0;

// -431 -505 -421
//  259   90  258

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
    delay(20);
    accel.setMagCalibration(-431, -505, -421, 259, 90, 258);
    accel.logStatus();
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

                Log.p("m=").p(magData.x).p(" ").p(magData.y).p(" ").p(magData.z).
                    p("  ").p(x).p(" ").p(y).p(" ").p(z).eol();
                Log.p("  min=").p(accel.minX).p(" ").p(accel.minY).p(" ").p(accel.minZ)
                   .p("  max=").p(accel.maxX).p(" ").p(accel.maxY).p(" ").p(accel.maxZ)
                   .p("  ave=").p((accel.maxX + accel.minX)/2).p(" ").p((accel.maxY + accel.minY)/2).p(" ").p((accel.maxZ + accel.minZ)/2)
                   .p(" heading=").p(heading).eol();
                break;
            }
        }

#if true
        float ax = data[0].ax;
        float ay = data[0].ay;
        float az = data[0].az;
//        Serial.print("a="); Serial.print(ax); Serial.print(" ");
//        Serial.print(ay); Serial.print(" ");
//        Serial.println(az);
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
