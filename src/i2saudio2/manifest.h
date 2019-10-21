#pragma once

#include "Grinliz_Util.h"

class Adafruit_SPIFlash;

struct MemUnit {
    static const int NAME_LEN = 8;

    CStrBuf<NAME_LEN> name;
    uint32_t offset;
    uint32_t size;
};

static_assert(sizeof(MemUnit) == 16, "MemUnit isn't the correct size.");

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

