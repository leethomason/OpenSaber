#include <Adafruit_ZeroI2S.h>
#include <Adafruit_ZeroDMA.h>
#include <Adafruit_ZeroTimer.h>
#include "utility/dma.h"
#include <Adafruit_SPIFlash.h>

#include "Grinliz_Util.h"
#include "mcmemimage.h"
#include "mcaudio.h"
#include "compress.h"

#define SERIAL_SPEED 115200
#define LOCAL_PAGESIZE 256

extern "C" char *sbrk(int i);

Adafruit_ZeroI2S i2s(0, 1, 12, 2);
Adafruit_SPIFlash spiFlash(SS1, &SPI1);     // Use hardware SPI 
Adafruit_ZeroDMA audioDMA;
SPIStream spiStream(spiFlash);
Adafruit_ZeroTimer zt4(4);
I2SAudio i2sAudio(i2s, zt4, audioDMA);

int32_t volume = 30*1000; // loudest: 65536

Timer2 statusTimer(1000);
uint32_t lastTime = 0;

int FreeRam () {
  char stack_dummy = 0;
  return &stack_dummy - sbrk(0);
}

#if 0
void testReadRate(int index)
{
    MemUnit file;
    readFile(spiFlash, index, &file);
    wav12::Wav12Header header;
    uint32_t baseAddr = 0;
    readAudioInfo(spiFlash, file, &header, &baseAddr);

    Log.p("Test: lenInBytes=").p(header.lenInBytes).p(" nSamples=").p(header.nSamples).p(" format=").p(header.format).eol();
    spiStream.init(baseAddr, header.lenInBytes);
    expander.init(&spiStream, header.nSamples, header.format);

    uint32_t start = millis();
    while(expander.pos() < expander.samples()) {
        audioBufferData[0].reset();
        audioBufferData[0].fillBuffer(spiFlash, expander, volume);
    }
    uint32_t end = millis();
    Log.p("Index ").p(index).p( " samples/second=").p((expander.samples()*uint32_t(1000))/(end - start)).eol();

    audioBufferData[0].reset();
}
#endif


void setup()
{
    Serial.begin(SERIAL_SPEED);
    while (!Serial)
        delay(400);
    Log.attachSerial(&Serial);

    Log.p("Serial open.").eol();

    spiFlash.begin(SPIFLASHTYPE_W25Q16BV);
    uint8_t manid, devid;
    spiFlash.GetManufacturerInfo(&manid, &devid);
    Log.p("SPI Flash Memory").eol();
    Log.p("Manufacturer: 0x").p(manid, HEX).eol();
    Log.p("Device ID: 0x").p(devid, HEX).eol();
    Log.p("Pagesize: ").p(spiFlash.pageSize()).p(" Page buffer: ").p(LOCAL_PAGESIZE).eol();

    i2sAudio.initStream(spiStream);
    i2sAudio.init();
    i2sAudio.setVolume(20);

    Log.p("Free ram:").p(FreeRam()).eol();

    dumpImage(spiFlash);

    //testReadRate(0);
    //testReadRate(1);

#if 0
    MemUnit file;
    readFile(spiFlash, 1, &file);
    wav12::Wav12Header header;
    uint32_t baseAddr = 0;
    readAudioInfo(spiFlash, file, &header, &baseAddr);

    Log.p("Init: lenInBytes=").p(header.lenInBytes).p(" nSamples=").p(header.nSamples).p(" format=").p(header.format).eol();
#endif
    lastTime = millis();
}

void loop()
{
    uint32_t t = millis();
    if (statusTimer.tick(t - lastTime)) {
        Log.p(" callbacks=").p(I2SAudio::timerCallbacks).p(" hits=").p(I2SAudio::timerCallbackHits).p(" errors=").p(I2SAudio::callbackErrors).eol();
    }
    lastTime = t;
}
