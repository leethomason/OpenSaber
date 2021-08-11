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

#include "./src/util/Grinliz_Util.h"
#include "./src/util/interface.h"

struct MemUnit {
    static const int NAME_LEN = 8;

    CStrBuf<NAME_LEN> name;   // NOT null terminated, but 0-filled.
    uint32_t offset;
    uint32_t size : 24;
    uint32_t is8Bit : 1;
    uint32_t table : 2;
    uint32_t reserve : 5;

    uint32_t numSamples() const { return is8Bit ? size : size * 2; }
    uint32_t timeInMSec() const {
        return numSamples() * 1000 / 22050;
    }
    CStr<10> getName() const {
        CStr<10> n;
        name.toStr(&n);
        return n;
    }
};

static_assert(sizeof(MemUnit) == 16, "16 byte MemUnit");

enum {
    MEM_IMAGE_NUM_DIR   = 4,
    MEM_IMAGE_NUM_FILES = 60,
    MEM_IMAGE_TOTAL = MEM_IMAGE_NUM_DIR + MEM_IMAGE_NUM_FILES
};


class Manifest
{
    public:
        Manifest();
        // Only scan interacts with the SPI. Afterwards, everything is cached locally.
        void scan(IMemory* flash);
        uint32_t dirHash() const;
        const MemUnit& getUnit(int id) const;
        
        int getDir(const char* dir) const;
        void dirRange(int dir, int* start, int* count) const;
        int getFile(int dir, const char* fname) const;

    private:        
        int find(const char* name, int start, int n) const;

        MemUnit memUnit[MEM_IMAGE_TOTAL];
};

