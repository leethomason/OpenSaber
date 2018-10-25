#include "mcmemimage.h"

#include <Adafruit_SPIFlash.h>

#include "Grinliz_Util.h"
#include "compress.h"

ConstMemImage::ConstMemImage(Adafruit_SPIFlash& _spiFlash) :
    m_spiFlash(_spiFlash)
{
    m_numDir = 0;
    while(m_numDir < NUM_DIR) {
        MemUnit dir;
        ::readDir(m_spiFlash, m_numDir, &dir);
        if (dir.name.empty()) break;
        m_numDir++;
    };
}

void ConstMemImage::readDir(int index, MemUnit* dir) const
{
    ::readDir(m_spiFlash, index, dir);
}

void ConstMemImage::readFile(int index, MemUnit* file) const
{
    ::readFile(m_spiFlash, index, file);
}


int ConstMemImage::lookup(const char* path)
{
    if (!dirToIndex.isScanned()) {
        dirToIndex.scan(m_spiFlash);
    }
    return dirToIndex.lookup(path);
}


Adafruit_SPIFlash& ConstMemImage::getSPIFlash() const
{
    return m_spiFlash;
}


void readDir(Adafruit_SPIFlash& flash, int index, MemUnit* dir)
{
    ASSERT(index >= 0 && index < ConstMemImage::NUM_DIR);

    flash.readBuffer(
        sizeof(MemUnit) * index, 
        (uint8_t*) dir, 
        sizeof(MemUnit));
}

void readFile(Adafruit_SPIFlash& flash, int index, MemUnit* file)
{
    ASSERT(index >= 0 && index < ConstMemImage::NUM_FILES);

    flash.readBuffer(
        sizeof(MemUnit) * (ConstMemImage::NUM_DIR + index),
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
            Log.p("  ").pt(dir.name).p(" offset=").p(dir.offset).p(" size=").p(dir.size).eol();

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

//#define DEBUG_LOOKUP

DirToIndex::DirToIndex()
{
    for(int i=0; i<ConstMemImage::NUM_DIR; i++) {
        dirHash[i] = 0;
        dirStart[i] = 0;
    }
    for(int i=0; i<ConstMemImage::NUM_FILES; i++) {
        fileHash[i] = 0;
    }
}

void DirToIndex::scan(Adafruit_SPIFlash& flash)
{
    for(int i=0; i<ConstMemImage::NUM_DIR; ++i) {
        MemUnit dir;
        readDir(flash, i, &dir);
        dirHash[i] = dir.name.hash8();
        dirStart[i] = dir.offset;
        #ifdef DEBUG_LOOKUP
        Log.p("scan ").pt(dir.name).p(" hash=").p(dirHash[i]).eol();
        #endif
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
    
    #ifdef DEBUG_LOOKUP
    CStr<10> dirStr, fileStr;
    dirStr.append(path, div);
    fileStr.append(div+1, end);    
    Log.p(dirStr.c_str()).p(" ").p(fileStr.c_str()).eol();
    #endif

    uint16_t dHash = hash8(path, div);
    uint16_t fHash = hash8(div+1, end);

    #ifdef DEBUG_LOOKUP
    Log.p("d=").p(dHash).p(" f=").p(fHash).eol();
    #endif

    if (!dHash || !fHash) return -1;

    for(int i=0; i<ConstMemImage::NUM_DIR; i++) {
        #ifdef DEBUG_LOOKUP
        Log.p("  d=").p(dirHash[i]).eol();
        #endif
        if (dirHash[i] == dHash) {
            for(int j=dirStart[i]; j<ConstMemImage::NUM_FILES; ++j) {
                #ifdef DEBUG_LOOKUP
                Log.p("    f[").p(j).p(")]=").p(fileHash[j]).eol();
                #endif
                if (fileHash[j] == fHash) {
                    return j;
                }
            }
        }
    }
    return -1;
}

#ifndef CORE_TEENSY
static const uint32_t BASE_ADDR = MEM_IMAGE_SIZE - MEM_IMAGE_EEPROM;   // 1k of "EEPROM" at the end.

void vpromPutRaw(uint32_t addr, size_t size, const void* data)
{
    ASSERT(addr + size < MEM_IMAGE_EEPROM);
    Adafruit_SPIFlash& spiFlash = MemImage.getSPIFlash();
    spiFlash.writeBuffer(BASE_ADDR + addr, (uint8_t*)data, uint32_t(size));
}

void vpromGetRaw(uint32_t addr, size_t size, void* data)
{
    ASSERT(addr + size < MEM_IMAGE_EEPROM);
    Adafruit_SPIFlash& spiFlash = MemImage.getSPIFlash();
    spiFlash.readBuffer(BASE_ADDR + addr, (uint8_t*)data, uint32_t(size));
}
#endif