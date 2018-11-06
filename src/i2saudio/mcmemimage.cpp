#include "mcmemimage.h"

#include <Adafruit_SPIFlash.h>

#include "Grinliz_Util.h"
#include "compress.h"

ConstMemImage::ConstMemImage(Adafruit_SPIFlash& _spiFlash) :
    m_spiFlash(_spiFlash)
{
}

void ConstMemImage::begin()
{
    m_numDir = 0;
    while(m_numDir < NUM_DIR) {
        MemUnit dir;
        ::readDir(m_spiFlash, m_numDir, &dir);
        if (dir.name.empty()) break;
        m_numDir++;
    };
    dirToIndex.scan(m_spiFlash);
}

void ConstMemImage::readDir(int index, MemUnit* dir) const
{
    ::readDir(m_spiFlash, index, dir);
}

void ConstMemImage::readDir(const char* dirName, MemUnit* dir) const
{
    int index = dirToIndex.lookup(dirName);
    if (index >= 0) {
        readDir(index, dir);
    }
}

void ConstMemImage::readFile(int index, MemUnit* file) const
{
    ::readFile(m_spiFlash, index, file);
}


void ConstMemImage::readAudioInfo(int index, wav12::Wav12Header* header, uint32_t* dataAddr)
{
    MemUnit file;
    ::readFile(m_spiFlash, index, &file);
    ::readAudioInfo(m_spiFlash, file, header, dataAddr);
}


int ConstMemImage::lookup(const char* path) const
{
    return dirToIndex.lookup(path);
}


Adafruit_SPIFlash& ConstMemImage::getSPIFlash() const
{
    return m_spiFlash;
}


void readDir(Adafruit_SPIFlash& spiFlash, int index, MemUnit* dir)
{
    ASSERT(index >= 0 && index < ConstMemImage::NUM_DIR);

    noInterrupts();
    spiFlash.readBuffer(
        sizeof(MemUnit) * index, 
        (uint8_t*) dir, 
        sizeof(MemUnit));
    interrupts();
}

void readFile(Adafruit_SPIFlash& spiFlash, int index, MemUnit* file)
{
    ASSERT(index >= 0 && index < ConstMemImage::NUM_FILES);

    noInterrupts();
    spiFlash.readBuffer(
        sizeof(MemUnit) * (ConstMemImage::NUM_DIR + index),
        (uint8_t*) file,
        sizeof(MemUnit)
    );
    interrupts();
}

void readAudioInfo(Adafruit_SPIFlash& spiFlash, const MemUnit& file, wav12::Wav12Header* header, uint32_t* dataAddr)
{
    noInterrupts();
    spiFlash.readBuffer(
        file.offset,
        (uint8_t*) header,
        sizeof(wav12::Wav12Header)
    );
    *dataAddr = file.offset + sizeof(wav12::Wav12Header);
    interrupts();
}

void dumpImage(Adafruit_SPIFlash& spiFlash)
{
    MemUnit dir;

    Log.p("Memory image:").eol();
    for(int i=0; i<ConstMemImage::NUM_DIR; ++i) {
        readDir(spiFlash, i, &dir);
        if (!dir.name.empty()) {
            Log.p("  ").pt(dir.name).p(" offset=").p(dir.offset).p(" size=").p(dir.size).eol();

            for(unsigned f=0; f<dir.size; ++f) {
                MemUnit file;
                int index = dir.offset + f;
                readFile(spiFlash, index, &file);

                wav12::Wav12Header wavHeader;
                uint32_t wavAddr = 0;
                readAudioInfo(spiFlash, file, &wavHeader, &wavAddr);

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

void DirToIndex::scan(Adafruit_SPIFlash& spiFlash)
{
    for(int i=0; i<ConstMemImage::NUM_DIR; ++i) {
        MemUnit dir;
        readDir(spiFlash, i, &dir);
        dirHash[i] = dir.name.hash8();
        dirStart[i] = dir.offset;
        #ifdef DEBUG_LOOKUP
        Log.p("scan ").pt(dir.name).p(" hash=").p(dirHash[i]).eol();
        #endif
    }
    for(int i=0; i<ConstMemImage::NUM_FILES; ++i) {
        MemUnit file;
        readFile(spiFlash, i, &file);
        fileHash[i] = file.name.hash8();
    }
}


int DirToIndex::lookup(const char* path) const
{
    const char* div = strchr(path, '/');
    if (!div) {
        // Just scan directories.
        uint16_t dHash = hash8(path, path + strlen(path));
        for(int i=0; i<ConstMemImage::NUM_DIR; i++) {
            if (dirHash[i] == dHash) {
                return i;
            }
        }
        return -1;
    }

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
uint8_t vpromBuffer[MEM_IMAGE_EEPROM] = { 0 };

void vpromPutRaw(uint32_t addr, size_t size, const void* data)
{
    ASSERT(addr + size < MEM_IMAGE_EEPROM);
    memcpy(vpromBuffer + addr, data, size);
}

void vpromGetRaw(uint32_t addr, size_t size, void* data)
{
    ASSERT(addr + size < MEM_IMAGE_EEPROM);
    memcpy(data, vpromBuffer + addr, size);
}

#endif

#if 0
#ifndef CORE_TEENSY
#define DEBUG_VPROM
static const uint32_t BASE_ADDR = MEM_IMAGE_SIZE - MEM_IMAGE_EEPROM;   // 1k of "EEPROM" at the end.
uint8_t vpromBuffer[MEM_IMAGE_EEPROM] = { 0 };

void vpromPutRaw(uint32_t addr, size_t size, const void* data)
{
    #ifdef DEBUG_VPROM
    Log.p("vpromPut addr=").p(addr).p(" size=").p(size).eol();
    #endif
    ASSERT(addr + size < MEM_IMAGE_EEPROM);

    noInterrupts();  // Lock down the SPI
    Adafruit_SPIFlash& spiFlash = MemImage.getSPIFlash();

    spiFlash.readBuffer(
        BASE_ADDR,
        vpromBuffer,
        MEM_IMAGE_EEPROM
    );

    spiFlash.eraseSector(W25Q16BV_SECTORS - 1); // last sector...takes out 4k. grr.

    memcpy(vpromBuffer + addr, data, size);

    uint32_t bytes = spiFlash.writeBuffer(BASE_ADDR, vpromBuffer, MEM_IMAGE_EEPROM);
    interrupts();
    ASSERT(bytes == size);
}

void vpromGetRaw(uint32_t addr, size_t size, void* data)
{
    #ifdef DEBUG_VPROM
    Log.p("vpromGet addr=").p(addr).p(" size=").p(size).eol();
    #endif
    ASSERT(addr + size < MEM_IMAGE_EEPROM);
    
    noInterrupts();  // Lock down the SPI
    Adafruit_SPIFlash& spiFlash = MemImage.getSPIFlash();
    uint32_t bytes = spiFlash.readBuffer(BASE_ADDR + addr, (uint8_t*)data, uint32_t(size));
    interrupts();
    ASSERT(bytes == size);
}
#endif
#endif