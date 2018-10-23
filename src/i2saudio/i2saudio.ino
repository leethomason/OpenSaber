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
DmacDescriptor* dmacDescriptor;
Adafruit_ZeroTimer zt(4);

// Two stereo buffers are ping-ponged between
// filling and the DMA -> DAC.
AudioBufferData audioBufferData[NUM_AUDIO_BUFFERS];     // Information about the state of audioBuffer0/1
int32_t audioBuffer0[STEREO_BUFFER_SAMPLES];            // stereo buffers. throwing away memory. *sigh*
int32_t audioBuffer1[STEREO_BUFFER_SAMPLES];

// When uncompressed, SPI is read in as 16 bit mono,
// and they are read to a buffer that is expanded to 32 bit stereo.
uint8_t subBuffer[AUDIO_SUB_BUFFER];

wav12::Expander expander(subBuffer, AUDIO_SUB_BUFFER);
SPIStream spiStream(spiFlash);

int32_t volume = 30*1000; // loudest: 65536
uint8_t dmaPlaybackBuffer = 0;

uint8_t callbackErrors = 0;
Timer2 statusTimer(1000);
uint32_t lastTime = 0;

int probes = 0;
int hits = 0;

void TC4_Handler(){
    Adafruit_ZeroTimer::timerHandler(4);
}

void TimerCallback()
{
    probes++;
    for(int i=0; i<NUM_AUDIO_BUFFERS; ++i) {
        if (audioBufferData[i].status == AUDBUF_EMPTY) {
            int rc = audioBufferData[i].fillBuffer(spiFlash, expander, volume);
            if (rc != 0) {
                Log.p("fillBuffer rc=").p(rc).eol();
            }
            hits++;
        }
    }
}

int FreeRam () {
  char stack_dummy = 0;
  return &stack_dummy - sbrk(0);
}

void dma_callback(Adafruit_ZeroDMA *dma)
{
    audioBufferData[dmaPlaybackBuffer].status = AUDBUF_EMPTY;

    dmaPlaybackBuffer = (dmaPlaybackBuffer + 1) % NUM_AUDIO_BUFFERS;
    
    if (audioBufferData[dmaPlaybackBuffer].status != AUDBUF_READY) {
        callbackErrors++;
    }
    audioBufferData[dmaPlaybackBuffer].status = AUDBUF_DRAINING;
    int32_t* src = audioBufferData[dmaPlaybackBuffer].buffer;

    audioDMA.changeDescriptor(
        dmacDescriptor,
        src,                            // move data from here
        (void *)(&I2S->DATA[0].reg),    // to here (M0+)
        STEREO_BUFFER_SAMPLES);      // this many...
    audioDMA.startJob();
}

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


void setup()
{
    Serial.begin(SERIAL_SPEED);
    while (!Serial)
        delay(400);
    Log.attachSerial(&Serial);

    Log.p("Serial open.").eol();

    audioBufferData[0].buffer = audioBuffer0;
    audioBufferData[1].buffer = audioBuffer1;
    audioBufferData[0].reset();
    audioBufferData[1].reset();

    spiFlash.begin(SPIFLASHTYPE_W25Q16BV);
    uint8_t manid, devid;
    spiFlash.GetManufacturerInfo(&manid, &devid);
    Log.p("SPI Flash Memory").eol();
    Log.p("Manufacturer: 0x").p(manid, HEX).eol();
    Log.p("Device ID: 0x").p(devid, HEX).eol();
    Log.p("Pagesize: ").p(spiFlash.pageSize()).p(" Page buffer: ").p(LOCAL_PAGESIZE).eol();

    dumpImage(spiFlash);

    testReadRate(0);
    testReadRate(1);

    MemUnit file;
    readFile(spiFlash, 1, &file);
    wav12::Wav12Header header;
    uint32_t baseAddr = 0;
    readAudioInfo(spiFlash, file, &header, &baseAddr);

    Log.p("Init: lenInBytes=").p(header.lenInBytes).p(" nSamples=").p(header.nSamples).p(" format=").p(header.format).eol();
    spiStream.init(baseAddr, header.lenInBytes);
    expander.init(&spiStream, header.nSamples, header.format);
    //Log.p("Expander: pos=").p(expander.pos()).p(" samples=").p(expander.samples()).eol();

    audioBufferData[0].reset();
    audioBufferData[1].reset();
    audioBufferData[0].fillBuffer(spiFlash, expander, volume);
    //Log.p("Expander: pos=").p(expander.pos()).p(" samples=").p(expander.samples()).eol();

    Log.p("Configuring DMA trigger").eol();
    audioDMA.setTrigger(I2S_DMAC_ID_TX_0);
    audioDMA.setAction(DMA_TRIGGER_ACTON_BEAT);

    ZeroDMAstatus stat = audioDMA.allocate();
    audioDMA.printStatus(stat);

    Log.p("Setting up transfer").eol();
    dmacDescriptor = audioDMA.addDescriptor(
        audioBuffer0,                   // move data from here
        (void *)(&I2S->DATA[0].reg),    // to here (M0+)
        STEREO_BUFFER_SAMPLES,       // this many...
        DMA_BEAT_SIZE_WORD,             // bytes/hword/words
        true,                           // increment source addr?
        false);

    Log.p("Adding callback").eol();
    audioDMA.setCallback(dma_callback);

    i2s.begin(I2S_32_BIT, AUDIO_FREQ);
    i2s.enableTx();

    Log.p("Free ram:").p(FreeRam()).eol();

    // Clock: 48 000 000 / second
    // div 1024 = 46875 / second
    // 
    zt.configure( TC_CLOCK_PRESCALER_DIV1024,     // prescaler
                  TC_COUNTER_SIZE_16BIT,         // bit width of timer/counter
                  TC_WAVE_GENERATION_MATCH_PWM   // frequency or PWM mode
                 );

    //zt.setCompare(0, 46875);
    static const uint32_t CLOCK = (48*1000*1000) / 1024;         // ticks / second
    static const uint32_t HZ = 1000 / MSEC_PER_AUDIO_BUFFER;
    static const uint32_t COUNTER = CLOCK / (HZ * 4);
    Log.p("MSec/Buffer=").p(MSEC_PER_AUDIO_BUFFER).p(" Counter=").p(COUNTER).eol();
    zt.setCompare(0, COUNTER);

    zt.setCallback(true, TC_CALLBACK_CC_CHANNEL0, TimerCallback);  // this one sets pin low
    zt.enable(true);

    stat = audioDMA.startJob();
    lastTime = millis();
}

void loop()
{
    if (callbackErrors > 0) {
        Log.p("Callback error count=").p(callbackErrors).eol();
        callbackErrors = 0;
    }
    uint32_t t = millis();
    if (statusTimer.tick(t - lastTime)) {
        Log.p("Read pos=").p(expander.pos()).p("/").p(expander.samples())
           .p(" probes=").p(probes).p(" hits=").p(hits)
           .eol();
        probes = hits = 0;
    }
    lastTime = t;
}
