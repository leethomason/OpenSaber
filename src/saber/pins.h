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

#ifndef PINS_HEADER
#define PINS_HEADER

static const int EEPROM_SIZE = 512;
#define SABER_ACCELEROMETER_NONE 	0
#define SABER_ACCELEROMETER_LIS3DH	1
#define SABER_ACCELEROMETER_NXP		2

#define SABER_NO_CRYSTAL 			0
#define SABER_DOTSTAR 				1
#define SABER_RGB_CRYSTAL			2

//#define SABER_DOTSTAR_CRYSTAL		0	// LED index if there is a crystal 
#define SABER_DOTSTAR_UI			0	// start led index (of 4) if there is a UI
#define SABER_NUM_LEDS				4

// --- Configuration ---
// Note: Serial connection should be set to 19200 baud with a newline after commands.
#define SERIAL_DEBUG 			1

#define SABER_ACCELEROMETER 	SABER_ACCELEROMETER_NXP
#define SABER_SOUND_ON
#define SABER_SOUND_SHUTDOWN
#define SABER_VOLTMETER
#define SABER_DISPLAY
#define SABER_CRYSTAL			SABER_DOTSTAR
//#define SABER_TWO_BUTTON
// Don't forget to set the emmitter in electrical.h!

/* Teensy 3.2 with Prop Shield */
// Digital
#define PIN_RX1           0
#define PIN_TX1           1
#define PIN_ACCEL_IRQ	  2	// fixed
#define PIN_SWITCH_B      3
#define PIN_LED_B         4
#define PIN_AMP_EN        5 // fixed
#define PIN_MEMORY_CS	  6	// fixed
#define PIN_DOTSTAR_EN    7 // fixed
// 8
#define PIN_OLED_DC       9
#define PIN_OLED_RESET    10
#define PIN_SABER_MOSI    11
#define PIN_SABER_MISO    12

// Analog
#define PIN_SABER_CLOCK   13
#define PIN_OLED_CS       14
#define PIN_SDCARD_CS     15
#define PIN_LED_A         16
#define PIN_SWITCH_A      17
#define PIN_SDA           18 // fixed
#define PIN_SCL           19 // fixed
#define PIN_VMETER        20
#define PIN_EMITTER_BLUE  21
#define PIN_EMITTER_GREEN 22
#define PIN_EMITTER_RED   23


enum {
  RED,
  GREEN,
  BLUE,
  NCHANNELS
};

static const float DEFAULT_G_FORCE_MOTION = 1.3f;
static const float DEFAULT_G_FORCE_IMPACT = 2.2f;

/*
Connections
	SaberZ
		Battery
			Bat+
			Bat-
		Audio
			VBat
			Aground x 2
			DAC
		Accel (LIS3DH)
			Vcc
			SCL
			SDA
			Ground
		Crystal
			R
			G
			B
			Ground

	Stack
		Power
			Bat+
			V5
			Ground (V5)
			Ground (Bat-)

		Oled
			Vcc
			MOSI
			CLCK
			CS
			RST
			D/C
			Ground

	Both
		SD
			Vcc
			MOSI
			MISO
			CLCK
			CS
			Ground
		USB
			D+
			D-
			Ground
*/

#endif // PINS_HEADER

