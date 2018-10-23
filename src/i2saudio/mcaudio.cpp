#include <Adafruit_SPIFlash.h>
#include <Adafruit_ZeroTimer.h>
#include <Adafruit_ZeroI2S.h>
#include <Adafruit_ZeroDMA.h>

#include "mcaudio.h"
#include "Grinliz_Util.h"
#include "compress.h"

// Two stereo buffers are ping-ponged between
// filling and the DMA -> DAC.
int32_t audioBuffer0[STEREO_BUFFER_SAMPLES];            // stereo buffers. throwing away memory. *sigh*
int32_t audioBuffer1[STEREO_BUFFER_SAMPLES];

// When uncompressed, SPI is read in as 16 bit mono,
// and they are read to a buffer that is expanded to 32 bit stereo.
uint8_t subBuffer[AUDIO_SUB_BUFFER];
wav12::Expander expander(subBuffer, AUDIO_SUB_BUFFER);
DmacDescriptor* dmacDescriptor = 0;

uint32_t I2SAudio::timerCallbacks = 0;
uint32_t I2SAudio::timerCallbackHits = 0;
uint32_t I2SAudio::callbackErrors = 0;
I2SAudio* I2SAudio::_instance = 0;

// Information about the state of audioBuffer0/1
AudioBufferData I2SAudio::audioBufferData[NUM_AUDIO_BUFFERS];

volatile uint8_t dmaPlaybackBuffer = 0;

void TC4_Handler(){
    Adafruit_ZeroTimer::timerHandler(4);
}

void I2SAudio::timerCallback()
{
    I2SAudio::timerCallbacks++;
    for(int i=0; i<NUM_AUDIO_BUFFERS; ++i) {
        if (audioBufferData[i].status == AUDBUF_EMPTY) {
            int rc = audioBufferData[i].fillBuffer(expander, I2SAudio::instance()->expandVolume());
            if (rc != 0) {
                Log.p("fillBuffer rc=").p(rc).eol();
            }
            I2SAudio::timerCallbackHits++;
        }
    }
}

void I2SAudio::dmaCallback(Adafruit_ZeroDMA *dma)
{
    audioBufferData[dmaPlaybackBuffer].status = AUDBUF_EMPTY;
    dmaPlaybackBuffer = (dmaPlaybackBuffer + 1) % NUM_AUDIO_BUFFERS;
    
    if (audioBufferData[dmaPlaybackBuffer].status != AUDBUF_READY) {
        I2SAudio::callbackErrors++;
    }
    audioBufferData[dmaPlaybackBuffer].status = AUDBUF_DRAINING;
    int32_t* src = audioBufferData[dmaPlaybackBuffer].buffer;
    Adafruit_ZeroDMA& audioDMA = I2SAudio::instance()->audioDMA;

    audioDMA.changeDescriptor(
        dmacDescriptor,
        src,                            // move data from here
        (void *)(&I2S->DATA[0].reg),    // to here (M0+)
        STEREO_BUFFER_SAMPLES);         // this many...

    audioDMA.startJob();
}

I2SAudio::I2SAudio(Adafruit_ZeroI2S& _i2s, Adafruit_ZeroTimer& _timer, Adafruit_ZeroDMA& _dma) : 
    i2s(_i2s),
    timer(_timer),
    audioDMA(_dma)
{
    _instance = this;
    audioBufferData[0].buffer = audioBuffer0;
    audioBufferData[1].buffer = audioBuffer1;
}


void I2SAudio::initStream(SPIStream& spiStream)
{
    spiStream.init(0, 0);
    expander.init(&spiStream, 0, 0);
}

void I2SAudio::init()
{
    audioBufferData[0].reset();
    audioBufferData[1].reset();

    // Clock: 48 000 000 / second
    // div 1024 = 46875 / second
    // 
    timer.configure( TC_CLOCK_PRESCALER_DIV1024,    // prescaler
                  TC_COUNTER_SIZE_16BIT,         // bit width of timer/counter
                  TC_WAVE_GENERATION_MATCH_PWM   // frequency or PWM mode
                 );

    static const uint32_t CLOCK = (48*1000*1000) / 1024;         // ticks / second
    static const uint32_t HZ = 1000 / MSEC_PER_AUDIO_BUFFER;
    static const uint32_t COUNTER = CLOCK / (HZ * 4);
    Log.p("MSec/Buffer=").p(MSEC_PER_AUDIO_BUFFER).p(" Counter=").p(COUNTER).eol();
    timer.setCompare(0, COUNTER);

    timer.setCallback(true, TC_CALLBACK_CC_CHANNEL0, I2SAudio::timerCallback);  // this one sets pin low
    timer.enable(true);

    audioBufferData[0].reset();
    audioBufferData[1].reset();
    audioBufferData[0].fillBuffer(expander, expandVolume());

    Log.p("Configuring DMA trigger").eol();
    audioDMA.setTrigger(I2S_DMAC_ID_TX_0);
    audioDMA.setAction(DMA_TRIGGER_ACTON_BEAT);

    ZeroDMAstatus stat = audioDMA.allocate();
    audioDMA.printStatus(stat);

    Log.p("Setting up transfer").eol();
    dmacDescriptor = audioDMA.addDescriptor(
        audioBuffer0,                   // move data from here
        (void *)(&I2S->DATA[0].reg),    // to here (M0+)
        STEREO_BUFFER_SAMPLES,          // this many...
        DMA_BEAT_SIZE_WORD,             // bytes/hword/words
        true,                           // increment source addr?
        false);

    Log.p("Adding callback").eol();
    audioDMA.setCallback(I2SAudio::dmaCallback);

    i2s.begin(I2S_32_BIT, AUDIO_FREQ);
    i2s.enableTx();

    stat = audioDMA.startJob();
}


bool I2SAudio::play(const char* filename)
{
    Log.p("play").eol();
    return true;
}


void I2SAudio::stop()
{
    Log.p("stop").eol();
}


bool I2SAudio::isPlaying()
{
    Log.p("isPlaying").eol();
    return true;
}



int AudioBufferData::fillBuffer(wav12::Expander& expander, int32_t volume)
{
    if (status != AUDBUF_EMPTY) {
        return AUDERROR_BUFFER_NOT_EMPTY;
    }

    status = AUDBUF_FILLING;

    uint32_t MILLION2 = 2 * 1024 * 1024;
    if (expander.samples() < expander.pos()) return AUDERROR_SAMPLES_POS_OUT_OF_RANGE;
    if (expander.samples() > MILLION2 || expander.pos() > MILLION2) return AUDERROR_SAMPLES_POS_OUT_OF_RANGE;

    uint32_t toRead = glMin(expander.samples() - expander.pos(), (uint32_t)AUDIO_BUFFER_SAMPLES);
    if (toRead)
        expander.expand2(buffer, toRead, volume);

    for(int i=toRead*2; i<STEREO_BUFFER_SAMPLES; ++i) {
        buffer[i] = 0;
    }
    status = AUDBUF_READY;
    return AUDERROR_NONE;
}


uint32_t SPIStream::fetch(uint8_t* target, uint32_t nBytes)
{
    uint32_t r = m_flash.readBuffer(
        m_addr + m_pos, 
        target,
        nBytes);
    m_pos += r;
    return r;
}
