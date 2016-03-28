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

// --- Configuration ---
// Note: Serial connection should be set to 19200 baud with a newline after commands.
#define SERIAL_DEBUG 1

//#define SABER_ACCELEROMETER
//#define SABER_SOUND_ON
//#define SABER_SOUND_SHUTDOWN
#define SABER_VOLTMETER
//#define SABER_LOGGING
//#define SABER_DISPLAY
//#define SABER_CRYSTAL
#define SABER_TWO_BUTTON
// Don't forget to set the emmitter in electrical.h!

/* Teensy 3.2 */
#define PIN_RX1           0
#define PIN_TX1           1
#define PIN_AMP_SHUTDOWN  2
#define PIN_CRYSTAL_R     3
#define PIN_CRYSTAL_G     4
#define PIN_CRYSTAL_B     5
#define PIN_SWITCH_B      6
#define PIN_SWITCH_A      7   // switch - power (also PWM)
#define PIN_LED_B         8   // switch LED - audio
#define PIN_LED_A         9   // switch LED - power
#define PIN_SDCARD_CS     10
#define PIN_SABER_MOSI    11
#define PIN_SABER_MISO    12
#define PIN_SABER_CLOCK   13
#define PIN_OLED_DC       14
#define PIN_VMETER        15
#define PIN_OLED_RESET    16 
#define PIN_OLED_CS       17
#define PIN_SDA           18
#define PIN_SCL           19
// 20
#define PIN_EMITTER_RED   21
#define PIN_EMITTER_GREEN 22
#define PIN_EMITTER_BLUE  23
// A14: DAC out

enum {
  RED,
  GREEN,
  BLUE,
  NCHANNELS
};

static const float DEFAULT_G_FORCE_MOTION = 1.3f;
static const float DEFAULT_G_FORCE_IMPACT = 2.2f;

/*
// PCB V4
#define PIN_RX1           0
#define PIN_TX1           1
#define PIN_AMP_SHUTDOWN  2
#define PIN_CRYSTAL_R     3
#define PIN_CRYSTAL_G     4
#define PIN_CRYSTAL_B     5
#define PIN_LED_A         6
#define PIN_LED_B         7
#define PIN_SWITCH_A      8
#define PIN_SWITCH_B      9
#define PIN_SDCARD_CS     10
#define PIN_SABER_MOSI    11
#define PIN_SABER_MISO    12
#define PIN_SABER_CLOCK   13
#define PIN_OLED_DC       14
// 15 open
#define PIN_OLED_RESET    16 
#define PIN_OLED_CS       17
#define PIN_SDA           18
#define PIN_SCL           19
#define PIN_VMETER        20
#define PIN_EMITTER_BLUE  21
#define PIN_EMITTER_GREEN 22
#define PIN_EMITTER_RED   23

Connections
	Battery
		Bat+
		Bat-
	Oled
		CS 	17
		RST 16
		D/C 14
		CLCK clock
		MOSI
		Vin
		Ground
	SD
		Ground
		Vin
		MOSI
		MISO
		CLCK
		CS  10
	Audio
		Aground x 2
		VBat
		DAC
	Accel
		SCL
		SDA
		Vin
		Ground
	Crystal
		R 3
		G 4
		B 5
		Ground
	USB
		D+
		D-
		Ground
*/

#endif // PINS_HEADER

