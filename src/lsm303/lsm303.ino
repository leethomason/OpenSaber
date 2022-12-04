#include "src/lsm303/grinliz_LSM303.h"
#include "src/util/Grinliz_Arduino_Util.h"
#include "swing.h"

GrinlizLSM303 accel(0);
Swing swing;

void setup() {
    Serial.begin(19200);
    while(!Serial) delay(200);
    Log.attachSerial(&Serial);

    Log.p("Serial connected.").eol();
    Log.p("Starting Accel/Gyro in I2C mode.").eol();
    delay(10);

    if (!accel.begin(0)) {
        Log.p("Accel error.").eol();
        while(true) {}
    }
    Log.p("Accel/Gyro success").eol();
    Swing::test();

    accel.recalibrateMag();
    accel.logMagStatus();
    accel.flushAccel(0);
}

void loop()
{
    static uint32_t lastAccelTime = 0;
    static uint32_t lastMagTime = 0;
    static uint32_t lastSwingTime = 0;

    Vec3<Fixed115> data;
    bool hasAccelData = accel.sampleAccel(&data);

    Vec3<int32_t> magData;
    bool hasMagData = accel.sampleMag(&magData);

    uint32_t ms = millis();
    if (hasAccelData && (ms - lastAccelTime > 200)) {
        lastAccelTime = ms;
        //Log.p("Accel=").v3(data).eol();
    }
    if (hasMagData && (ms - lastMagTime > 200)) {
        lastMagTime = ms;
        //Log.p("Mag  =").v3(magData).eol();
    }

    if(hasMagData) {
        swing.push(magData, accel.getMagMin(), accel.getMagMax());
        if (ms - lastSwingTime > 200) {
            lastSwingTime = ms;
            Log.p("Swing=").p(swing.speed()).p(" val=").v3(magData).p(" min=").v3(accel.getMagMin()).p(" max=").v3(accel.getMagMax()).eol();
        }
    }

    #if false
    {
        if (nMag) {
            static const int N = 40;
            float speed = swing.speed();
            int ticks = (int)(speed * 2);
            if (ticks < 0) ticks = 0;
            if (ticks >= N) ticks = N - 1;
            for(int i=0; i<ticks; i++) Serial.print("#");
            for(int i=ticks; i<N; i++) Serial.print("-");
            Serial.print(" speed="); Serial.print(speed);
            //Serial.print(" x="); Serial.print(x);
            //Serial.print(" y="); Serial.print(y);
            //Serial.print(" z="); Serial.print(z);
            Serial.println("");
        }
    }
    #endif 

#if false
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
