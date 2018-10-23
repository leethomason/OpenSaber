#include "mcmemimage.h"

#include <Adafruit_SPIFlash.h>

#include "Grinliz_Util.h"
#include "compress.h"

using namespace wav12;

void readDir(Adafruit_SPIFlash& flash, MemUnit* dir)
{
    flash.readBuffer(0, (uint8_t*) dir, sizeof(MemUnit)*MemImage::NUM_DIR);
}

void readFile(Adafruit_SPIFlash& flash, int index, MemUnit* file)
{
    flash.readBuffer(
        sizeof(MemUnit) * MemImage::NUM_DIR + sizeof(MemUnit) * index,
        (uint8_t*) file,
        sizeof(MemUnit)
    );
}

void readAudioInfo(Adafruit_SPIFlash& flash, const MemUnit& file, Wav12Header* header, uint32_t* dataAddr)
{
    flash.readBuffer(
        file.offset,
        (uint8_t*) header,
        sizeof(Wav12Header)
    );
    *dataAddr = file.offset + sizeof(Wav12Header);
}

void dumpImage(Adafruit_SPIFlash& flash)
{
    MemUnit dir[MemImage::NUM_DIR];
    readDir(flash, dir);

    Log.p("Memory image:").eol();
    for(int i=0; i<MemImage::NUM_DIR; ++i) {
        if (!dir[i].name.empty()) {
            Log.p("  ").pt(dir[i].name).eol();

            for(unsigned f=0; f<dir[i].size; ++f) {
                MemUnit file;
                int index = dir[i].offset + f;
                readFile(flash, index, &file);

                Wav12Header wavHeader;
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
