#include <SPI.h>
#include <math.h>
#include "GrinlizLSM303.h"
#include "Grinliz_Util.h"
#include "fixed.h"
#include "swing.h"

uint32_t lastMillis = 0;
int nRead = 0;
int nMagRead = 0;
GrinlizLSM303 accel;
Swing swing;

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
    Swing::test();
}

void loop()
{
    static bool printTime = false;

    Vec3<float> data[8];
    Vec3<int32_t> rawData[8];
    int n = accel.readInner(rawData, data, 8);
    nRead += n;

    Vec3<float> fMagData;
    Vec3<int32_t> magData;
    int nMag = accel.readMag(&magData, &fMagData);
    if(nMag) {
        swing.push(millis(), magData, accel.getMagMin(), accel.getMagMax());
        ++nMagRead;
    }

    #if false
    {
        if (nMag) {
            static const int N = 40;
            swing.push(millis(), x, y, z);
            float speed = swing.speed();
            int ticks = (int)(speed * 2);
            if (ticks < 0) ticks = 0;
            if (ticks >= N) ticks = N - 1;
            for(int i=0; i<ticks; i++) Serial.print("#");
            for(int i=ticks; i<N; i++) Serial.print("-");
            Serial.print(" speed="); Serial.print(speed);
            Serial.print(" x="); Serial.print(x);
            Serial.print(" y="); Serial.print(y);
            Serial.print(" z="); Serial.print(z);
            Serial.println("");
        }
    }
    #endif 

    if (printTime && nMag) {
        printTime = false;

#if true
    {
        float heading = atan2(fMagData.y, fMagData.x) * 180.0f / 3.14159f;
        if (heading < 0) heading += 360;

        Vec3<int32_t> mMin = accel.getMagMin();
        Vec3<int32_t> mMax = accel.getMagMax();
        
        Log.v3(magData.x, magData.y, magData.z)
            .v2(mMin.x, mMax.x, "[]")
            .v2(mMin.y, mMax.y, "[]")
            .v2(mMin.z, mMax.z, "[]")
            .p(" heading=").p(heading)
            .p(" speed=").p(swing.speed())
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

    uint32_t t = millis();
    if (t - lastMillis >= 1000) {
        //Serial.print("Samples = ");
        //Serial.println(nRead);
        Serial.print("MagSamples=");
        Serial.println(nMagRead);
        lastMillis = t;
        nRead = 0;
        nMagRead = 0;
        printTime = true;
    }
}
