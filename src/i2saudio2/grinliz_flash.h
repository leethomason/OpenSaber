#ifndef GRINLIZ_FLASH
#define GRINLIZ_FLASH

#include <stdint.h>

#include <Arduino.h>
#include <SPI.h>
#include "interface.h"

class GrinlizFlash : public IMemory
{
public:
    static const int ADDRESS_LEN = 3; 
    static const uint32_t MEM_CLOCK = 104 * uint32_t(1000000);
    static const int32_t SIZE = 2048 * 1024;
    static const int32_t NUM_PAGES = 8192;
    static const int32_t PAGE_SIZE = 256;

    GrinlizFlash(uint8_t ss, SPIClass *spiinterface);

    void begin(void);
    int32_t numPages() const { return NUM_PAGES; }
    int32_t pageSize() const { return PAGE_SIZE; }
    uint32_t getJEDECID();

    virtual bool readMemory(uint32_t addr, uint8_t *data, uint32_t len) override;
    virtual int32_t memorySize() override { return SIZE; }

    void resetTimer() { _nCalls = 0; _callTimeMicros = 0; _nBytesRead = 0; }
    uint32_t nCalls() const { return _nCalls; }
    // Only works when not on interrupt!!
    uint32_t bandwidthKB() const { return _callTimeMicros ? _nBytesRead * 1000U / _callTimeMicros : 0; }

private:
    bool runCommand(uint8_t command);
    bool readCommand(uint8_t command, uint8_t *response, uint32_t len);
    uint8_t readCommand(uint8_t command);
    bool writeCommand(uint8_t command, uint8_t const *data, uint32_t len);
    void waitUntilReady();

    void beginTransaction(uint32_t clock_hz)
    {
        digitalWrite(_ss, LOW);
        _spi->beginTransaction(SPISettings(clock_hz, MSBFIRST, SPI_MODE0));
    }

    void endTransaction(void)
    {
        _spi->endTransaction();
        digitalWrite(_ss, HIGH);
    }

    void fillAddress(uint8_t *buf, uint32_t addr);

    uint8_t _cmd_read;
    uint8_t _ss;
    uint32_t _clock_wr, _clock_rd;
    uint32_t _nCalls = 0;
    uint32_t _callTimeMicros = 0;
    uint32_t _nBytesRead = 0;
    SPIClass *_spi;
};

#endif
