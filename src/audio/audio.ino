#include <SPI.h>
#include <SD.h>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = 10;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(19200);
  while (!Serial) {}

  Serial.println("\nInitializing SD card...");
  
  if (!SD.begin(chipSelect)) {
    Serial.println("Error opening SD card");
  }
  File a440 = SD.open("440HZ.WAV", FILE_READ);
  if (a440) {
    Serial.println("a440 open");
    a440.seek(0);
    for(int i=0; i<4; ++i) {
      int b = a440.read();
      Serial.println(char(b));
    }
    a440.seek(8);
    for(int i=0; i<4; ++i) {
      int b = a440.read();
      Serial.println(char(b));
    }
    a440.seek(22);
    Serial.print("channels:        "); Serial.println(readU32(a440, 2));
    Serial.print("nSamplesPerSec:  "); Serial.println(readU32(a440, 4));
    Serial.print("nAvgBytesPerSec: "); Serial.println(readU32(a440, 4));
    Serial.print("nBlockAlign:     "); Serial.println(readU32(a440, 2));
    Serial.print("wBitsPerSample:  "); Serial.println(readU32(a440, 2));

    a440.seek(44);
    Serial.println("Sound data");
    for(int i=0; i<100; ++i) {
      Serial.println(readS16(a440));
    }
  }  
}

uint32_t readU32(File& file, int n) {
  uint32_t v = 0;
  for(int i=0; i<n; ++i) {
    int b = file.read();
    //Serial.print("b: "); Serial.println(b);
    if (b < 0)
      return 0;
    v += b << (i*8);  
  }
  return v;
}

int16_t readS16(File& file) {
  int16_t v = 0;
  ((uint8_t*)&v)[0] = file.read();
  ((uint8_t*)&v)[1] = file.read();
  return v;
}


void loop(void) {

}
