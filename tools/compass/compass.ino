#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_LIS3DH.h>

#include "Grinliz_Util.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 7 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_LIS3DH lis3dh;

uint32_t lastTime = 0;
AverageSample<float, float, 32> aveX(0);
AverageSample<float, float, 32> aveZ(1);

void setup()
{
    /*Serial.begin(9600);
    while(!Serial) {
        delay(500);
        Serial.begin(9600);
    }
    Serial.println("hello...");*/

    delay(100);
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
        Serial.println("SSD1306 init failed");
        for(;;); // Don't proceed, loop forever
    }
    Serial.println("SSD1306 success.");

    delay(100);
    if (!lis3dh.begin(0x18)) {   // change this to 0x19 for alternative i2c address
        Serial.println("LIS3DH init failed");
        for(;;);
    }
    Serial.println("LIS3DH success.");
    Serial.print("Range="); Serial.print(2 << lis3dh.getRange());
    Serial.println("G");
    lis3dh.setDataRate(LIS3DH_DATARATE_100_HZ);

    display.display();
    delay(1000); // Pause for 2 seconds

    // Clear the buffer
    display.clearDisplay();

    // Draw a single pixel in white
    //display.drawPixel(10, 10, SSD1306_WHITE);
    //display.drawLine(5, 5, 25, 10, SSD1306_WHITE);
    display.setTextSize(2);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);     // Start at top-left corner
    display.cp437(true);         // Use full 256 char 'Code Page 437' font
    display.write("Compass");


    // Show the display buffer on the screen. You MUST call display() after
    // drawing commands to make them visible on screen!
    display.display();
    lastTime = millis();
}

void loop() 
{
    uint32_t t = millis();
    if (t - lastTime <= 10) return;
    lastTime = t;

    sensors_event_t event;
    lis3dh.getEvent(&event);

    float x = event.acceleration.x;
    float z = event.acceleration.z;
    aveX.push(x);
    aveZ.push(z);
    x = aveX.average();
    z = aveZ.average();

    float len = sqrtf(x*x + z*z);
    x = x / len;
    z = z / len;

    float rad = atan2f(x, z);
    float deg = rad * 57.295779513f;

    while(deg < 0) deg += 360.0f;
    while(deg >= 360.0f) deg -= 360.0f;

    int degInt = int(deg * 10.0f);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.write("Deg ");

    display.write('0' + degInt / 1000);
    degInt -= (degInt / 1000) * 1000;
    display.write('0' + degInt / 100);
    degInt -= (degInt / 100) * 100;
    display.write('0' + degInt / 10);
    degInt -= (degInt / 10) * 10;
    display.write('.');
    display.write('0' + degInt);

    display.display();

    //Serial.println(deg);
}