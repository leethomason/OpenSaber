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
    static bool active = false;

    Vec3<Fixed115> data;
    bool hasAccelData = accel.sampleAccel(&data);

    Vec3<int32_t> magData;
    bool hasMagData = accel.sampleMag(&magData);

    if (!active && hasMagData) {
        active = true;
        accel.log(10);
        hasMagData = false;
    }

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
            Log.p("Swing=").p(swing.speed()).p(" dot=").p(swing.dotOrigin()).p(" val=").v3(magData).p(" min=").v3(accel.getMagMin()).p(" max=").v3(accel.getMagMax()).eol();
            //Log.p("Swing=").p(swing.speed()).eol();
        }
    }

    if (swing.speed() < 0.4f) {
        accel.recalibrateMag();
    }  
}
