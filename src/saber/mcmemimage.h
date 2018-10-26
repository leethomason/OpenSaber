#ifndef MC_MEM_IMAGE
#define MC_MEM_IMAGE

#include <stdint.h>
#include "Grinliz_Util.h"
#include "compress.h"

class Adafruit_SPIFlash;
class DirToIndex;

struct MemUnit {
    static const int NAME_LEN = 8;

    CStrBuf<NAME_LEN> name;
    uint32_t offset;
    uint32_t size;
};

static_assert(sizeof(MemUnit) == 16, "correct size");

enum {
    MEM_IMAGE_NUM_DIR = 4,
    MEM_IMAGE_NUM_FILES = 60,
    MEM_IMAGE_SIZE = 2 * 1024 * 1024,   // fixme: should pick up from definition somewhere?
    MEM_IMAGE_EEPROM = 1024
};

class DirToIndex
{
public:
    DirToIndex();
    bool isScanned() const { return dirHash[0] != 0; }
    void scan(Adafruit_SPIFlash&);
    
    int lookup(const char* path);

private:
    uint16_t dirHash[MEM_IMAGE_NUM_DIR];
    uint16_t fileHash[MEM_IMAGE_NUM_FILES];
    int dirStart[MEM_IMAGE_NUM_DIR];
};


class ConstMemImage {
public:
    static const int NUM_DIR = MEM_IMAGE_NUM_DIR;
    static const int NUM_FILES = MEM_IMAGE_NUM_FILES;

    ConstMemImage(Adafruit_SPIFlash&);
    void begin();

    void readDir(int index, MemUnit* dir) const;
    void readDir(const char* dirName) const;
    void readFile(int index, MemUnit* file) const;
    void readAduioInfo(int index, wav12::Wav12Header* header, uint32_t* dataAddr);

    int lookup(const char* path);

    int numDir() const { return m_numDir; }
    Adafruit_SPIFlash& getSPIFlash() const;

private:
    Adafruit_SPIFlash& m_spiFlash;
    int m_numDir;
    DirToIndex dirToIndex;
};

// Needs to be declared in the ino with the resources.
// Which is awkward.
extern ConstMemImage MemImage;

void readDir(Adafruit_SPIFlash&, int index, MemUnit* dir);
void readFile(Adafruit_SPIFlash& flash, int index, MemUnit* file);
void readAudioInfo(Adafruit_SPIFlash& flash, const MemUnit& file, wav12::Wav12Header* header, uint32_t* dataAddr);

void dumpImage(Adafruit_SPIFlash&);

#endif // MC_MEM_IMAGE 
