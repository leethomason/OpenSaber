#pragma once

#include "Grinliz_Util.h"

class Adafruit_SPIFlash;

struct MemUnit {
    static const int NAME_LEN = 8;

    CStrBuf<NAME_LEN> name;   // NOT null terminated, but 0-filled.
    uint32_t offset;
    uint32_t size : 24;
    uint32_t is8Bit : 1;
    uint32_t reserve : 7;

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
        void scan(Adafruit_SPIFlash* flash);

        const MemUnit& getUnit(int id) const;
        
        int getDir(const char* dir) const;
        void dirRange(int dir, int* start, int* count) const;
        int getFile(int dir, const char* fname) const;

    private:        
        int find(const char* name, int start, int n) const;

        MemUnit memUnit[MEM_IMAGE_TOTAL];
};

