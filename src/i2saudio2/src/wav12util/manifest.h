/*
  Copyright (c) Lee Thomason, Grinning Lizard Software

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
  of the Software, and to permit persons to whom the Software is furnished to do
  so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#pragma once

#include <stdint.h>
#include <cstddef>

struct MemUnit {
    static constexpr int NAME_LEN = 8;
    static constexpr int NAME_ALLOC = NAME_LEN + 1; // not used here, but amount to allocate if you need to null-terminate

    char name[NAME_LEN];   // NOT null terminated, but 0-filled.
    uint32_t offset;
    uint32_t size : 24;     // if needed, an extra sample is added so that size==nSamples
    uint32_t table : 4;     // 0-15 to select table

    uint32_t numSamples() const { return size * 2; }
    uint32_t timeInMSec() const {
        return numSamples() * 100 / 2205;
    }
    bool nameMatch(const char* n) const;
    uint32_t nameHash(uint32_t h) const;
};

static_assert(sizeof(MemUnit) == 16, "16 byte MemUnit");

struct MemPalette {
    static constexpr int NUM_PALETTES = 8;
    struct RGB {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
    
    uint8_t soundFont = 0;
    RGB bladeColor = { 0, 0, 0 };
    RGB impactColor = { 0, 0, 0 };
};

struct MemImage {
    static const int NUM_DIR = 4;
    static const int NUM_FILES = 96 - NUM_DIR;
    static const int NUM_MEMUNITS = NUM_DIR + NUM_FILES;

    static const size_t SIZE_DESC = 64;                                                 // null filled terminated string. Follows the MemUnits.
    static const size_t SIZE_PALETTE = MemPalette::NUM_PALETTES * sizeof(MemPalette);   // palettes - follows the description
    
    static const size_t SIZE_MEMUNITS = NUM_MEMUNITS * sizeof(MemUnit);
    static const size_t SIZE_BASE = SIZE_MEMUNITS + SIZE_DESC + SIZE_PALETTE;

    MemUnit unit[NUM_MEMUNITS];
};


class Manifest
{
    public:
        Manifest();
        // Used by the SPI readed to initialize the block of memory.
        MemImage* getBasePtr() { return &image; }
        static uint32_t DescAddr() { return MemImage::SIZE_MEMUNITS; }
        static uint32_t PaletteAddr(int index) { return MemImage::SIZE_MEMUNITS + MemImage::SIZE_DESC + index * sizeof(MemPalette); }

        const MemUnit& getUnit(int id) const;
        
        int getDir(const char* dir) const;
        int getFile(int dir, const char* fname) const;
        int getFile(const char* dir, const char* fname) const {
            return getFile(getDir(dir), fname);
        }

        void dirRange(int dir, int* start, int* count) const;
        static bool Test();

    private:        
        int find(const char* name, int start, int n) const;

        MemImage image;
};

