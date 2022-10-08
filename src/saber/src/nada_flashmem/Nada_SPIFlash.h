#pragma once

// for debugging
#define SPIFLASH_DEBUG 0

#include <stdint.h>

#include "Nada_FlashTransport.h"
#include "nada_flash_devices.h"
#include "../wav12/interface.h"

/// Constant that is (mostly) true to all external flash devices
enum
{
    SFLASH_BLOCK_SIZE = 64 * 1024,
    SFLASH_SECTOR_SIZE = 4 * 1024,
    SFLASH_PAGE_SIZE = 256,
};

// An easy to use interface for working with Flash memory.
class Nada_SPIFlash : public IMemory
{
public:
    Nada_SPIFlash(Nada_FlashTransport *transport);
    bool begin();

    uint16_t numPages();
    uint16_t pageSize();

    uint32_t size();

    uint8_t readStatus();
    uint8_t readStatus2();
    void waitUntilReady();
    bool writeEnable();

    uint32_t getJEDECID();

    uint32_t readBuffer(uint32_t address, uint8_t *buffer, uint32_t len);
    uint32_t writeBuffer(uint32_t address, uint8_t const *buffer, uint32_t len);

    bool eraseSector(uint32_t sectorNumber);
    bool eraseBlock(uint32_t blockNumber);
    bool eraseChip();

    // Helper
    uint8_t read8(uint32_t addr);
    uint16_t read16(uint32_t addr);
    uint32_t read32(uint32_t addr);

    virtual void readMemory(uint32_t addr, uint8_t *data, uint32_t len) override {
        readBuffer(addr, data, len);
    }
    virtual int32_t memorySize() override { return size(); }

private:
    Nada_FlashTransport *_trans;
    SPIFlash_Device_t const *_flash_dev;
};
