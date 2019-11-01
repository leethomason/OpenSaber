#include <Adafruit_ZeroI2S.h>
#include <Adafruit_ZeroDMA.h>
#include <Adafruit_SPIFlash.h>

#include "Grinliz_Util.h"
#include "mcmemimage.h"
#include "mcaudio.h"
#include "compress.h"

#define SERIAL_SPEED 19200 //115200
#define LOCAL_PAGESIZE 256

extern "C" char *sbrk(int i);

Adafruit_ZeroI2S i2s(0, 1, 12, 2);
Adafruit_FlashTransport_SPI flashTransport(SS1, &SPI1);
Adafruit_SPIFlash spiFlash(&flashTransport);     // Use hardware SPI 
Adafruit_ZeroDMA audioDMA;
SPIStream spiStream(spiFlash);

ConstMemImage MemImage(spiFlash);
I2SAudio i2sAudio(i2s, audioDMA, spiFlash);

Random randPlus;
bool testMode = false;
uint32_t testTime = 0;

void setup()
{
    Serial.begin(SERIAL_SPEED);
    while (!Serial)
        delay(400);
    Log.attachSerial(&Serial);

    Log.p("Serial open.").eol();
    Log.p("p: play track channel=0").eol();
    Log.p("l: loop track channel=0").eol();
    Log.p("s: stop channel (stop all if no channel)").eol();
    Log.p("v: volume value=0,256 channel").eol();

    SPI.begin();
    spiFlash.begin();
    uint32_t jedecid = spiFlash.getJEDECID();
    Log.p("SPI Flash JEDECID=").p(jedecid).eol();

    MemImage.begin();
    dumpImage(spiFlash);

    i2sAudio.initStream(&spiStream, 0);
    i2sAudio.init();
    i2sAudio.setVolume(50, 0);

    Log.p("Setup complete.");
}

CStr<32> cmd;
CQueue<16> queue;

void processCmd(const CStr<32>& str) 
{
    CStr<32> action, str0, str1;
    CStr<32>* arr[3] = { &action, &str0, &str1};
    str.tokenize(' ', arr, 3);
    int value0 = -1, value1 = -1;
    if (!str0.empty()) value0 = atoi(str0.c_str());
    if (!str1.empty()) value1 = atoi(str0.c_str());

    if (action == 'p') {
        if (value1 < 0) value1 = 0;
        Log.p("Play track=").p(value0).p(" channel=").p(value1).eol();
        i2sAudio.play(value0, false, value1);
    }
    else if (action == 'l') {
        if (value1 < 0) value1 = 0;
        Log.p("Loop track=").p(value0).p(" channel=").p(value1).eol();
        i2sAudio.play(value0, true, value1);
    }
    else if (action == 's') {
        if (value0 >= 0) {
            Log.p("Stop channel=").p(value0).eol();
            i2sAudio.stop(value0);
        }
        else {
            Log.p("Stop all.").eol();
            for(int i=0; i<NUM_AUDIO_CHANNELS; ++i) {
                i2sAudio.stop(i);
            }
        }
    }
    else if (action == 'v') {
        if (value0 >= 0 && value1 >= 0) {
            Log.p("Volume=").p(value0).p(" channel=").p(value1).eol();
            i2sAudio.setVolume(value0, value1);
        }
        else if (value0 >= 0) {
            Log.p("Volume=").p(i2sAudio.volume(value0)).eol();
        }
    }
}

void loop()
{
    i2sAudio.process();

    bool error = i2sAudio.tracker.hasErrors();
    if(error) {
        i2sAudio.dumpStatus();
        while (error) {}
    }
    if (!i2sAudio.isPlaying(0) && !queue.empty()) {
        int id = queue.pop();
        i2sAudio.play(id, false, 0);
    }

    if (testMode) {
        if (!i2sAudio.isPlaying(0)) 
            i2sAudio.play(5, false, 0);
        if (testTime <= millis()) {
            testTime = millis() + randPlus.rand(3000);
            i2sAudio.play(randPlus.rand(5), false, 0);
        }
    }

    while (Serial.available()) {
        int c = Serial.read();
        if (c == '\n') {
            Log.pt(cmd).eol();
            processCmd(cmd);
            cmd.clear();
        }
        else {
            cmd.append(c);
        }
    }
}
