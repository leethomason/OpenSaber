#ifndef MC_AUDIO_DEFINED
#define MC_AUDIO_DEFINED

#include <stdint.h>
#include "compress.h"
#include "iaudio.h"
#include "Grinliz_Util.h"

// Debugging:
//#define USE_DECODE
//#define USE_AUDIO

// 1 or 4
#ifndef NUM_AUDIO_CHANNELS
#   define NUM_AUDIO_CHANNELS 1
#endif

#define AUDIO_FREQ 22050
#define AUDIO_BUFFER_SAMPLES 384
#define STEREO_BUFFER_SAMPLES (AUDIO_BUFFER_SAMPLES*2)
#define MSEC_PER_AUDIO_BUFFER (1000 * AUDIO_BUFFER_SAMPLES / AUDIO_FREQ)
#define MICRO_PER_AUDIO_BUFFER (1000 * MSEC_PER_AUDIO_BUFFER)
#define NUM_AUDIO_BUFFERS 2

class Adafruit_SPIFlash;
class Adafruit_ZeroI2S;
class Adafruit_ZeroDMA;
class SPIStream;

enum {
    AUDERROR_NONE,
    AUDERROR_READING_SPI,
    AUDERROR_SAMPLES_POS_OUT_OF_RANGE,
};

struct AudioBufferData {
    int32_t* buffer = 0;    
    int fillBuffer(wav12::ExpanderAD4& expander, int32_t volume, bool loop, bool add);
};

struct I2STracker
{
    uint32_t timerCalls;
    uint32_t timerQueued;
    uint32_t timerErrors;

    uint32_t dmaCalls;
    uint32_t dmaErrors;
    uint32_t dmaMicros; // microseconds in the DMA & fills. Can compute utilization w/ dmaCalls.
                        // Keep in mind the calls & micros aren't written atomically

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
    I2SAudio(Adafruit_ZeroI2S& i2s, Adafruit_ZeroDMA& dma, Adafruit_SPIFlash& spiFlash);

    virtual void init();
    // Initialize the streams before the init() call.
    void initStream(wav12::IStream* stream, int channel=0) { iStream[clamp(channel, 0, NUM_AUDIO_CHANNELS-1)] = stream;}

    bool isInitialized() const { return _instance != 0; }

    bool play(int fileIndex, bool loop, int channel);
    virtual bool play(const char* filename, bool loop, int channel);
    virtual bool canLoop() const { return true; }

    virtual void stop(int channel);
    virtual bool isPlaying(int channel) const;

    virtual void process();     // checks for errors
    void dumpStatus();

    // Volume 256 is "full" - can boost or cut from there.
    virtual void setVolume(int v, int channel) { volume256[clamp(channel, 0, NUM_AUDIO_CHANNELS-1)] = v; }
    virtual int volume(int channel) const { return volume256[clamp(channel, 0, NUM_AUDIO_CHANNELS-1)]; }

    void testReadRate(int index);

    static I2STracker tracker;
    static I2SAudio* instance() { return _instance; }

private:
    struct ChangeReq
    {
        bool     isQueued;
        uint32_t addr;
        uint32_t size;
        uint32_t nSamples;
        int      format;
        bool     loop;

        void reset() {
            isQueued = false;
            addr = size = nSamples = 0;
            format = 0;
            loop = false;
        }
    };

    int32_t expandVolume(int channel) const { 
        channel = clamp(channel, 0, NUM_AUDIO_CHANNELS-1);
        return this->volume(channel) * 256; 
    }

    static I2SAudio* _instance;
    static void dmaCallback(Adafruit_ZeroDMA *dma);
    static void outerFill(int id);

    static AudioBufferData audioBufferData[NUM_AUDIO_BUFFERS];     // Information about the state of audioBuffer0/1
    static int32_t audioBuffer0[STEREO_BUFFER_SAMPLES];
    static int32_t audioBuffer1[STEREO_BUFFER_SAMPLES];

    // Access from interupts disabled.
    static ChangeReq changeReq[NUM_AUDIO_CHANNELS];
    // end interupt section

    Adafruit_ZeroI2S&   i2s;
    Adafruit_ZeroDMA&   audioDMA;  
    Adafruit_SPIFlash&  spiFlash;
    wav12::IStream*     iStream[NUM_AUDIO_CHANNELS];
    uint32_t            lastLogTime = 0;

    // these are access in interupts. Assuming atomic read on M0 (?) seems okay.
    int volume256[NUM_AUDIO_CHANNELS];    
    int looping[NUM_AUDIO_CHANNELS];      // 0 or 1
};


class SPIStream : public wav12::IStream
{
public:
    SPIStream(Adafruit_SPIFlash& flash) : m_flash(flash) {}

    virtual void set(uint32_t addr, uint32_t size);
    virtual uint32_t fetch(uint8_t* target, uint32_t nBytes);
    virtual void rewind();
    virtual bool done() const;

private:
    Adafruit_SPIFlash& m_flash;
    uint32_t m_addr = 0;
    uint32_t m_size = 0;
    uint32_t m_pos = 0;
};

#endif // MC_AUDIO_DEFINED
