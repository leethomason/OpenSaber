#include <Arduino.h>
#include "grinliz_flash.h"
#include "manifest.h"

Manifest manifest;
static const int SIZE = 256;
uint8_t buffer[SIZE];

void dumpMemUnit(int id, const MemUnit& memUnit)
{
    Log.p("[").p(id).p("] ").pt(memUnit.name).p(" addr=").p(memUnit.offset).p(" size=").p(memUnit.size)
       .p(" bits=").p(memUnit.is8Bit ? 8 : 4)
       .p(" table=").p(memUnit.table)
       .p(" samples=").p(memUnit.numSamples())
      .eol();
}

uint32_t readTest(uint32_t start, uint32_t _count, IMemory* mem)
{
    uint32_t count = _count;
    uint32_t totalTime = 0;

    uint32_t crc = 0;
    uint32_t startTime = micros();
    while(count) {
        int n = SIZE;
        if (count < SIZE) n = count;
        mem->readMemory(start, buffer, n);
        //totalTime += endTime - startTime;

        for(int i=0; i<n; ++i) {
            crc += buffer[i];
        }
        start += n;
        count -= n;
    }
    uint32_t endTime = micros();
    totalTime = endTime - startTime;
    //Log.p("count=").p(_count).p(" time=").p(totalTime).eol();
    Log.p("KB/second=").p(1000U * _count / totalTime).eol();

    return crc;
}

void setup() {
    Serial.begin(19200);  // still need to turn it on in case a command line is connected.
    while (!Serial) {
        delay(100);
    }
    Serial.println("Hello.");
    Log.attachSerial(&Serial);
    
    GrinlizFlash flash(SS1, &SPI1);
    flash.begin();
    uint32_t jedec = flash.getJEDECID();
    Serial.print("Jedec: "); Serial.println(jedec, HEX);

    manifest.scan(&flash);
    int start = 0, count = 0;
    manifest.dirRange(0, &start, &count);
    Log.p("Dir range. start=").p(start).p(" count=").p(count).eol();
    for(int i=0; i<count; ++i)
        dumpMemUnit(i + start, manifest.getUnit(i + start));

    for(int i=20; i<=24; ++i) {
        const MemUnit& unit = manifest.getUnit(i);
        uint32_t crc = readTest(unit.offset, unit.size, &flash);
        Serial.print("crc="); Serial.println(crc);
    }

    Serial.println("Done.");
}

void loop() {
}
