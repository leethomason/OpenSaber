#include <Grinliz_Arduino_Util.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

void setup() {
    Serial.begin(19200);  // still need to turn it on in case a command line is connected.
    while (!Serial) {
        delay(100);
    }
    Log.attachSerial(&Serial);
    Log.p("Hello.").eol();

    SerialFlash.begin(6);

    SPI.setMOSI(11);
    SPI.setSCK(13);
    if (!(SD.begin(15))) {
        while (1) {
            Serial.println("Unable to access the SD card");
            delay(500);
        }
    }
    Log.p("SD open.").eol();
    //SerialFlash.eraseAll();
    //Log.p("Flash ereased.").eol();

    File src = SD.open("/BESPIN2/HUM.WAV");
    if (!src) {
        Serial.println("Couldn't open src.");
    }

    uint32_t size = src.size();
    SerialFlash.createErasable("HUM.WAV", size);
    SerialFlashFile dst = SerialFlash.open("HUM.WAV");
    if (!dst) {
        Serial.println("Couldn't open dst.");
    }

    Serial.println("good to go. starting copy.");

    uint32_t rem = size;
    static const int BUF_SIZE = 256;
    uint8_t buf[BUF_SIZE];
    while(rem) {
        uint32_t chunk = min(BUF_SIZE, rem);
        rem -= chunk;
        src.read(buf, chunk);
        dst.write(buf, chunk);
    }

    /*
    for(uint32_t i=0; i<size; ++i) {
        uint8_t byte = src.read();
        dst.write(&byte, 1);
    }
    */

    Serial.println("copied");


    /*
    uint32_t totalSize = 0;

    File root = SD.open("/BESPIN2/");
    while (true) {
        File entry =  root.openNextFile();
        if (!entry) {
            break;
        }
        if (entry.isDirectory()) {
            continue;
        }
        uint32_t size = entry.size();
        Log.p(entry.name()).p(" ").p(size).eol();

        SerialFlash.create(entry.name(), size);
        totalSize += size;
        Log.p("total size=").p(totalSize/1024).p("k").eol();

        //Log.p("created.").eol();
        SerialFlashFile sff = SerialFlash.open(entry.name());
        //Log.p("sff.").eol();
        ASSERT(sff == true);

        //Log.p("closing..").eol();
        sff.close();
        entry.close();
    }
    root.close();
    */
}

void loop() {
}
