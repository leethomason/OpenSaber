/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 hathach for Adafruit Industries
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

#ifndef ADAFRUIT_FLASHTRANSPORT_H_
#define ADAFRUIT_FLASHTRANSPORT_H_

#include <stdbool.h>
#include <stdint.h>

enum
{
    SFLASH_CMD_READ = 0x03,      // Single Read
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
};

class Nada_FlashTransport
{
public:
    virtual void begin(void) = 0;

    /// Set clock speed in hertz
    /// @param clock_hz clock speed in hertz
    virtual void setClockSpeed(uint32_t clock_hz) = 0;

    /// Execute a single byte command e.g Reset, Write Enable
    /// @param command command code
    /// @return true if success
    virtual bool runCommand(uint8_t command) = 0;

    /// Execute a command with response data e.g Read Status, Read JEDEC
    /// @param command    command code
    /// @param response   buffer to hold data
    /// @param len        number of bytes to read
    /// @return true if success
    virtual bool readCommand(uint8_t command, uint8_t *response,
                             uint32_t len) = 0;

    /// Execute a command with data e.g Write Status,
    /// @param command    command code
    /// @param data       writing data
    /// @param len        number of bytes to read
    /// @return true if success
    virtual bool writeCommand(uint8_t command, uint8_t const *data,
                              uint32_t len) = 0;

    /// Erase external flash by address
    /// @param command  can be sector erase (0x20) or block erase 0xD8
    /// @param address  adddress to be erased
    /// @return true if success
    virtual bool eraseCommand(uint8_t command, uint32_t address) = 0;

    /// Read data from external flash contents.
    /// @param addr       address to read
    /// @param buffer     buffer to hold data
    /// @param len        number of byte to read
    virtual void readMemory(uint32_t addr, uint8_t *buffer, uint32_t len) = 0;

    /// Write data to external flash contents, flash sector must be previously
    /// erased first. Typically it uses quad write command 0x32
    /// @param addr       address to read
    /// @param data       writing data
    /// @param len        number of byte to read
    /// @return true if success
    virtual bool writeMemory(uint32_t addr, uint8_t const *data,
                             uint32_t len) = 0;
};

#endif /* ADAFRUIT_FLASHTRANSPORT_H_ */
