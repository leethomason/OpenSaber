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

class ConstMemImage {
public:
    static const int NUM_DIR = 4;
    static const int NUM_FILES = 60;

    ConstMemImage(Adafruit_SPIFlash&);

    void readDir(int index, MemUnit* dir) const;
    void readFile(int index, MemUnit* file) const;

    int numDir() const;
    int firstFile(const MemUnit& dir) const;
    int numFiles(const MemUnit& dir) const 

    Adafruit_SPIFlash& getSPIFlash() const;

private:
    Adafruit_SPIFlash& spiFlash;
    int numDir;
};

// Needs to be declared in the ino with the resources.
// Which is awkward.
extern ConstMemImage MemImage;

void readDir(Adafruit_SPIFlash&, int index, MemUnit* dir);
void readFile(Adafruit_SPIFlash& flash, int index, MemUnit* file);
void readAudioInfo(Adafruit_SPIFlash& flash, const MemUnit& file, wav12::Wav12Header* header, uint32_t* dataAddr);

class DirToIndex
{
public:
    DirToIndex();

    void scan(Adafruit_SPIFlash&);
    int lookup(const char* path);

private:
    uint16_t dirHash[ConstMemImage::NUM_DIR];
    uint16_t fileHash[ConstMemImage::NUM_FILES];
    int dirStart[ConstMemImage::NUM_DIR];
};

void dumpImage(Adafruit_SPIFlash&);

#endif // MC_MEM_IMAGE 
