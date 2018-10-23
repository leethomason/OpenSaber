#ifndef MC_MEM_IMAGE
#define MC_MEM_IMAGE

#include <stdint.h>
#include "Grinliz_Util.h"
#include "compress.h"

class Adafruit_SPIFlash;

struct MemUnit {
    static const int NAME_LEN = 8;

    CStrBuf<NAME_LEN> name;
    uint32_t offset;
    uint32_t size;
};

static_assert(sizeof(MemUnit) == 16, "correct size");

struct MemImage {
    static const int NUM_DIR = 4;
    static const int NUM_FILES = 60;
};

void readDir(Adafruit_SPIFlash&, MemUnit* dir);
void readFile(Adafruit_SPIFlash& flash, int index, MemUnit* file);
void readAudioInfo(Adafruit_SPIFlash& flash, const MemUnit& file, wav12::Wav12Header* header, uint32_t* dataAddr);

void dumpImage(Adafruit_SPIFlash&);

#endif // MC_MEM_IMAGE 
