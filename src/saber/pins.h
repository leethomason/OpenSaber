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

#include <stdint.h>

// --- Configuration ---
// Note: Serial connection should be set to 19200 baud with a newline after commands.

#define SERIAL_DEBUG 1

#define SABER_MODEL_TEST			0
#define SABER_MODEL_GECKO			1	// PCB, Teensy 3, external amp and accel
#define SABER_MODEL_BLACK			2	// PCB, Prop Shield, Teensy 3, Dotstar UI
#define SABER_MODEL_SHOCK			3	// PCB, Prop Shield, Teensy 3, OLED
#define SABER_MODEL_SILVER_SHOCK	4	// PCB, Prop Shield, Teensy 3
#define SABER_MODEL_BO				5	// 2 button prop shield
#define SABER_MODEL_TANO 			6

#define SABER_MODEL 				SABER_MODEL_BO

static const int EEPROM_SIZE = 512;
static const int32_t NOMINAL_VOLTAGE 	= 3700;
static const int32_t HIGH_VOLTAGE 		= 4200;
static const int32_t LOW_VOLTAGE 		= 3500;

#define SABER_ACCELEROMETER_NONE 	0
#define SABER_ACCELEROMETER_LIS3DH	1
#define SABER_ACCELEROMETER_NXP		2

#if SABER_MODEL == SABER_MODEL_GECKO
	#define SABER_ACCELEROMETER 		SABER_ACCELEROMETER_LIS3DH
	#define SABER_SOUND_ON
	#define SABER_SOUND_SHUTDOWN
	#define SABER_VOLTMETER
	#define SABER_TWO_BUTTON
	#define SABER_LOCK

	static const int32_t UVOLT_MULT = 6680;	// FIXME
	#define ID_STR "Gecko (Sentris Body) RGB Luxeon"

	static const int32_t RED_VF   = 2900;   // milli-volts
	static const int32_t RED_I    = 350;    // milli-amps
	static const int32_t RED_R    = 2400;   // milli-ohms

	static const int32_t GREEN_VF = 3150;
	static const int32_t GREEN_I  = 350;
	static const int32_t GREEN_R  = 1800;

	static const int32_t BLUE_VF  = 3150;
	static const int32_t BLUE_I   = 350;
	static const int32_t BLUE_R   = 1800;

#elif SABER_MODEL == SABER_MODEL_BLACK
	#define SABER_NUM_LEDS				4
	#define SABER_UI_START				0
	#define SABER_ACCELEROMETER 		SABER_ACCELEROMETER_NXP
	#define SABER_SOUND_ON
	#define SABER_SOUND_SHUTDOWN
	#define SABER_VOLTMETER
	#define SABER_UI_BRIGHTNESS			4

	static const int32_t UVOLT_MULT = 6680;
	#define ID_STR "Black Knight RGB Cree XPE2"

	static const int32_t RED_VF   = 2100;   
	static const int32_t RED_I    = 350;    
	static const int32_t RED_R    = 4700;

	static const int32_t GREEN_VF = 3400;
	static const int32_t GREEN_I  = 350;
	static const int32_t GREEN_R  = 1000;

	static const int32_t BLUE_VF  = 3100;
	static const int32_t BLUE_I   = 350;
	static const int32_t BLUE_R   = 1800;

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 200;


#elif SABER_MODEL == SABER_MODEL_SHOCK
	#define SABER_ACCELEROMETER 		SABER_ACCELEROMETER_NXP
	#define SABER_SOUND_ON
	#define SABER_SOUND_SHUTDOWN
	#define SABER_VOLTMETER
	#define SABER_NUM_LEDS 			1
	#define SABER_CRYSTAL			80
	#define SABER_CRYSTAL_LOW		24
	#define SABER_DISPLAY

	// FIXME TUNE ALL
	static const int32_t UVOLT_MULT = 6680;
	#define ID_STR "Shock / Golden Knight RGB Cree XPE2"

	static const int32_t RED_VF   = 2100;   
	static const int32_t RED_I    = 350;    
	static const int32_t RED_R    = 4300;

	static const int32_t GREEN_VF = 3400;
	static const int32_t GREEN_I  = 350;
	static const int32_t GREEN_R  = 1000;

	static const int32_t BLUE_VF  = 3100;
	static const int32_t BLUE_I   = 350;
	static const int32_t BLUE_R   = 1600;

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 200;

#elif SABER_MODEL == SABER_MODEL_SILVER_SHOCK
	#define SABER_ACCELEROMETER 		SABER_ACCELEROMETER_NXP
	#define SABER_SOUND_ON
	#define SABER_SOUND_SHUTDOWN
	#define SABER_VOLTMETER
	#define SABER_NUM_LEDS 			5
	#define SABER_UI_START  		1
	#define SABER_CRYSTAL			80
	#define SABER_CRYSTAL_LOW		16
	#define SABER_UI_BRIGHTNESS		8

	static const int32_t UVOLT_MULT = 6750;
	#define ID_STR "Silver Shock Custom RGB Cree XPE2"

	static const int32_t RED_VF   = 2100;   
	static const int32_t RED_I    = 350;    
	static const int32_t RED_R    = 4300;

	static const int32_t GREEN_VF = 3400;
	static const int32_t GREEN_I  = 350;
	static const int32_t GREEN_R  = 1000;

	static const int32_t BLUE_VF  = 3100;
	static const int32_t BLUE_I   = 350;
	static const int32_t BLUE_R   = 1600;

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 200;

#elif SABER_MODEL == SABER_MODEL_BO
	#define SABER_ACCELEROMETER 		SABER_ACCELEROMETER_NXP
	#define SABER_SOUND_ON
	#define SABER_SOUND_SHUTDOWN
	#define SABER_VOLTMETER

	static const int32_t UVOLT_MULT = 6750;
	#define ID_STR "Bo Luxeon RGB"

	static const int32_t RED_VF   = 2900;   // milli-volts
	static const int32_t RED_I    = 350;    // milli-amps
	static const int32_t RED_R    = 2400;   // milli-ohms

	static const int32_t GREEN_VF = 3150;
	static const int32_t GREEN_I  = 350;
	static const int32_t GREEN_R  = 1800;

	static const int32_t BLUE_VF  = 3150;
	static const int32_t BLUE_I   = 350;
	static const int32_t BLUE_R   = 1800;

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 200;

#elif SABER_MODEL == SABER_MODEL_TANO
	//#define SABER_ACCELEROMETER 		SABER_ACCELEROMETER_NXP
	//#define SABER_SOUND_ON
	#define SABER_SOUND_SHUTDOWN
	#define SABER_VOLTMETER
	//#define SABER_NUM_LEDS 			4
	//#define SABER_UI_START  		0
	//#define SABER_UI_BRIGHTNESS		8

	static const int32_t UVOLT_MULT = 6750;
	#define ID_STR "Tano Cree XPE2 GGB"

	static const int32_t RED_VF   = 2100;   
	static const int32_t RED_I    = 350;    
	static const int32_t RED_R    = 4300;

	static const int32_t GREEN_VF = 3400;
	static const int32_t GREEN_I  = 350;
	static const int32_t GREEN_R  = 1000;

	static const int32_t BLUE_VF  = 3100;
	static const int32_t BLUE_I   = 350;
	static const int32_t BLUE_R   = 1600;

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 200;

#endif

#if ((SABER_MODEL == SABER_MODEL_GECKO) || (SABER_MODEL == SABER_MODEL_TANO))	
	/* Teensy 3.2 */
	#define PIN_RX1           0
	#define PIN_TX1           1
	#define PIN_AMP_EN  	  2
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

#else
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
#endif

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

