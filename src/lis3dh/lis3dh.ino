#include <SPI.h>

#include "./src/util/Grinliz_Util.h"
#include "./src/grinliz_lis3dh/grinliz_lis3dh.h"

GrinlizLIS3DH accel(2);
int hold = 0;

void setup() {
    Serial.begin(19200);
    while(!Serial) delay(200);
    Log.attachSerial(&Serial);

    Log.p("Serial connected.").eol();
    //Log.p("Starting Accel/Gyro in I2C mode.").eol();
    //delay(10);
    accel.begin(&SPI, LIS3DH_DATARATE_1_HZ);
    accel.flushAccel(0);
}

void loop()
{
    Fixed115 x, y, z;
    while (accel.sampleAccel(&x, &y, &z)) {
        Log.p("x=").p(x).p(" y=").p(y).p(" z=").p(z).eol();
    }
}
