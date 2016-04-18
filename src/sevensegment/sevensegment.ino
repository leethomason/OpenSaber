#include <SevSeg.h>
#include <Grinliz_Arduino_Util.h>

SevSeg display7;
static const int SPI_BUFFER_SIZE = 4;
volatile uint8_t spiBuffer[SPI_BUFFER_SIZE];
CStr<4> serialBuffer;

// 7 segment bubble display.
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

void setup() {
  Serial.begin(19200);
  while(!Serial) {
    delay(100);
  }
  
  int displayType = COMMON_CATHODE;

  int numberOfDigits = 4;

  display7.Begin(displayType, numberOfDigits,
                 DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4,
                 SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, 255);

  display7.SetBrightness(50);
  Serial.println("7seg setup()");
}

void serialEvent() {
  while (Serial.available()) {
    int c = Serial.read();
    if (c == '\n') {
      for(int i=0; i<SPI_BUFFER_SIZE && i < serialBuffer.size(); ++i) {
        spiBuffer[i] = serialBuffer[i];
      }
      serialBuffer.clear();
    }
  }
}

void loop() {
  uint32_t t = millis() / 200;

  char str[5] = {0};
  str[0] = (t / 1000) % 10 + '0';
  str[1] = (t / 100)  % 10 + '0';
  str[2] = (t / 10)   % 10 + '0';
  str[3] = (t)        % 10 + '0';

  display7.DisplayString(str, 0);
}

