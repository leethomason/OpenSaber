#ifndef MC_AUDIO_DEFINED
#define MC_AUDIO_DEFINED

#include <stdint.h>
#include "compress.h"
#include "iaudio.h"

#define AUDIO_FREQ 22050
#define AUDIO_BUFFER_SAMPLES 512
#define STEREO_BUFFER_SAMPLES (AUDIO_BUFFER_SAMPLES*2)
#define MSEC_PER_AUDIO_BUFFER (1000 * AUDIO_BUFFER_SAMPLES / AUDIO_FREQ)
#define AUDIO_SUB_BUFFER 512
#define NUM_AUDIO_BUFFERS 2

class Adafruit_SPIFlash;
class Adafruit_ZeroTimer;
class Adafruit_ZeroI2S;
class Adafruit_ZeroDMA;
class SPIStream;

enum {
    AUDBUF_EMPTY,
    AUDBUF_FILLING,
    AUDBUF_DRAINING,
    AUDBUF_READY
};

enum {
    AUDERROR_NONE,
    AUDERROR_READING_SPI,
    AUDERROR_BUFFER_NOT_EMPTY,
    AUDERROR_SAMPLES_POS_OUT_OF_RANGE,
};

struct AudioBufferData {
    uint8_t status = AUDBUF_EMPTY;
    uint32_t dataAvailable = 0;
    int32_t* buffer = 0;
    
    int fillBuffer(wav12::Expander& expander, int32_t volume);

    void reset() { status = AUDBUF_EMPTY; dataAvailable = 0; }
};

struct I2STracker
{
    uint32_t timerCalls;
    uint32_t timerFills;
    uint32_t timerQueued;
    uint32_t timerErrors;

    uint32_t dmaCalls;
    uint32_t dmaErrors;

    uint32_t fillEmpty;
    uint32_t fillSome;
    uint32_t fillErrors;
    uint32_t fillCritErrors;

    void reset() {
        memset(this, 0, sizeof(*this));
    }
    bool hasErrors() const {
        return timerErrors || dmaErrors || fillErrors || fillCritErrors;
    }
};


class I2SAudio : public IAudio
{
public:
    I2SAudio(Adafruit_ZeroI2S& i2s, Adafruit_ZeroTimer& timer, Adafruit_ZeroDMA& dma, Adafruit_SPIFlash& spiFlash, SPIStream& stream);

    virtual void init();
    bool isInitialized() const { return _instance != 0; }

    bool play(int fileIndex);
    virtual bool play(const char* filename);

    virtual void stop();
    virtual bool isPlaying() const;

    virtual void process();

    // Volume 256 is "full" - can boost or cut from there.
    virtual void setVolume(int v) { volume256 = v; }
    virtual int volume() const { return volume256; }

    void testReadRate(int index);

    static I2STracker tracker;
    static I2SAudio* instance() { return _instance; }

private:
    int32_t expandVolume() const { return this->volume() * 256; }

    static I2SAudio* _instance;
    static void dmaCallback(Adafruit_ZeroDMA *dma);
    static void timerCallback();

    static AudioBufferData audioBufferData[NUM_AUDIO_BUFFERS];     // Information about the state of audioBuffer0/1
    static int32_t audioBuffer0[STEREO_BUFFER_SAMPLES];
    static int32_t audioBuffer1[STEREO_BUFFER_SAMPLES];

    // Access from interupts disabled.
    static bool isStreamQueued;
    static uint32_t queued_addr;
    static uint32_t queued_size;
    static uint32_t queued_nSamples;
    static int      queued_format;
    // end interupt section

    Adafruit_ZeroI2S&   i2s;
    Adafruit_ZeroTimer& timer;
    Adafruit_ZeroDMA&   audioDMA;  
    Adafruit_SPIFlash&  spiFlash;
    SPIStream&          spiStream;

    int volume256 = 256;
};


class SPIStream : public wav12::IStream
{
public:
    SPIStream(Adafruit_SPIFlash& flash) : m_flash(flash) { init(0, 0); }

    void init(uint32_t addr, uint32_t size) {
        m_addr = addr;
        m_size = size;
        m_pos = 0;
    }

    virtual uint32_t fetch(uint8_t* target, uint32_t nBytes);

private:
    Adafruit_SPIFlash& m_flash;
    uint32_t m_addr = 0;
    uint32_t m_size = 0;
    uint32_t m_pos = 0;
};

#endif // MC_AUDIO_DEFINED
