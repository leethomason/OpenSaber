#include <Arduino.h>    

// #define LOG_TIME

// This audio system is set up for using I2S on
// an Itsy-Bitsy M0. Relies on DMA and SPI memory
// streaming, so work needs to be done to get it
// to compile & run on other platforms.
#ifndef CORE_TEENSY

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

// Could be a static:
wav12::ExpanderV expander[NUM_AUDIO_CHANNELS];
DmacDescriptor* dmacDescriptor = 0;

I2STracker I2SAudio::tracker;
I2SAudio* I2SAudio::_instance = 0;

// Information about the state of audioBuffer0/1
AudioBufferData I2SAudio::audioBufferData[NUM_AUDIO_BUFFERS];

volatile uint8_t dmaPlaybackBuffer = 0;

I2SAudio::ChangeReq I2SAudio::changeReq[NUM_AUDIO_CHANNELS];

void I2SAudio::outerFill(int id)
{
    I2SAudio::tracker.timerCalls++;
    I2SAudio *audio = I2SAudio::instance();

    for(int i=0; i<NUM_AUDIO_CHANNELS; ++i) {
        if (changeReq[i].isQueued) {
            ChangeReq& cr = changeReq[i];
            cr.isQueued = false;
            I2SAudio::tracker.timerQueued++;
            wav12::IStream *iStream = I2SAudio::instance()->iStream[i];
            iStream->set(cr.addr, cr.size);
            expander[i].attach(iStream);
            audio->looping[i] = cr.loop;
        }
    }

    ASSERT(audioBuffer0 == audioBufferData[0].buffer);
    ASSERT(audioBuffer1 == audioBufferData[1].buffer);
    ASSERT(audio);
    
    for (int i = 0; i < NUM_AUDIO_CHANNELS; ++i)
    {
        bool add = i > 0;
        bool looping = audio->looping[i];
        int rc = audioBufferData[id].fillBuffer(expander[i], audio->expandVolume(i), looping, add);
        if (rc != 0)
        {
            I2SAudio::tracker.timerErrors++;
        }
    }
}

void I2SAudio::dmaCallback(Adafruit_ZeroDMA* dma)
{
    const uint32_t start = micros();
    dmaPlaybackBuffer = (dmaPlaybackBuffer + 1) % NUM_AUDIO_BUFFERS;
    
    I2SAudio::tracker.dmaCalls++;
    int32_t* src = audioBufferData[dmaPlaybackBuffer].buffer;

    dma->changeDescriptor(
        dmacDescriptor,
        src,                            // move data from here
        (void *)(&I2S->DATA[0].reg),    // to here (M0+)
        STEREO_BUFFER_SAMPLES);         // this many...
    dma->startJob();

    // Fill up the next buffer while this one is being played.
    outerFill((dmaPlaybackBuffer + 1) % NUM_AUDIO_BUFFERS);
    const uint32_t end = micros();
    I2SAudio::tracker.dmaMicros += (end - start);
}

I2SAudio::I2SAudio(Adafruit_ZeroI2S &_i2s, Adafruit_ZeroDMA &_dma, Adafruit_SPIFlash &_spiFlash) : 
    i2s(_i2s),
    audioDMA(_dma),
    spiFlash(_spiFlash)
{
    // As usual, do nothing in the constructor.
    // The services aren't started yet.
    _instance = this;

    for(int i=0; i<NUM_AUDIO_CHANNELS; i++) {
        iStream[i] = 0;
        volume256[i] = 256;
        looping[i] = 0;
        changeReq[i].isQueued = false;
    }
}

void I2SAudio::init()
{
    Log.p("I2SAudio::init()").eol();
    for(int i=0; i<NUM_AUDIO_CHANNELS; i++) {
        if (iStream[i] == 0) {
            Log.p("I2SAudio::init() IStream index=").p(i).p(" is null.").eol();
            ASSERT(false);
        }
    }

    for(int i=0; i<NUM_AUDIO_CHANNELS; i++) {
        changeReq[i].reset();
    }

    audioBufferData[0].buffer = audioBuffer0;
    audioBufferData[1].buffer = audioBuffer1;
    audioBufferData[0].reset();
    audioBufferData[1].reset();
    ASSERT(audioBuffer0 == audioBufferData[0].buffer);

    Log.p("Configuring DMA trigger").eol();
    audioDMA.setTrigger(I2S_DMAC_ID_TX_0);
    audioDMA.setAction(DMA_TRIGGER_ACTON_BEAT);

    ZeroDMAstatus stat = audioDMA.allocate();
    if (stat == DMA_STATUS_OK)
    {
        Log.p("DMA status OK.").eol();
    }
    else
    {
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

    Log.p("Initial outerFill").eol();
    I2SAudio::outerFill(0);
    dmaPlaybackBuffer = 1;  // seed to the first, so the dma will switch back to 0.
    stat = audioDMA.startJob();
    Log.p("Audio init complete.").eol();
}

bool I2SAudio::play(int fileIndex, bool loop, int channel)
{
    MemUnit file;

    readFile(spiFlash, fileIndex, &file);
    wav12::Wav12Header header;
    uint32_t baseAddr = 0;
    readAudioInfo(spiFlash, file, &header, &baseAddr);

    Log.p("Play [").p(fileIndex).p("]: lenInBytes=").p(header.lenInBytes).p(" nSamples=").p(header.nSamples).p(" baseAddr=").p(baseAddr).p(" format=").p(header.format).eol();

    // Queue members need to be in the no-interupt lock since
    // it is read and modified by the timer callback. readFile()
    // above will acquire and release the lock on its own.

    channel = clamp(channel, 0, NUM_AUDIO_CHANNELS-1);
    noInterrupts();
    ChangeReq &cr = changeReq[channel];
    cr.addr = baseAddr;
    cr.size = header.lenInBytes;
    cr.nSamples = header.nSamples;
    cr.format = header.format;
    cr.loop = loop;
    cr.isQueued = true;
    interrupts();

    return header.nSamples > 0;
}

bool I2SAudio::play(const char *filename, bool loop, int channel)
{
    Log.p("Play(").p(filename).p(")").eol();
    int index = MemImage.lookup(filename);
    if (index >= 0) {
        play(index, loop, clamp(channel, 0, NUM_AUDIO_CHANNELS-1));
    }
    return true;
}

void I2SAudio::stop(int channel)
{
    //Log.p("stop").eol();
    channel = clamp(channel, 0, NUM_AUDIO_CHANNELS-1);
    noInterrupts();
    ChangeReq &cr = changeReq[channel];
    cr.addr = 0;
    cr.size = 0;
    cr.nSamples = 0;
    cr.format = 0;
    cr.loop = false;
    cr.isQueued = true;
    interrupts();
}

bool I2SAudio::isPlaying(int channel) const
{
    channel = clamp(channel, 0, NUM_AUDIO_CHANNELS-1);
    noInterrupts();
    ChangeReq &cr = changeReq[channel];
    bool isQueued = cr.isQueued;
    bool hasSamples = !expander[channel].done();
    interrupts();

    return isQueued || hasSamples;
}

void I2SAudio::process()
{
#ifdef LOG_TIME
    uint32_t t = millis();
    if (t - lastLogTime >= 1000)
    {
        lastLogTime = t;
        uint32_t dmaTime = tracker.dmaMicros;
        uint32_t nCalls = tracker.dmaCalls;
        uint32_t wallTime = nCalls * MICRO_PER_AUDIO_BUFFER / 1000;
        if (wallTime > 0)
        {
            Log.p("CPU utiliziation for audio callback: ").p(dmaTime / wallTime).p(" / 1000").eol();
            tracker.dmaMicros = 0;
            tracker.dmaCalls = 0;
        }
    }
#endif
    if (tracker.hasErrors())
    {
        dumpStatus();
        ASSERT(false);
    }
}

void I2SAudio::dumpStatus()
{
    if (tracker.hasErrors())
        Log.p("Audio tracker ERROR.").eol();
    Log.p(" OuterFill calls:").p(tracker.timerCalls).p(" queue:").p(tracker.timerQueued).p(" errors:").p(tracker.timerErrors)

        .p(" DMA calls:")
        .p(tracker.dmaCalls)
        .p(" errors:")
        .p(tracker.dmaErrors)

        .p(" Fill empty:")
        .p(tracker.fillEmpty)
        .p(" some:")
        .p(tracker.fillSome)
        .p(" errors:")
        .p(tracker.fillErrors)
        .p(" crit errors:")
        .p(tracker.fillCritErrors)
        .eol();

    tracker.reset();
}

int AudioBufferData::fillBuffer(wav12::ExpanderV &expander, int32_t volume, bool loop, bool add)
{
    if (loop)
    {
        uint32_t totalRead = 0;
        while (totalRead < AUDIO_BUFFER_SAMPLES)
        {
            if (expander.done())
                expander.rewind();
            uint32_t read = expander.expand(buffer + totalRead * 2, AUDIO_BUFFER_SAMPLES - totalRead, volume, add);
            totalRead += read;
        }
        I2SAudio::tracker.fillSome++;
    }
    else
    {
        uint32_t toRead = expander.expand(buffer, AUDIO_BUFFER_SAMPLES, volume, add);
        if (toRead)
            I2SAudio::tracker.fillSome++;
        else
            I2SAudio::tracker.fillEmpty++;

        if (!add)
        {
            for (uint32_t i = toRead * 2; i < STEREO_BUFFER_SAMPLES; ++i)
            {
                buffer[i] = 0;
            }
        }
    }
    return AUDERROR_NONE;
}

void SPIStream::set(uint32_t addr, uint32_t size)
{
    m_addr = addr;
    m_size = size;
    m_pos = 0;
}

// Call from interrupt
void SPIStream::rewind()
{
    m_pos = 0;
}

// Call from interrupt
uint32_t SPIStream::fetch(uint8_t* target, uint32_t nBytes)
{
    // Normally, the SPI would need to be locked.
    // HOWEVER, this is only called by the interrupt, so 
    // all should be well.

    // Don't read over the end. (Oops - that was a bug.)

    if (m_pos + nBytes > m_size)
        nBytes = m_size - m_pos;

    uint32_t r = m_flash.readBuffer(
        m_addr + m_pos, 
        target,
        nBytes);

    m_pos += r;
    return r;
}

#endif // !CORE_TEENSY
