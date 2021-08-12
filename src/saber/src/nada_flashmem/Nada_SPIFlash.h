/**
 * @file Adafruit_QSPI_Flash.h
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach and Dean Miller for Adafruit Industries LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef ADAFRUIT_SPIFLASHBASE_H_
#define ADAFRUIT_SPIFLASHBASE_H_

// for debugging
#define SPIFLASH_DEBUG 0

#include <stdint.h>
#include "../util/interface.h"
#include "Nada_FlashTransport.h"
#include "nada_flash_devices.h"

/// Constant that is (mostly) true to all external flash devices
enum
{
    SFLASH_BLOCK_SIZE = 64 * 1024,
    SFLASH_SECTOR_SIZE = 4 * 1024,
    SFLASH_PAGE_SIZE = 256,
};

// An easy to use interface for working with Flash memory.
//
// If you are managing allocation of the Flash space yourself, this is the
// class to use as it take very little RAM.
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

#endif /* ADAFRUIT_SPIFLASHBASE_H_ */
