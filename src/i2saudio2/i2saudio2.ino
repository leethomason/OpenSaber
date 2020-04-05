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
I2SAudioDriver i2sAudioDriver(&dma, &i2s, &spiFlash, manifest);

int currentDir = 0;

void scan()
{
    Log.p("p: play track channel=0").eol();
    Log.p("l: loop track channel=0").eol();
    Log.p("s: stop channel (stop all if no channel)").eol();
    Log.p("v: volume value=0,256 channel").eol();

    int start = 0, count = 0;
    manifest.dirRange(currentDir, &start, &count);
    Log.p("Dir range. start=").p(start).p(" count=").p(count).eol();
    for(int i=0; i<count; ++i)
        dumpMemUnit(i + start, manifest.getUnit(i + start));

}


void dumpMemUnit(int id, const MemUnit& memUnit)
{
    Log.p("[").p(id).p("] ").pt(memUnit.name).p(" addr=").p(memUnit.offset).p(" size=").p(memUnit.size)
       .p(" bits=").p(memUnit.is8Bit ? 8 : 4)
       .p(" table=").p(memUnit.table)
       .p(" samples=").p(memUnit.numSamples())
      .eol();
}

void processCmd(const CStr<32>& str) 
{
    CStr<32> action, str0, str1;
    CStr<32>* arr[3] = { &action, &str0, &str1};
    str.tokenize(' ', arr, 3);
    int value0 = -1, value1 = -1;
    if (!str0.empty()) value0 = atoi(str0.c_str());
    if (!str1.empty()) value1 = atoi(str1.c_str());

    //Log.p("process: ").p(action.c_str()).p(" ").pt(str0).p(" ").pt(str1).eol();

    if (action == 'p') {
        if (value1 < 0) value1 = 0;
        Log.p("Play track=").p(value0).p(" channel=").p(value1).eol();
        i2sAudioDriver.play(value0, false, value1);
    }
    else if (action == 'l') {
        if (value1 < 0) value1 = 0;
        Log.p("Loop track=").p(value0).p(" channel=").p(value1).eol();
        i2sAudioDriver.play(value0, true, value1);
    }
    else if (action == 's') {
        if (value0 >= 0) {
            Log.p("Stop channel=").p(value0).eol();
            i2sAudioDriver.stop(value0);
        }
        else {
            Log.p("Stop all.").eol();
            for(int i=0; i<AUDDRV_NUM_CHANNELS; ++i) {
                i2sAudioDriver.stop(i);
            }
        }
    }
    else if (action == 'v') {
        if (value1 < 0) value1 = 0;

        if (value0 >= 0 && value1 >= 0) {
            Log.p("Volume=").p(value0).p(" channel=").p(value1).eol();
            i2sAudioDriver.setVolume(value0, value1);
        }
        else if (value0 >= 0) {
            Log.p("Volume=").p(i2sAudioDriver.getVolume(value0)).eol();
        }
    }
    else if (action == 'd') {
        for(int i=0; i<AUDDRV_NUM_CHANNELS; ++i) {
            i2sAudioDriver.stop(i);
        }
        if (value0 >=0 && value0 < 4) {
            currentDir = value0;
            scan();
        }
    }
}

void setup()
{
    Serial.begin(19200);
    while (!Serial)
        delay(400);
    Log.attachSerial(&Serial);

    Log.p("Serial open.").eol();

    flashTransport.begin();
    spiFlash.begin();
    manifest.scan(&spiFlash);
    i2sAudioDriver.begin();

    scan();

    Log.p("setup() complete.").eol();
}

CStr<32> cmd;
static uint32_t lastLoopTime = 0;

void loop()
{
    uint32_t deltaMilli = millis() - lastLoopTime;
    if (deltaMilli > 5000) {

        Log.p("Cycle=").p(I2SAudioDriver::getCallbackCycle())
            .p(" Time%=").p(I2SAudioDriver::callbackMicros / (10 * deltaMilli))
            .eol();
        I2SAudioDriver::callbackMicros = 0;
        lastLoopTime = millis();
    }

    while (Serial.available()) {
        int c = Serial.read();
        if (c == '\n') {
            //Log.pt(cmd).eol();
            processCmd(cmd);
            cmd.clear();
        }
        else {
            cmd.append(c);
        }
    }

}