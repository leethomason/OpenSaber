#include "mcaudio.h"
#include "Grinliz_Util.h"
#include "compress.h"
#include <Adafruit_SPIFlash.h>

int AudioBufferData::fillBuffer(Adafruit_SPIFlash& flash, wav12::Expander& expander, int32_t volume)
{
    if (status != AUDBUF_EMPTY) {
        return AUDERROR_BUFFER_NOT_EMPTY;
    }

    status = AUDBUF_FILLING;

    uint32_t MILLION2 = 2 * 1024 * 1024;
    if (expander.samples() < expander.pos()) return AUDERROR_SAMPLES_POS_OUT_OF_RANGE;
    if (expander.samples() > MILLION2 || expander.pos() > MILLION2) return AUDERROR_SAMPLES_POS_OUT_OF_RANGE;

    uint32_t toRead = glMin(expander.samples() - expander.pos(), (uint32_t)AUDIO_BUFFER_SAMPLES);
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
