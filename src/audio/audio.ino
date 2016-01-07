#include <SPI.h>
#include <SD.h>
#include <TimerThree.h>

#include "audioStreamer.h"

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
AudioStreamer streamer;
File a440;

const int chipSelect = 10;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(19200);
  while (!Serial) {}

  analogWriteResolution(16);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  //Timer3.initialize(2273 / 2);
  Timer3.initialize(45);
  Timer3.attachInterrupt(AudioStreamer::interuptFunc);

  Serial.println("\nInitializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("Error opening SD card");
  }

  File root = SD.open("/");
  printDirectory(root, 0);
  root.close();

  a440 = SD.open("440HZ.WAV", FILE_READ);
  if (a440) {
    Serial.println("a440 open");
    streamer.play(&a440);
  }
}

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

int state = HIGH;
void blinkLED()
{
  state = (state == HIGH) ? LOW : HIGH;
  digitalWrite(2, state);
}

uint32_t wave = 0;
void testWave() {
  wave = wave ? 0 : 65525;
  analogWrite(A14, wave);
}

//float phase = 0.0;
//float twopi = 3.14159 * 2;

void loop() {
  /*
  static const uint32_t WIDTH = 2273;
  uint32_t wave = micros() % WIDTH;
  analogWrite(A14, wave < WIDTH / 2 ? 65535 : 0);
*/
  delay(100);
  int n = streamer.fillBuffer();
  Serial.println(n);
}

