#include "./src/util/Grinliz_Util.h"
#include "./src/lsm6d/grinliz_lsm6d.h"

GrinlizLSM6D accel(A5);

void setup() {
    Serial.begin(19200);
    while(!Serial) delay(200);
    Log.attachSerial(&Serial);

    Log.p("Serial connected.").eol();
    accel.begin(&SPI, GrinlizLSM6D::Freq::HZ_12);

    Log.p("Accel begin() complete.").eol();
    uint8_t whoAmI = accel.whoAmI();
    Log.p("WhoAmI=").p(whoAmI, HEX).p(" (expected=").p(GrinlizLSM6D::WHOAMI, HEX).p(")").eol();

    accel.test();
}

void loop()
{
    Vec3<Fixed115> a;
    Vec3<int32_t> gyro;
    if (accel.sampleAccelGyro(&a, &gyro)) {
        Log.p("Accel: ").v3(a).p(" Gyro: ").v3(gyro).eol();
    }
}
