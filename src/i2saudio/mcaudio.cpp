#include <Adafruit_SPIFlash.h>
#include <Adafruit_ZeroI2S.h>
#include <Adafruit_ZeroDMA.h>

#include <Arduino.h>    

#include "mcaudio.h"
#include "Grinliz_Util.h"
#include "compress.h"
#include "mcmemimage.h"

/*
    1. SPIStream not interupt safe? If not a problem now, will be.
        a. Remove global support. Breaks virtual eeprom.
        b. Interupt safe it - performance?
        c. Lock it down at the MemImage level.
*/

// Two stereo buffers are ping-ponged between
// filling and the DMA -> DAC.
int32_t I2SAudio::audioBuffer0[STEREO_BUFFER_SAMPLES];            // stereo buffers. throwing away memory. *sigh*
int32_t I2SAudio::audioBuffer1[STEREO_BUFFER_SAMPLES];

// When uncompressed, SPI is read in as 16 bit mono,
// and they are read to a buffer that is expanded to 32 bit stereo.
uint8_t subBuffer[AUDIO_SUB_BUFFER];
wav12::Expander expander(subBuffer, AUDIO_SUB_BUFFER);
DmacDescriptor* dmacDescriptor = 0;

I2STracker I2SAudio::tracker;
I2SAudio* I2SAudio::_instance = 0;

// Information about the state of audioBuffer0/1
AudioBufferData I2SAudio::audioBufferData[NUM_AUDIO_BUFFERS];

volatile uint8_t dmaPlaybackBuffer = 0;

bool I2SAudio::isStreamQueued = false;
uint32_t I2SAudio::queued_addr = 0;
uint32_t I2SAudio::queued_size = 0;
uint32_t I2SAudio::queued_nSamples = 0;
int      I2SAudio::queued_format = 0;

void I2SAudio::timerCallback(int id)
{
    I2SAudio::tracker.timerCalls++;

    if (I2SAudio::isStreamQueued) {
        I2SAudio::isStreamQueued = false;
        I2SAudio::tracker.timerQueued++;
        SPIStream& spiStream = I2SAudio::instance()->spiStream;
        spiStream.init(I2SAudio::queued_addr, I2SAudio::queued_size);
        expander.init(&spiStream, I2SAudio::queued_nSamples, I2SAudio::queued_format);
    }

    if (audioBufferData[id].status != AUDBUF_EMPTY)
        I2SAudio::tracker.timerErrors++;

    ASSERT(audioBuffer0 == audioBufferData[0].buffer);
    ASSERT(audioBuffer1 == audioBufferData[1].buffer);
    int rc = audioBufferData[id].fillBuffer(expander, I2SAudio::instance()->expandVolume());
    if (rc != 0) {
        I2SAudio::tracker.timerErrors++;
    }
}

void I2SAudio::dmaCallback(Adafruit_ZeroDMA* dma)
{
    audioBufferData[dmaPlaybackBuffer].status = AUDBUF_EMPTY;
    dmaPlaybackBuffer = (dmaPlaybackBuffer + 1) % NUM_AUDIO_BUFFERS;
    
    I2SAudio::tracker.dmaCalls++;
    if (audioBufferData[dmaPlaybackBuffer].status != AUDBUF_READY) {
        I2SAudio::tracker.dmaErrors++;
    }
    audioBufferData[dmaPlaybackBuffer].status = AUDBUF_DRAINING;
    int32_t* src = audioBufferData[dmaPlaybackBuffer].buffer;

    dma->changeDescriptor(
        dmacDescriptor,
        src,                            // move data from here
        (void *)(&I2S->DATA[0].reg),    // to here (M0+)
        STEREO_BUFFER_SAMPLES);         // this many...
    dma->startJob();

    // Fill up the next buffer while this one is being played.
    timerCallback((dmaPlaybackBuffer + 1) % NUM_AUDIO_BUFFERS);
}

I2SAudio::I2SAudio(Adafruit_ZeroI2S& _i2s, Adafruit_ZeroDMA& _dma, Adafruit_SPIFlash& _spiFlash, SPIStream& _stream) : 
    i2s(_i2s),
    audioDMA(_dma),
    spiFlash(_spiFlash),
    spiStream(_stream)
{
    // As usual, do nothing in the constructor.
    // The services aren't started yet.
    _instance = this;
}


void I2SAudio::init()
{
    Log.p("I2SAudio::init()").eol();

    audioBufferData[0].buffer = audioBuffer0;
    audioBufferData[1].buffer = audioBuffer1;
    audioBufferData[0].reset();
    audioBufferData[1].reset();
    ASSERT(audioBuffer0 == audioBufferData[0].buffer);

#   if 0
    for (int i=0; i<2; ++i)
        testReadRate(i);
#   endif

    Log.p("Configuring DMA trigger").eol();
    audioDMA.setTrigger(I2S_DMAC_ID_TX_0);
    audioDMA.setAction(DMA_TRIGGER_ACTON_BEAT);

    ZeroDMAstatus stat = audioDMA.allocate();
    if (stat == DMA_STATUS_OK) {
        Log.p("DMA status OK.").eol();
    }
    else {
        audioDMA.printStatus(stat);
    }

    Log.p("DMA: Setting up transfer").eol();
    dmacDescriptor = audioDMA.addDescriptor(
        audioBuffer0,                   // move data from here
        (void *)(&I2S->DATA[0].reg),    // to here (M0+)
        STEREO_BUFFER_SAMPLES,          // this many...
        DMA_BEAT_SIZE_WORD,             // bytes/hword/words
        true,                           // increment source addr?
        false);

    Log.p("DMA: Adding callback").eol();
    audioDMA.setCallback(I2SAudio::dmaCallback);

    Log.p("Starting I2S").eol();
    i2s.begin(I2S_32_BIT, AUDIO_FREQ);
    i2s.enableTx();

    I2SAudio::timerCallback(0);     
    Log.p("Buffer 0 status=").p(audioBufferData[0].status == AUDBUF_READY ? "ready" : "ERROR")
       .p(" dmaPlaybackBuffer=").p(dmaPlaybackBuffer).eol();

    dmaPlaybackBuffer = 1;  // seed to the first, so the dma will switch back to 0.
    stat = audioDMA.startJob();
    Log.p("Audio init complete.").eol();
}


void I2SAudio::testReadRate(int index)
{
    MemUnit file;
    readFile(spiFlash, index, &file);
    wav12::Wav12Header header;
    uint32_t baseAddr = 0;
    readAudioInfo(spiFlash, file, &header, &baseAddr);

    Log.p("Test: lenInBytes=").p(header.lenInBytes).p(" nSamples=").p(header.nSamples).p(" format=").p(header.format).eol();
    spiStream.init(baseAddr, header.lenInBytes);
    expander.init(&spiStream, header.nSamples, header.format);
    int volume = 1000;

    uint32_t start = millis();
    while(expander.pos() < expander.samples()) {
        audioBufferData[0].reset();
        audioBufferData[0].fillBuffer(expander, volume);
    }
    uint32_t end = millis();
    Log.p("Index ").p(index).p( " samples/second=").p((expander.samples()*uint32_t(1000))/(end - start)).eol();

    audioBufferData[0].reset();
}


bool I2SAudio::play(int fileIndex)
{
    MemUnit file;

    readFile(spiFlash, fileIndex, &file);
    wav12::Wav12Header header;
    uint32_t baseAddr = 0;
    readAudioInfo(spiFlash, file, &header, &baseAddr);

    //Log.p("Play [").p(fileIndex).p("]: lenInBytes=").p(header.lenInBytes).p(" nSamples=").p(header.nSamples).p(" format=").p(header.format).eol();

    // Queue members need to be in the no-interupt lock since
    // it is read and modified by the timer callback. readFile()
    // above will acquire and release the lock on its own.

    noInterrupts();
    I2SAudio::queued_addr = baseAddr;
    I2SAudio::queued_size = header.lenInBytes;
    I2SAudio::queued_nSamples = header.nSamples;
    I2SAudio::queued_format = header.format;
    I2SAudio::isStreamQueued = true;
    interrupts();

    return header.nSamples > 0;
}


bool I2SAudio::play(const char* filename)
{
    int index = MemImage.lookup(filename);
    if (index >= 0) {
        play(index);
    }
    return true;
}


void I2SAudio::stop()
{
    //Log.p("stop").eol();
    noInterrupts();
    I2SAudio::queued_addr = 0;
    I2SAudio::queued_size = 0;
    I2SAudio::queued_nSamples = 0;
    I2SAudio::queued_format = 0;
    I2SAudio::isStreamQueued = true;
    interrupts();
}


bool I2SAudio::isPlaying() const
{
    noInterrupts();
    bool isQueued = I2SAudio::isStreamQueued;
    bool hasSamples = expander.pos() < expander.samples();    
    interrupts();

    return isQueued || hasSamples;
}


void I2SAudio::process()
{
    if(tracker.hasErrors()) {
        dumpStatus();
        ASSERT(false);
    }
}


void I2SAudio::dumpStatus()
{
    if (tracker.hasErrors())
        Log.p("Audio tracker ERROR.").eol();

    Log.p(" Timer calls:").p(tracker.timerCalls)
        .p(" queue:").p(tracker.timerQueued)
        .p(" errors:").p(tracker.timerErrors)

        .p(" DMA calls:").p(tracker.dmaCalls)
        .p(" errors:").p(tracker.dmaErrors)
        
        .p(" Fill empty:").p(tracker.fillEmpty)
        .p(" some:").p(tracker.fillSome)
        .p(" errors:").p(tracker.fillErrors)
        .p(" crit errors:").p(tracker.fillCritErrors)
        .eol();
    tracker.reset();
}


int AudioBufferData::fillBuffer(wav12::Expander& expander, int32_t volume)
{
    if (status != AUDBUF_EMPTY) {
        I2SAudio::tracker.fillErrors++;
    }
    status = AUDBUF_FILLING;

    uint32_t MILLION2 = 2 * 1024 * 1024;
    if (expander.samples() < expander.pos()) {
        I2SAudio::tracker.fillCritErrors++;
        status = AUDBUF_READY;
        return AUDERROR_SAMPLES_POS_OUT_OF_RANGE;
    }
    if (expander.samples() > MILLION2 || expander.pos() > MILLION2) {
        I2SAudio::tracker.fillCritErrors++;
        status = AUDBUF_READY;
        return AUDERROR_SAMPLES_POS_OUT_OF_RANGE;
    }

    uint32_t toRead = glMin(expander.samples() - expander.pos(), (uint32_t)AUDIO_BUFFER_SAMPLES);
    if (toRead) {
        // The expand is buried here:
        expander.expand2(buffer, toRead, volume);
        I2SAudio::tracker.fillSome++;
    }
    else {
        I2SAudio::tracker.fillEmpty++;
    }

    for(int i=toRead*2; i<STEREO_BUFFER_SAMPLES; ++i) {
        buffer[i] = 0;
    }
    status = AUDBUF_READY;
    return AUDERROR_NONE;
}


uint32_t SPIStream::fetch(uint8_t* target, uint32_t nBytes)
{
    // Normally, the SPI would need to be locked.
    // HOWEVER, this is only called by the interrupt, so 
    // all should be well.
    uint32_t r = m_flash.readBuffer(
        m_addr + m_pos, 
        target,
        nBytes);
    m_pos += nBytes;
    return r;
}
