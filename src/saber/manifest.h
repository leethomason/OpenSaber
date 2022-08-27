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

#include "../util/interface.h"

struct MemUnit {
    static const int NAME_LEN = 8;

    char name[NAME_LEN];   // NOT null terminated, but 0-filled.
    uint32_t offset;
    uint32_t size : 24;     // if needed, an extra sample is added so that size==nSamples
    uint32_t table : 4;     // 0-15 to select table

    uint32_t numSamples() const { return size * 2; }
    bool nameMatch(const char* n) const;
    uint32_t nameHash(uint32_t h) const;
};

static_assert(sizeof(MemUnit) == 16, "16 byte MemUnit");

struct ConfigUnit {
    static const int NUM_CONFIG = 8;
    char name[MemUnit::NAME_LEN];
    uint8_t soundFont, bc_r, bc_b, bc_g;
    uint8_t ic_r, ic_b, ic_g, reserve;
};

static_assert(sizeof(ConfigUnit) == 16, "16 byte ConfigUnit");
static_assert(sizeof(ConfigUnit) == sizeof(MemUnit), "MemUnit and ConfigUnit should be the same size");


struct MemImage {
    static const int NUM_DIR = 4;
    static const int NUM_FILES = 60;
    static const int NUM = NUM_DIR + NUM_FILES;
    static const size_t SIZE = NUM * sizeof(MemUnit);

    MemUnit unit[NUM];
};


class Manifest
{
    public:
        Manifest();
        // Used by the SPI readed to initialize the block of memory.
        MemImage* getBasePtr() { return &image; }

        uint32_t dirHash() const;
        const MemUnit& getUnit(int id) const;
        const ConfigUnit& getConfig(int id) const;
        
        int getDir(const char* dir) const;
        int getFile(int dir, const char* fname) const;
        int getFile(const char* dir, const char* fname) const {
            return getFile(getDir(dir), fname);
        }

        static bool Test();

    private:        
        void dirRange(int dir, int* start, int* count) const;
        int find(const char* name, int start, int n) const;

        MemImage image;
};

