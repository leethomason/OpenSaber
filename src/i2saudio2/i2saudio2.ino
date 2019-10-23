#include "Grinliz_Util.h"
#include "manifest.h"
#include "i2saudiodrv.h"

#include <Adafruit_SPIFlash.h>
#include <Adafruit_ZeroI2S.h>
#include <Adafruit_ZeroDMA.h>

Adafruit_FlashTransport_SPI flashTransport(SS1, &SPI1);
Adafruit_SPIFlash spiFlash(&flashTransport);     // Use hardware SPI 
Adafruit_ZeroDMA dma;
Adafruit_ZeroI2S i2s(0, 1, 12, 2);
Manifest manifest;
I2SAudioDriver i2sAudioDriver(&dma, &i2s, manifest);

int currentDir = 0;

void dumpMemUnit(int id, const MemUnit& memUnit)
{
    Log.p("[").p(id).p("] ").pt(memUnit.name).p(" addr=").p(memUnit.offset).p(" size=").p(memUnit.size)
       .p(" bits=").p(memUnit.is8Bit ? 8 : 4)
       .p(" samples=").p(memUnit.numSamples())
      .eol();
}

void setup()
{
    Serial.begin(19200);
    while (!Serial)
        delay(400);
    Log.attachSerial(&Serial);

    flashTransport.begin();
    spiFlash.begin();
    manifest.scan(&spiFlash);

    int start = 0, count = 0;
    manifest.dirRange(currentDir, &start, &count);
    Log.p("Dir range. start=").p(start).p(" count=").p(count).eol();
    for(int i=0; i<count; ++i)
        dumpMemUnit(i + start, manifest.getUnit(i + start));

    i2sAudioDriver.begin();

    Log.p("setup() complete.").eol();
}

void loop()
{
    Log.p("cycle=").p(I2SAudioDriver::getCallbackCycle()).eol();
    delay(1000);
}