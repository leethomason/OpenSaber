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
  buttonA.init(8);
  buttonB.init(9);
  
  audioPlayer.init();
  audioPlayer.setVolume(0.5);

  buttonA.setClickHandler(buttonAClickHandler);
  buttonB.setClickHandler(buttonBClickHandler);

  SD.begin();
  File root = SD.open("/");
  while (true) {
    File entry =  root.openNextFile();
    if (!entry) {
      break;
    }
    Serial.print(entry.name());
    Serial.print(" ");
    Serial.println(entry.size(), DEC);
    entry.close();
  }
}

void buttonAClickHandler(const Button& button) {
  Serial.println("buttonAClickHandler");

  static const int NUM_TRACKS = 3;
  track = (track + 1) % NUM_TRACKS;
  //audioPlayer.stop();
  switch (track) {
    case 0: audioPlayer.play("FIATI.WAV");    break;
    case 1: audioPlayer.play("DEMICHEL.WAV"); break;
    case 2: audioPlayer.play("ELYSIUM.WAV");  break;
  }
}

int sequence = 0;

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

  //audioPlayer.mute(!audioPlayer.isMuted());
  //audioPlayer.stop();

/*
  switch(sequence) {
    case 0: audioPlayer.setVolume(0.7); break;
    case 1: audioPlayer.setVolume(1);   break;
    case 2: audioPlayer.setVolume(1.2); break;
  }
  sequence = (sequence + 1) % 3;
  Serial.print("Volume "); Serial.println(audioPlayer.volume());*/
}

void loop() {
  buttonA.process();
  buttonB.process();
  audioPlayer.process();
}

