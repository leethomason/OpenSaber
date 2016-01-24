/*
Copyright (c) 2016 Lee Thomason, Grinning Lizard Software

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.
*/

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

void loop() {
  delay(100);
  //int n = 
  streamer.fillBuffer();
  //Serial.println(n);
}

