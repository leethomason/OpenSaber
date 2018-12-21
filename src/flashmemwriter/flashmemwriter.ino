#include <Adafruit_SPIFlash.h>

#include "Grinliz_Arduino_Util.h"
#include "Grinliz_Util.h"

#define FLASH_SS       SS1                    // Flash chip SS pin.
#define FLASH_SPI_PORT SPI1                   // What SPI port is Flash on?
Adafruit_SPIFlash spiFlash(FLASH_SS, &FLASH_SPI_PORT);     // Use hardware SPI 

static const int SERIAL_SPEED = 115200;
CStr<200> line;
bool firstLine = true;
int size = 0;
int nRead = 0;
static const int LOCAL_PAGESIZE = 256;
uint8_t pageBuffer[LOCAL_PAGESIZE] = {0};
uint32_t pageAddr = 0;
int nPages = 0;
bool userConfirmed = false;

void setup()
{
    Serial.begin(115200);
    while (!Serial) {
        delay(200);
    }
    Log.attachSerial(&Serial);

    Log.p("Serial started at ").p(SERIAL_SPEED).eol();

    spiFlash.begin(SPIFLASHTYPE_W25Q16BV);

    uint8_t manid, devid;
    spiFlash.GetManufacturerInfo(&manid, &devid);
    Log.p("Manufacturer: 0x").p(manid, HEX).eol();
    Log.p("Device ID: 0x").p(devid, HEX).eol();
    Log.p("Pagesize: ").p(spiFlash.pageSize()).p(" Page buffer: ").p(LOCAL_PAGESIZE).eol();

    Log.p("Enter 'y' to erase chip and begin upload.").eol();
}

void eraseChip()
{
    bool eraseOkay = spiFlash.eraseChip();
    Log.p("Erase Chip=").p(eraseOkay ? "true" : "false").eol();
    Log.p("Waiting for file.").eol();
}

void parseSize() {
    Log.p("Linesize=").p(line.size()).eol();
    int mult = 1;
    for(int i=1; i<line.size(); ++i)
        mult *= 10;
    for(int i=0; i<line.size(); ++i) {
        size += mult * (line[i] - '0');
        mult /= 10;
    }
    Log.p("Size=").p(size).eol();
}

void sendPage()
{
    uint32_t addr = LOCAL_PAGESIZE * nPages;
    uint32_t nWritten = spiFlash.writePage(addr, pageBuffer, pageAddr);

    Log.p("Page ").p(nPages).p(" written. nBytes=").p(nWritten).p("/").p(pageAddr)
        .p(" ").p(nWritten == pageAddr ? "okay" : "ERROR").eol();

    nPages++;
    pageAddr = 0;
    memset(pageBuffer, 0, LOCAL_PAGESIZE);
}


void loop()
{
    while(Serial.available()) {
        int b = Serial.read();
        if (b == '\n' || b=='\r') {
            if (!line.empty()) {
                if (!userConfirmed) {
                    if (line == "y" || line == "Y") {
                        userConfirmed = true;
                        eraseChip();
                    }
                }
                else if (firstLine) {
                    parseSize();
                    firstLine = false;
                }
                else {
                    for(int i=0; i<line.size(); i+=2) {
                        int v = hexToDec(line[i]) * 16;
                        v += hexToDec(line[i+1]);
                        pageBuffer[pageAddr++] = v;
                        if (pageAddr == LOCAL_PAGESIZE) {
                            sendPage();
                        }
                        
                        ++nRead;
                        if (nRead == size) {
                            if (pageAddr) {
                                sendPage();
                            }
                            Log.p("100%. Size=").p(nRead).p(" bytes read.").eol();
                        }
                    }
                }
            }
            line.clear();
        }
        else {
            line.append(b);
        }
    }
}
