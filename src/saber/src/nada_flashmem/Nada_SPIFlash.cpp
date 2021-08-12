#include "Nada_SPIFlash.h"
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>

#if SPIFLASH_DEBUG
#define SPIFLASH_LOG(_address, _count) \
    do                                 \
    {                                  \
        Serial.print(__FUNCTION__);    \
        Serial.print(": adddress = "); \
        Serial.print(_address, HEX);   \
        if (_count)                    \
        {                              \
            Serial.print(" count = "); \
            Serial.print(_count);      \
        }                              \
        Serial.println();              \
    } while (0)
#else
#define SPIFLASH_LOG(_sector, _count)
#endif

/// List of all possible flash devices used by Adafruit boards
static const SPIFlash_Device_t possible_devices[] = {
    // Main devices used in current Adafruit products
    GD25Q16C,
    GD25Q64C,
    S25FL116K,
    S25FL216K,

    // Only a handful of production run
    W25Q16FW,
    W25Q64JV_IQ,

    // Nordic PCA10056
    MX25R6435F,

    // Other common flash devices
    W25Q16JV_IQ,
};

/// Flash device list count
enum
{
    EXTERNAL_FLASH_DEVICE_COUNT =
        sizeof(possible_devices) / sizeof(possible_devices[0])
};

Nada_SPIFlash::Nada_SPIFlash(Nada_FlashTransport *transport)
{
    _trans = transport;
    _flash_dev = NULL;
}

static SPIFlash_Device_t const *findDevice(SPIFlash_Device_t const *device_list,
                                           int count,
                                           uint8_t const (&jedec_ids)[3])
{
    for (uint8_t i = 0; i < count; i++)
    {
        const SPIFlash_Device_t *dev = &device_list[i];
        if (jedec_ids[0] == dev->manufacturer_id &&
            jedec_ids[1] == dev->memory_type && // comment to appease format check
            jedec_ids[2] == dev->capacity)
        {
            return dev;
        }
    }
    return NULL;
}

bool Nada_SPIFlash::begin()
{
    if (_trans == NULL)
        return false;

    _trans->begin();

    //------------- flash detection -------------//
    uint8_t jedec_ids[3];
    _trans->readCommand(SFLASH_CMD_READ_JEDEC_ID, jedec_ids, 3);

    _flash_dev = findDevice(possible_devices, EXTERNAL_FLASH_DEVICE_COUNT, jedec_ids);
    if (_flash_dev == NULL) {
        #if SPIFLASH_DEBUG
            Serial.println("ERROR Nada SPI Flash: device not found.");
        #endif
        return false;
    }

    // We don't know what state the flash is in so wait for any remaining writes
    // and then reset.

    // The write in progress bit should be low.
    while (readStatus() & 0x01) { }

    // The suspended write/erase bit should be low.
    while (readStatus2() & 0x80) { }

    _trans->runCommand(SFLASH_CMD_ENABLE_RESET);
    _trans->runCommand(SFLASH_CMD_RESET);

    // Wait 30us for the reset
    delayMicroseconds(30);

    // Speed up to max device frequency, or as high as possible
    _trans->setClockSpeed(min(
        (uint32_t)(_flash_dev->max_clock_speed_mhz * 1000000U), (uint32_t)F_CPU));

    // Turn off writes in case this is a microcontroller only reset.
    _trans->runCommand(SFLASH_CMD_WRITE_DISABLE);

    waitUntilReady();

    return true;
}

uint32_t Nada_SPIFlash::size(void)
{
    return _flash_dev ? _flash_dev->total_size : 0;
}

uint16_t Nada_SPIFlash::numPages(void)
{
    return _flash_dev ? _flash_dev->total_size / pageSize() : 0;
}

uint16_t Nada_SPIFlash::pageSize(void) { return SFLASH_PAGE_SIZE; }

uint32_t Nada_SPIFlash::getJEDECID(void)
{
    return (_flash_dev->manufacturer_id << 16) | (_flash_dev->memory_type << 8) |
           _flash_dev->capacity;
}

uint8_t Nada_SPIFlash::readStatus()
{
    uint8_t status;
    _trans->readCommand(SFLASH_CMD_READ_STATUS, &status, 1);
    return status;
}

uint8_t Nada_SPIFlash::readStatus2(void)
{
    uint8_t status;
    _trans->readCommand(SFLASH_CMD_READ_STATUS2, &status, 1);
    return status;
}

void Nada_SPIFlash::waitUntilReady(void)
{
    // both WIP and WREN bit should be clear
    while (readStatus() & 0x03)
        yield();
}

bool Nada_SPIFlash::writeEnable(void)
{
    return _trans->runCommand(SFLASH_CMD_WRITE_ENABLE);
}

bool Nada_SPIFlash::eraseSector(uint32_t sectorNumber)
{
    if (!_flash_dev)
        return false;

    // Before we erase the sector we need to wait for any writes to finish
    waitUntilReady();
    writeEnable();

    SPIFLASH_LOG(sectorNumber * SFLASH_SECTOR_SIZE, 0);

    return _trans->eraseCommand(SFLASH_CMD_ERASE_SECTOR,
                                sectorNumber * SFLASH_SECTOR_SIZE);
}

bool Nada_SPIFlash::eraseBlock(uint32_t blockNumber)
{
    if (!_flash_dev)
        return false;

    // Before we erase the sector we need to wait for any writes to finish
    waitUntilReady();
    writeEnable();

    return _trans->eraseCommand(SFLASH_CMD_ERASE_BLOCK,
                                blockNumber * SFLASH_BLOCK_SIZE);
}

bool Nada_SPIFlash::eraseChip(void)
{
    if (!_flash_dev)
        return false;

    // We need to wait for any writes to finish
    waitUntilReady();
    writeEnable();

    return _trans->runCommand(SFLASH_CMD_ERASE_CHIP);
}

uint32_t Nada_SPIFlash::readBuffer(uint32_t address, uint8_t *buffer,
                                           uint32_t len)
{
    if (!_flash_dev)
        return 0;

    waitUntilReady();

    SPIFLASH_LOG(address, len);

    _trans->readMemory(address, buffer, len);
    return len;
}

uint8_t Nada_SPIFlash::read8(uint32_t addr)
{
    uint8_t ret = 0;
    return readBuffer(addr, &ret, sizeof(ret)) ? ret : 0xff;
}

uint16_t Nada_SPIFlash::read16(uint32_t addr)
{
    uint16_t ret = 0;
    return readBuffer(addr, (uint8_t *)&ret, sizeof(ret)) ? ret : 0xffff;
}

uint32_t Nada_SPIFlash::read32(uint32_t addr)
{
    uint32_t ret = 0;
    return readBuffer(addr, (uint8_t *)&ret, sizeof(ret)) ? ret : 0xffffffff;
}

uint32_t Nada_SPIFlash::writeBuffer(uint32_t address,
                                            uint8_t const *buffer,
                                            uint32_t len)
{
    if (!_flash_dev)
        return 0;

    SPIFLASH_LOG(address, len);

    uint32_t remain = len;

    // write one page at a time
    while (remain)
    {
        waitUntilReady();
        writeEnable();

        uint32_t const leftOnPage =
            SFLASH_PAGE_SIZE - (address & (SFLASH_PAGE_SIZE - 1));

        uint32_t const toWrite = min(remain, leftOnPage);

        if (!_trans->writeMemory(address, buffer, toWrite))
            break;

        remain -= toWrite;
        buffer += toWrite;
        address += toWrite;
    }

    return len - remain;
}
