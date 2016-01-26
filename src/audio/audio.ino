#include <Button.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Audio.h>

#include "AudioPlayer.h"

ButtonCB buttonA;
ButtonCB buttonB;
AudioPlayer audioPlayer;

int track = 0;

void setup() {
  Serial.begin(19200);
  while (!Serial) {
    delay(500);
  }
  Serial.println("Hello");
  buttonA.init(3);
  buttonB.init(4);
  audioPlayer.init();

  buttonA.clickHandler(buttonAClickHandler);
  buttonB.clickHandler(buttonBClickHandler);
}

void buttonAClickHandler(const Button& button) {
  Serial.println("buttonAClickHandler");

  static const int NUM_TRACKS = 3;
  track = (track + 1) % NUM_TRACKS;
  audioPlayer.stop();
  switch (track) {
    case 0: audioPlayer.play("FIATI.WAV", 0);    break;
    case 1: audioPlayer.play("DEMICHEL.WAV", 1); break;
    case 2: audioPlayer.play("ELYSIUM.WAV", 2);  break;
  }
}

void buttonBClickHandler(const Button& button) {
  Serial.println("buttonBClickHandler");

  Serial.print("CPU: ");
  Serial.print(AudioProcessorUsage());
  Serial.print(",");
  Serial.print(AudioProcessorUsageMax());
  Serial.print("    ");
  Serial.print("Memory: ");
  Serial.print(AudioMemoryUsage());
  Serial.print(",");
  Serial.println(AudioMemoryUsageMax());
}

void loop() {
  buttonA.process();
  buttonB.process();
  audioPlayer.doLoop();
  //  playFile("IMPERIAL.WAV");
  //  playFile("440HZ.WAV");
  //  playFile("DEMICHEL.WAV");
  //  playFile("ELYSIUM.WAV");
}

