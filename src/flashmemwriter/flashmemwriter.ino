#include <Adafruit_SPIFlash.h>

#include "Grinliz_Arduino_Util.h"
#include "Grinliz_Util.h"

Adafruit_FlashTransport_SPI flashTransport(SS1, &SPI1);
Adafruit_SPIFlash spiFlash(&flashTransport);     // Use hardware SPI 

static const int SERIAL_SPEED = 115200;
CStr<400> line;
bool firstLine = true;
int size = 0;
int nRead = 0;
static const int LOCAL_PAGESIZE = 256;      // base64 encoding assumes 256
uint8_t pageBuffer[LOCAL_PAGESIZE] = {0};
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

    spiFlash.begin();

    uint32_t jedecid = spiFlash.getJEDECID();
    Log.p("SPI Flash JEDECID=").p(jedecid).eol();

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
    uint32_t nWritten = spiFlash.writeBuffer(addr, pageBuffer, LOCAL_PAGESIZE);

    Log.p("Page ").p(nPages).p(" written. nBytes=").p(nWritten).p("/").p(LOCAL_PAGESIZE)
        .p(" ").p(nWritten == LOCAL_PAGESIZE ? "okay" : "ERROR").eol();

    nPages++;
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
                    static const int STEP = 256;
                    int n = STEP;
                    if (nRead + n > size) {
                        n = size - nRead;
                    }
                    decodeBase64(line.c_str(), n, pageBuffer);
                    sendPage();
                    nRead += n;

                    if (nRead == size) {
                        Log.p("100%. Size=").p(nRead).p(" bytes read.").eol();
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
