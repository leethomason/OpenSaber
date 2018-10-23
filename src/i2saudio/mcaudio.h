#ifndef MC_AUDIO_DEFINED
#define MC_AUDIO_DEFINED

#include <stdint.h>
#include "compress.h"

#define AUDIO_FREQ 22050
#define AUDIO_BUFFER_SAMPLES 512
#define STEREO_BUFFER_SAMPLES (AUDIO_BUFFER_SAMPLES*2)
#define MSEC_PER_AUDIO_BUFFER (1000 * AUDIO_BUFFER_SAMPLES / AUDIO_FREQ)
#define AUDIO_SUB_BUFFER 512
#define NUM_AUDIO_BUFFERS 2

class Adafruit_SPIFlash;

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
    
    int fillBuffer(Adafruit_SPIFlash& flash, wav12::Expander& expander, int32_t volume);

    void reset() { status = AUDBUF_EMPTY; dataAvailable = 0; }
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



