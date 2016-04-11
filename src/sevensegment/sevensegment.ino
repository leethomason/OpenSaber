#include <SevSeg.h>

SevSeg display7;

void setup() {
  int displayType = COMMON_CATHODE; //Your display is either common cathode or common anode

  //This pinout is for a bubble dispaly
  //Declare what pins are connected to the GND pins (cathodes)
  #define DIGIT_1 A0
  #define DIGIT_2 4
  #define DIGIT_3 A1
  #define DIGIT_4 3

  #define SEG_A 8
  #define SEG_B 6
  #define SEG_C A5
  #define SEG_D A2
  #define SEG_E A4
  #define SEG_F 5
  #define SEG_G A3

  int numberOfDigits = 4; //Do you have a 1, 2 or 4 digit display?

  display7.Begin(displayType, numberOfDigits, DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4, 
                  SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, 255);

  display7.SetBrightness(100); //Set the display to 100% brightness level
}

void loop() {
  uint32_t t = millis() / 200;

  char str[5] = "0000";
  str[0] = (t / 1000) % 10 + '0';
  str[1] = (t / 100)  % 10 + '0';
  str[2] = (t / 10)   % 10 + '0';
  str[3] = (t)        % 10 + '0';
  
  display7.DisplayString(str, 0);
}
