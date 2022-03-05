#include "./src/util/Grinliz_Util.h"
#include "./src/lsm6d/grinliz_lsm6d.h"

GrinlizLSM6D accel(A5);

void setup() {
    Serial.begin(19200);
    while(!Serial) delay(200);
    Log.attachSerial(&Serial);

    Log.p("Serial connected.").eol();
    accel.begin(&SPI);
    Log.p("Accel begin() complete.").eol();
    uint8_t whoAmI = accel.whoAmI();
    Log.p("WhoAmI=").p(whoAmI, HEX).p(" (expected=").p(GrinlizLSM6D::WHOAMI, HEX).p(")").eol();
    accel.flushFifo();
}

void loop()
{
    accel.flushFifo();
}
