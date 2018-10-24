#include "mcmemimage.h"

#include <Adafruit_SPIFlash.h>

#include "Grinliz_Util.h"
#include "compress.h"

ConstMemImage::ConstMemImage(Adafruit_SPIFlash& _spiFlash) :
    spiFlash(_spiFlash)
{
    
}

void ConstMemImage::readDir(int index, MemUnit* dir) const
{
    ::readDir(spiFlash, index, dir);
}

void ConstMemImage::readFile(int index, MemUnit* file) const
{
    ::readFile(spiFlash, index, file);
}


Adafruit_SPIFlash& ConstMemImage::getSPIFlash() const
{
    return spiFlash;
}


void readDir(Adafruit_SPIFlash& flash, int index, MemUnit* dir)
{
    ASSERT(index >= 0 && index < ConstMemImage::NUM_DIR);

    flash.readBuffer(
        sizeof(MemUnit) * index, 
        (uint8_t*) dir, 
        sizeof(MemUnit)*ConstMemImage::NUM_DIR);
}

void readFile(Adafruit_SPIFlash& flash, int index, MemUnit* file)
{
    ASSERT(index >= 0 && index < ConstMemImage::NUM_FILES);

    flash.readBuffer(
        sizeof(MemUnit) * ConstMemImage::NUM_DIR + sizeof(MemUnit) * index,
        (uint8_t*) file,
        sizeof(MemUnit)
    );
}

void readAudioInfo(Adafruit_SPIFlash& flash, const MemUnit& file, wav12::Wav12Header* header, uint32_t* dataAddr)
{
    flash.readBuffer(
        file.offset,
        (uint8_t*) header,
        sizeof(wav12::Wav12Header)
    );
    *dataAddr = file.offset + sizeof(wav12::Wav12Header);
}

void dumpImage(Adafruit_SPIFlash& flash)
{
    MemUnit dir;

    Log.p("Memory image:").eol();
    for(int i=0; i<ConstMemImage::NUM_DIR; ++i) {
        readDir(flash, i, &dir);
        if (!dir.name.empty()) {
            Log.p("  ").pt(dir.name).eol();

            for(unsigned f=0; f<dir.size; ++f) {
                MemUnit file;
                int index = dir.offset + f;
                readFile(flash, index, &file);

                wav12::Wav12Header wavHeader;
                uint32_t wavAddr = 0;
                readAudioInfo(flash, file, &wavHeader, &wavAddr);

                Log.p("    ")
                    .pt(file.name).p(" [")
                    .p(index)
                    .p("] addr=").p(file.offset)
                    .p(" size=").p(file.size)
                    .p(" comp=").p(wavHeader.format)
                    .eol();
            }
        }
    }
}


DirToIndex::DirToIndex()
{
    memset(dirHash, 0, sizeof(uint16_t) * ConstMemImage::NUM_DIR + ConstMemImage::NUM_FILES);
    memset(fileHash, 0, sizeof(uint16_t) * ConstMemImage::NUM_FILES);
    memset(dirStart, 0, sizeof(int) * ConstMemImage::NUM_DIR);
}

void DirToIndex::scan(Adafruit_SPIFlash& flash)
{
    MemUnit dir;
    for(int i=0; i<ConstMemImage::NUM_DIR; ++i) {
        readDir(flash, i, &dir);
        dirHash[i] = dir.name.hash8();
        dirStart[i] = dir.offset;
    }
    for(int i=0; i<ConstMemImage::NUM_FILES; ++i) {
        MemUnit file;
        readFile(flash, i, &file);
        fileHash[i] = file.name.hash8();
    }
}


int DirToIndex::lookup(const char* path)
{
    const char* div = strchr(path, '/');
    if (!div) return -1;
    const char* end = path + strlen(path);
    
    /* Debugging
    CStr<10> dirStr, fileStr;
    dirStr.append(path, div);
    fileStr.append(div+1, end);    
    Log.p(dirStr.c_str()).p(" ").p(fileStr.c_str()).eol();
    uint16_t dHash = dirStr.hash8();
    uint16_t fHash = fileStr.hash8();
    */
    uint16_t dHash = hash8(path, div);
    uint16_t fHash = hash8(div+1, end);

    //Log.p("d=").p(dHash).p(" f=").p(fHash).eol();

    if (!dHash || !fHash) return -1;

    for(int i=0; i<ConstMemImage::NUM_DIR; i++) {
        //Log.p("  d=").p(dirHash[i]).eol();
        if (dirHash[i] == dHash) {
            for(int j=dirStart[i]; j<ConstMemImage::NUM_FILES; ++j) {
                //Log.p("    f[").p(j).p(")]=").p(fileHash[j]).eol();
                if (fileHash[j] == fHash) {
                    return j;
                }
            }
        }
    }
    return -1;
}

