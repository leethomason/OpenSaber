#include "manifest.h"
#include <Adafruit_SPIFlash.h>

Manifest::Manifest()
{
    memset(memUnit, 0, sizeof(MemUnit) * MEM_IMAGE_TOTAL);
}

void Manifest::scan(Adafruit_SPIFlash* flash)
{
    flash->readBuffer(0, (uint8_t*) memUnit, sizeof(MemUnit) * MEM_IMAGE_TOTAL);
}

const MemUnit& Manifest::getUnit(int id) const
{
    id = glClamp(id, 0, MEM_IMAGE_TOTAL-1);
    return memUnit[id];
}

int Manifest::find(const char* name, int start, int n) const
{
    CStrBuf<MemUnit::NAME_LEN> strBuf = name;
    for(int i=0; i<n; ++i) {
        if (strBuf == memUnit[i+start].name) {
            return i + start;
        }
    }
    return -1;
}


int Manifest::getDir(const char* name) const
{
    int dir = find(name, 0, MEM_IMAGE_NUM_DIR);
    return dir;
}

void Manifest::dirRange(int dir, int* start, int* count) const
{
    dir = glClamp(dir, 0, MEM_IMAGE_NUM_DIR-1);
    *start = memUnit[dir].offset;   // gah. ugly. need to fix in the generator.
    *count = memUnit[dir].size;
}

int Manifest::getFile(int dir, const char* fname) const
{
    int start = 0;
    int count = 0;
    dirRange(dir, &start, &count);

    int file = find(fname, start, count);
    return file;
}

