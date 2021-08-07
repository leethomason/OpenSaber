#include "grinliz_flash.h"

#define DEEP_DEBUG 0
#define PROFILE 1

enum
{
    SFLASH_CMD_READ = 0x03,      // Single Read
    SFLASH_CMD_FAST_READ = 0x0B, // Fast Read
    SFLASH_CMD_QUAD_READ = 0x6B, // 1 line address, 4 line data

    SFLASH_CMD_READ_JEDEC_ID = 0x9f,

    SFLASH_CMD_PAGE_PROGRAM = 0x02,
    SFLASH_CMD_QUAD_PAGE_PROGRAM = 0x32, // 1 line address, 4 line data

    SFLASH_CMD_READ_STATUS = 0x05,
    SFLASH_CMD_READ_STATUS2 = 0x35,

    SFLASH_CMD_WRITE_STATUS = 0x01,
    SFLASH_CMD_WRITE_STATUS2 = 0x31,

    SFLASH_CMD_ENABLE_RESET = 0x66,
    SFLASH_CMD_RESET = 0x99,

    SFLASH_CMD_WRITE_ENABLE = 0x06,
    SFLASH_CMD_WRITE_DISABLE = 0x04,

    SFLASH_CMD_ERASE_SECTOR = 0x20,
    SFLASH_CMD_ERASE_BLOCK = 0xD8,
    SFLASH_CMD_ERASE_CHIP = 0xC7,

    SFLASH_CMD_4_BYTE_ADDR = 0xB7,
    SFLASH_CMD_3_BYTE_ADDR = 0xE9,
};

/// Constant that is (mostly) true to all external flash devices
enum
{
    SFLASH_BLOCK_SIZE = 64 * 1024UL,
    SFLASH_SECTOR_SIZE = 4 * 1024,
    SFLASH_PAGE_SIZE = 256,
};

GrinlizFlash::GrinlizFlash(uint8_t ss, SPIClass *spiinterface)
{
    //_cmd_read = SFLASH_CMD_FAST_READ;   // 503 KB/sec. Crashes the audio driver. Hmm.
    _cmd_read = SFLASH_CMD_READ;          // 507-530 KB/sec WHAT???
    // https://electronics.stackexchange.com/questions/240500/what-is-the-difference-between-normal-read-and-fast-read-in-the-flash-a25l03/240506
    _ss = ss;
    _spi = spiinterface;
    _clock_wr = _clock_rd = min(MEM_CLOCK, (uint32_t)F_CPU);
#if DEEP_DEBUG
    Serial.print("F_CPU="); Serial.println(F_CPU);
    Serial.print("write speed="); Serial.println(_clock_wr);
    Serial.print("read speed="); Serial.println(_clock_rd);
#endif
}

void GrinlizFlash::begin(void)
{
    pinMode(_ss, OUTPUT);
    digitalWrite(_ss, HIGH);

    _spi->begin();

    // JEDEC 0xC84015
    uint32_t jedecid = getJEDECID();
    if (jedecid != 0xc84915)
    {
#if DEEP_DEBUG
        Serial.println("Bad jedec id");
#endif        
        return;
    }

#if DEEP_DEBUG
    Serial.println("Jedec okay");
#endif

    // Set up the initial state:
    while (readCommand(SFLASH_CMD_READ_STATUS) & 0x01)
    {
        yield();
    }

    // The suspended write/erase bit should be low.
    while (readCommand(SFLASH_CMD_READ_STATUS2) & 0x80)
    {
        yield();
    }

    runCommand(SFLASH_CMD_ENABLE_RESET);
    runCommand(SFLASH_CMD_RESET);

    // Wait 30us for the reset
    delayMicroseconds(30);

    runCommand(SFLASH_CMD_WRITE_DISABLE);
    waitUntilReady();
#if DEEP_DEBUG
    Serial.println("Memory ready");
#endif
}

uint32_t GrinlizFlash::getJEDECID()
{
    uint8_t id[4];
    readCommand(SFLASH_CMD_READ_JEDEC_ID, (uint8_t *)&id, 4);
    return ((uint32_t)(id[0] << 16) | (id[1] << 8) | id[2]);
}

void GrinlizFlash::waitUntilReady()
{
    while (readCommand(SFLASH_CMD_READ_STATUS) & 0x03)
    {
        yield();
    }
}

uint8_t GrinlizFlash::readCommand(uint8_t cmd)
{
    uint8_t status = 0;
    readCommand(cmd, &status, 1);
    return status;
}

bool GrinlizFlash::runCommand(uint8_t command)
{
    beginTransaction(_clock_wr);
    _spi->transfer(command);
    endTransaction();
    return true;
}

bool GrinlizFlash::readCommand(uint8_t command, uint8_t *response, uint32_t len)
{
    beginTransaction(_clock_rd);

    _spi->transfer(command);
    while (len--)
    {
        *response++ = _spi->transfer(0xFF);
    }

    endTransaction();
    return true;
}

bool GrinlizFlash::writeCommand(uint8_t command,
                                uint8_t const *data,
                                uint32_t len)
{
    beginTransaction(_clock_wr);

    _spi->transfer(command);
    while (len--)
    {
        (void)_spi->transfer(*data++);
    }

    endTransaction();
    return true;
}

void GrinlizFlash::fillAddress(uint8_t *buf, uint32_t addr)
{
    static_assert(ADDRESS_LEN == 3, "only works for ADDRESS_LEN=3");
    *buf++ = (addr >> 16) & 0xFF;
    *buf++ = (addr >> 8) & 0xFF;
    *buf++ = addr & 0xFF;
}

bool GrinlizFlash::readMemory(uint32_t addr, uint8_t *data, uint32_t len)
{
#if PROFILE
    uint32_t startTime = micros();
#endif

    beginTransaction(_clock_rd);

    uint8_t cmd_with_addr[6] = {_cmd_read};
    fillAddress(cmd_with_addr + 1, addr);

    // Fast Read has 1 extra dummy byte
    uint8_t const cmd_len = 1 + ADDRESS_LEN + (SFLASH_CMD_FAST_READ == _cmd_read ? 1 : 0);

    _spi->transfer(cmd_with_addr, cmd_len);
    //_spi->transfer(data, len);
    _spi->transfer(0, data, len, true); // blocking DMA mode. performance 2x. interesting.

    endTransaction();

#if PROFILE
    uint32_t endTime = micros();
    _callTimeMicros += (endTime - startTime);
    _nCalls += 1;
    _nBytesRead += len;
#endif

    return true;
}
