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


#define SABER_MODEL_TEST			255
#define SABER_MODEL_GECKO			1	// PCB, Teensy 3, external amp and accel
#define SABER_MODEL_BLACK			2	// PCB, Prop Shield, Teensy 3, Dotstar UI
//#define SABER_MODEL_SHOCK			3	// PCB, Prop Shield, Teensy 3, OLED. Dropped; heatsink crushed carriage when I dropped it. Become SHOCK2.
#define SABER_MODEL_SILVER_SHOCK	4	// PCB, Prop Shield, Teensy 3
//#define SABER_MODEL_BO			5	// 2 button prop shield Never finished
//#define SABER_MODEL_PROTO 		6	// Burned out; never needed to rebuild.
#define SABER_MODEL_TANO 			7
//#define SABER_MODEL_SISTER 		8	// Shield v3, Teensy 3.5, RF24 communication
#define SABER_MODEL_AQUATIC			9
#define SABER_MODEL_SHOCK2		   10	// Shield v3, Teensy 3.5, Dotstar, Display (woh)
#define SABER_MODEL_VIGILANCE	   11	// Shield v4, Teensy 3.5, Dotstar crystals, bubble display AKA Tali
#define SABER_MODEL_AHSOKA		   12
#define SABER_MODEL_GREEN_SKY	   13
#define SABER_MODEL_TANO_2 		   14   // Shield v4, Teensy 3.5, Dotstar UI
#define SABER_MODEL_SISTER_2	   15   // Shield v4, Teensy 3.5, 5x7 Display. Replaced the cool-but-useless wirelss communication
#define SABER_MODEL_KENOBI_IV	   16   // ItsyBitsy, Shield v1,Neopixel
#define SABER_MODEL_AQUATIC_2	   17   // ItsyBitsy, Shield v1c, Dotstar
#define SABER_MODEL_TYVOKKA		   18   // ItsyBitsy, Shield v1c, Dotstar
//#define SABER_MODEL_SHOCK3  	   19   // Itsy v2b, OLED (128x32), Dotstar crystal
#define SABER_MODEL_LEIA  	   	   20   // Itsy v2b, Dotstar UI

#define SABER_SUB_MODEL_NONE		0
#define SABER_SUB_MODEL_LUNA		1
#define SABER_SUB_MODEL_CELESTIA	2
#define SABER_SUB_MODEL_SHOTO       3
#define SABER_SUB_MODEL_STANDARD    4

// ----------------------------------
#define SERIAL_DEBUG 				1
#define SABER_MODEL 				SABER_MODEL_LEIA
#define SABER_SUB_MODEL				SABER_SUB_MODEL_STANDARD
// ----------------------------------

#define LED_TYPE_RGB				1   // red-green-blue
#define LED_TYPE_BBG				2   // blue-blue-green
#define LED_TYPE_WWG				3   // white-white-green
#define LED_TYPE_GGC				4	// green-green-cyan

#define SABER_SOUND_SD 				1	// external memory card
#define SABER_SOUND_FLASH 			2	// M0 memory

#define SABER_DISPLAY_128_32		1
#define SABER_DISPLAY_7_5_DEPRECATED 2	// NON shifted. So many wires. So many.
#define SABER_DISPLAY_SEGMENT		3	// Shifted
#define SABER_DISPLAY_7_5 			4	// Shifted

#define SABER_LED_NEOPIXEL			1
#define SABER_LED_DOTSTAR			2

#define PCB_VERSION_1				1   // PCB, Teensy 3.2, external amp and accel
#define PCB_VERSION_5				5   // PCB, Prop Shield, Teensy 3.2, OLED (VERIFY)
#define PCB_VERSION_7				7	// Split PCB. Prop shield. Teensy 3.5.
#define PCB_VERSION_9				9   // Split PCB. Prop shield. Teensy 3.5.
#define PCB_SHIELD_2			   12	// Grinliz shield + Teensy 3.5
#define PCB_SHIELD_3			   13	
#define PCB_SHIELD_4			   14	
#define PCB_SHIELD_5			   15

#define PCB_ITSY_1				   16
#define PCB_ITSY_1C				   17	// 1B was a bad run, 1C adds dotstar support
#define PCB_ITSY_2A				   18
#define PCB_ITSY_2B				   19   // Gyro + Accelerometer on the I2C bus. Dotstar. SPI. I2S audio. Flash mem sound.

static const int EEPROM_SIZE = 512;

static const int32_t HIGH_VOLTAGE 		= 4000;		// "full charge" at 4.0v or above
static const int32_t NOMINAL_VOLTAGE    = 3700;
static const int32_t LOW_VOLTAGE 		= 3400;		// 3500 gets sketchy. By 3.4 we're weird.

#define SABER_ACCELEROMETER_NONE 		0
#define SABER_ACCELEROMETER_LIS3DH		1
#define SABER_ACCELEROMETER_NXP			2
#define SABER_ACCELEROMETER_LIS3DH_SPI 	3
#define SABER_ACCELEROMETER_LSM303 		4	// SPI, accel, gyro

#define SABER_SOUND_DEF_BESPIN_ROGUE  1
#define SABER_SOUND_DEF_BESPIN_JAINA  2

#if SABER_MODEL == SABER_MODEL_GECKO
	#define PCB_VERSION 				PCB_VERSION_1
	#define SABER_ACCELEROMETER 		SABER_ACCELEROMETER_LIS3DH
	#define SABER_SOUND_ON 				SABER_SOUND_SD
	#define SABER_VOLTMETER
	#define SABER_TWO_BUTTON
	#define SABER_LOCK
	#define SABER_BUTTON 				Button::PULL_DOWN

	static const int32_t UVOLT_MULT = 6680;
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

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 200;	

#elif SABER_MODEL == SABER_MODEL_BLACK
	#define PCB_VERSION 				PCB_VERSION_5
	#define SABER_NUM_LEDS				4
	#define SABER_UI_START				0
	#define SABER_ACCELEROMETER 		SABER_ACCELEROMETER_NXP
	#define SABER_SOUND_ON 				SABER_SOUND_SD
	#define SABER_VOLTMETER
	#define SABER_UI_BRIGHTNESS			8

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
	#define PCB_VERSION 				PCB_VERSION_5
	#define SABER_ACCELEROMETER 		SABER_ACCELEROMETER_NXP
	#define SABER_SOUND_ON 				SABER_SOUND_SD
	#define SABER_VOLTMETER
	#define SABER_NUM_LEDS 			1
	#define SABER_CRYSTAL			80
	#define SABER_CRYSTAL_LOW		24
	#define SABER_DISPLAY			SABER_DISPLAY_128_32
	#define SABER_LOWPASS_FILTER	1

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
	#define PCB_VERSION 				PCB_VERSION_5
	#define SABER_ACCELEROMETER 		SABER_ACCELEROMETER_NXP
	#define SABER_SOUND_ON 				SABER_SOUND_SD
	#define SABER_VOLTMETER
	#define SABER_NUM_LEDS 			5
	#define SABER_UI_START  		1
	#define SABER_CRYSTAL			80
	#define SABER_CRYSTAL_LOW		16
	#define SABER_UI_BRIGHTNESS		8
	#define SABER_LOWPASS_FILTER	1

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
	#define PCB_VERSION 				PCB_VERSION_5
	#define SABER_ACCELEROMETER 		SABER_ACCELEROMETER_NXP
	#define SABER_SOUND_ON 				SABER_SOUND_SD
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

#elif (SABER_MODEL == SABER_MODEL_TANO)
	#define PCB_VERSION 				PCB_VERSION_7
	#define SABER_ACCELEROMETER 		SABER_ACCELEROMETER_NXP
	#define SABER_SOUND_ON 				SABER_SOUND_SD
	#define SABER_VOLTMETER
	#define SABER_INTEGRATED_SD
	#define LED_TYPE 					LED_TYPE_BBG
	#define SABER_BUTTON 				Button::INTERNAL_PULLUP

	#define SABER_NUM_LEDS 			4
	#define SABER_UI_START  		0
	#define SABER_UI_BRIGHTNESS		8

	static const int32_t UVOLT_MULT = 6750;
	#define ID_STR "Tano-2 Cree XPE2 GGB"

	// Actually GREEN
	static const int32_t RED_VF   = 3400;   // milli-volts
	static const int32_t RED_I    = 350;    // milli-amps
	static const int32_t RED_R    = 1000;   // milli-ohms

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

#elif (SABER_MODEL == SABER_MODEL_SISTER)
	#define PCB_VERSION 				PCB_SHIELD_3
	#define SABER_ACCELEROMETER 		SABER_ACCELEROMETER_LIS3DH_SPI
	#define SABER_SOUND_ON 				SABER_SOUND_SD
	#define SABER_VOLTMETER
	#define SABER_INTEGRATED_SD
	#define SABER_COMRF24
	#define SABER_AUDIO_UI
	#define SABER_BUTTON 				Button::INTERNAL_PULLUP

	#define SABER_ADDR_0 "1Sist"
	#define SABER_ADDR_1 "2Sist"

	static const int32_t UVOLT_MULT = 6691;
	#if SABER_SUB_MODEL == SABER_SUB_MODEL_LUNA
	#define ID_STR "Luna: Sisters Cree XPE2 RGB"
	#elif SABER_SUB_MODEL == SABER_SUB_MODEL_CELESTIA
	#define ID_STR "Celestia: Sisters Cree XPE2 RGB"
	#endif

	// Thermal paste heat sinking.
	static const int32_t RED_VF   = 2100;   // milli-volts
	static const int32_t RED_I    = 400;    // milli-amps
	static const int32_t RED_R    = 4300;   // milli-ohms

	static const int32_t GREEN_VF = 3400;
	static const int32_t GREEN_I  = 400;
	static const int32_t GREEN_R  = 1000;

	static const int32_t BLUE_VF  = 3100;
	static const int32_t BLUE_I   = 400;
	static const int32_t BLUE_R   = 1600;

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 200;

#elif (SABER_MODEL == SABER_MODEL_AQUATIC)
	#define PCB_VERSION 			PCB_SHIELD_3
	#define SABER_ACCELEROMETER 	SABER_ACCELEROMETER_LIS3DH_SPI
	#define SABER_SOUND_ON 			SABER_SOUND_SD
	#define SABER_VOLTMETER
	#define SABER_INTEGRATED_SD
//	#define SABER_AUDIO_UI
	#define SABER_DISPLAY			SABER_DISPLAY_7_5_DEPRECATED

	#define SABER_BUTTON 			Button::INTERNAL_PULLUP

	static const int32_t UVOLT_MULT = 6691;		
	#define ID_STR "Aquatic Cree XPE2 RGB"

	// Thermal paste heat sinking.
	static const int32_t RED_VF   = 2100;   // milli-volts
	static const int32_t RED_I    = 400;    // milli-amps
	static const int32_t RED_R    = 4300;   // milli-ohms

	static const int32_t GREEN_VF = 3400;
	static const int32_t GREEN_I  = 400;
	static const int32_t GREEN_R  = 1000;

	static const int32_t BLUE_VF  = 3100;
	static const int32_t BLUE_I   = 400;
	static const int32_t BLUE_R   = 1600;

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 200;

	static const uint8_t ANODE_COL_1 	= 28;
	static const uint8_t ANODE_COL_2 	= 32;
	static const uint8_t ANODE_COL_3 	= 38;
	static const uint8_t ANODE_COL_4 	= 27;
	static const uint8_t ANODE_COL_5 	= 33;

	static const uint8_t CATHODE_ROW_1 	= 31;
	static const uint8_t CATHODE_ROW_2 	= 34;
	static const uint8_t CATHODE_ROW_3 	= 30;
	static const uint8_t CATHODE_ROW_4 	= 29;
	static const uint8_t CATHODE_ROW_5 	= 35;
	static const uint8_t CATHODE_ROW_6 	= 36;
	static const uint8_t CATHODE_ROW_7 	= 37;

#elif (SABER_MODEL == SABER_MODEL_SHOCK2)
	#define PCB_VERSION 			PCB_SHIELD_3
	#define SABER_ACCELEROMETER 	SABER_ACCELEROMETER_LIS3DH_SPI
	#define SABER_SOUND_ON 			SABER_SOUND_SD
	#define SABER_VOLTMETER
	#define SABER_INTEGRATED_SD
	#define SABER_DISPLAY			SABER_DISPLAY_128_32
	#define SABER_NUM_LEDS 			1
	#define SABER_CRYSTAL			80
	#define SABER_CRYSTAL_LOW		24

	#define SABER_BUTTON 			Button::INTERNAL_PULLUP

	static const int32_t UVOLT_MULT = 6691;		
	#define ID_STR "Shock-2 Cree XPE2 RGB"

	// Thermal paste heat sinking.
	static const int32_t RED_VF   = 2100;   // milli-volts
	static const int32_t RED_I    = 380;    // milli-amps
	static const int32_t RED_R    = 4700;   // milli-ohms

	static const int32_t GREEN_VF = 3400;
	static const int32_t GREEN_I  = 380;
	static const int32_t GREEN_R  = 1000;

	static const int32_t BLUE_VF  = 3100;
	static const int32_t BLUE_I   = 380;
	static const int32_t BLUE_R   = 1800;

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 200;

#elif (SABER_MODEL == SABER_MODEL_VIGILANCE)
	#define PCB_VERSION 			PCB_SHIELD_4
	#define SABER_ACCELEROMETER 	SABER_ACCELEROMETER_LIS3DH_SPI
	#define SABER_SOUND_ON 			SABER_SOUND_SD
	#define SABER_VOLTMETER
	#define SABER_INTEGRATED_SD

	#define OVERRIDE_BOOT_SOUND		"mesfx/Blip"
	#define OVERRIDE_READY_SOUND	"mesfx/Blip"

	#define SABER_CRYSTAL			80
	#define SABER_CRYSTAL_LOW		16
	#define SABER_NUM_LEDS 			2
	#define SABER_FLASH_LED			1
	#define SABER_BUTTON 			Button::INTERNAL_PULLUP
	#define SABER_DISPLAY			SABER_DISPLAY_SEGMENT

	static const int32_t UVOLT_MULT = 6734;		
	#define ID_STR "Vigilance (Tali) Luxeon RGB"

	// Thermal paste heat sinking.
	static const int32_t RED_VF   = 2900;   // milli-volts
	static const int32_t RED_I    = 400;    // milli-amps
	static const int32_t RED_R    = 4300;   // milli-ohms

	static const int32_t GREEN_VF = 3150;
	static const int32_t GREEN_I  = 400;
	static const int32_t GREEN_R  = 1000;

	static const int32_t BLUE_VF  = 3150;
	static const int32_t BLUE_I   = 400;
	static const int32_t BLUE_R   = 1600;

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 200;

#elif (SABER_MODEL == SABER_MODEL_AHSOKA)
	#define PCB_VERSION 			PCB_SHIELD_3
	#define SABER_ACCELEROMETER 	SABER_ACCELEROMETER_LIS3DH_SPI
	#define SABER_SOUND_ON 			SABER_SOUND_SD
	#define SABER_BOOT_SOUND
	#define LED_TYPE 				LED_TYPE_WWG

	#if SABER_SUB_MODEL == SABER_SUB_MODEL_SHOTO
		#define ID_STR "Ahsoka Shoto Cree XPE2 WWG"
		#define PIN_SDCARD_CS    10
	#elif SABER_SUB_MODEL == SABER_SUB_MODEL_STANDARD
		#define ID_STR "Ahsoka Standard Cree XPE2 WWG"
		#define SABER_INTEGRATED_SD
	#else
	  	#error sub model not defined
	#endif
	#define SABER_VOLTMETER
	#define SABER_AUDIO_UI

	#define SABER_BUTTON 			Button::INTERNAL_PULLUP

	static const int32_t UVOLT_MULT = 6691;		

	// Thermal paste heat sinking to the case.
	// White
	static const int32_t RED_VF   = 2950;
	static const int32_t RED_I    = 400;
	static const int32_t RED_R    = 2200;

	// Green
	static const int32_t GREEN_VF = 3400;
	static const int32_t GREEN_I  = 400;
	static const int32_t GREEN_R  = 1000;

	// White
	static const int32_t BLUE_VF   = 2950;
	static const int32_t BLUE_I    = 400;
	static const int32_t BLUE_R    = 2200;

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 200;

#elif (SABER_MODEL == SABER_MODEL_GREEN_SKY)
	#define PCB_VERSION 			PCB_SHIELD_4
	#define SABER_ACCELEROMETER 	SABER_ACCELEROMETER_LIS3DH_SPI
	#define SABER_SOUND_ON 			SABER_SOUND_SD
	#define ID_STR "Green Skywalker Luxeon GGC"
	#define SABER_INTEGRATED_SD
	
	#define SABER_VOLTMETER
	#define SABER_AUDIO_UI
	#define SABER_BOOT_SOUND
	#define LED_TYPE 				LED_TYPE_GGC

	#define SABER_BUTTON 			Button::INTERNAL_PULLUP

	static const int32_t UVOLT_MULT = 6691;

	// Thermal paste heat sinking to the case.
	// Green
	static const int32_t RED_VF   = 2900;
	static const int32_t RED_I    = 400;
	static const int32_t RED_R    = 2200;

	// Green
	static const int32_t GREEN_VF = 2900;
	static const int32_t GREEN_I  = 400;
	static const int32_t GREEN_R  = 2200;

	// Cyan
	static const int32_t BLUE_VF   = 2900;
	static const int32_t BLUE_I    = 400;
	static const int32_t BLUE_R    = 2200;

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 200;

#elif (SABER_MODEL == SABER_MODEL_TANO_2)
	#define PCB_VERSION 			PCB_SHIELD_4
	#define SABER_ACCELEROMETER 	SABER_ACCELEROMETER_LIS3DH_SPI
	#define SABER_SOUND_ON 			SABER_SOUND_SD
	#define SABER_VOLTMETER
	#define SABER_INTEGRATED_SD
 
	#define SABER_NUM_LEDS 			4
	#define SABER_UI_START			0
	#define SABER_BUTTON 			Button::INTERNAL_PULLUP
	#define SABER_UI_BRIGHTNESS		8

	static const int32_t UVOLT_MULT = 6780; // 6734;		
	#define ID_STR "Tano V2 Cree XPE2 RGB"

	// Actually GREEN
	static const int32_t RED_VF   = 3400;   // milli-volts
	static const int32_t RED_I    = 400;    // milli-amps
	static const int32_t RED_R    = 4700;   // milli-ohms

	static const int32_t GREEN_VF = 3400;
	static const int32_t GREEN_I  = 400;
	static const int32_t GREEN_R  = 1000;

	static const int32_t BLUE_VF  = 3100;
	static const int32_t BLUE_I   = 400;
	static const int32_t BLUE_R   = 1800;

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 200;

#elif (SABER_MODEL == SABER_MODEL_SISTER_2)
	#define PCB_VERSION 			PCB_SHIELD_5
	#define SABER_ACCELEROMETER 	SABER_ACCELEROMETER_LIS3DH_SPI
	#define SABER_SOUND_ON 			SABER_SOUND_SD
	#define SABER_VOLTMETER
	#define SABER_INTEGRATED_SD
	#define SABER_DISPLAY			SABER_DISPLAY_7_5
	#define SABER_BUTTON 			Button::INTERNAL_PULLUP
	
	static const int32_t UVOLT_MULT = 6730;	
	#define ID_STR "Sisters V2 Cree XPE2 RGB"

	// Good cooling; short blades.
	// Try 350mA
	// Updated the RGB Vf from the Cree specs.

	static const int32_t RED_VF   = 2200;   // milli-volts
	static const int32_t RED_I    = 350;    // milli-amps
	static const int32_t RED_R    = 4700;   // milli-ohms

	static const int32_t GREEN_VF = 3200;
	static const int32_t GREEN_I  = 350;
	static const int32_t GREEN_R  = 1000;

	static const int32_t BLUE_VF  = 3100;
	static const int32_t BLUE_I   = 350;
	static const int32_t BLUE_R   = 1800;

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 200;

#elif (SABER_MODEL == SABER_MODEL_KENOBI_IV)
	#define PCB_VERSION 			PCB_ITSY_1
	#define SABER_SOUND_ON 			SABER_SOUND_FLASH
	#define SABER_VOLTMETER			
	#define SABER_BUTTON 			Button::INTERNAL_PULLUP
	#define SABER_UI_LED			SABER_LED_NEOPIXEL
	
	#define SABER_NUM_LEDS 			4
	#define SABER_UI_START			0
	#define SABER_UI_COUNT			4
	#define SABER_UI_BRIGHTNESS		8

	#define SABER_SOUND_DEF SABER_SOUND_DEF_BESPIN_JAINA

	static const int32_t UVOLT_MULT = 5530;	
	#define ID_STR "Kenoby IV Cree XPE2 RGB"

	// Heat sink compound; copper TCSS heatsink.
	// Updated the RGB Vf from the Cree specs.
	static const int32_t RED_VF   = 2200;   // milli-volts
	static const int32_t RED_I    = 400;    // milli-amps
	static const int32_t RED_R    = 4700;   // milli-ohms

	static const int32_t GREEN_VF = 3200;
	static const int32_t GREEN_I  = 400;
	static const int32_t GREEN_R  = 1000;

	static const int32_t BLUE_VF  = 3100;
	static const int32_t BLUE_I   = 400;
	static const int32_t BLUE_R   = 1800;

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 204;

#elif (SABER_MODEL == SABER_MODEL_AQUATIC_2)
	#define PCB_VERSION 			PCB_ITSY_1C
	#define SABER_SOUND_ON 			SABER_SOUND_FLASH
	#define SABER_VOLTMETER			
	#define SABER_BUTTON 			Button::INTERNAL_PULLUP
	#define SABER_UI_LED			SABER_LED_DOTSTAR

	#define SABER_NUM_LEDS 			7
	#define SABER_UI_START			0
	#define SABER_UI_COUNT			6
	// There are 32 leves of brightness, expressed from 0-256 in a step size of 8.
	#define SABER_UI_BRIGHTNESS		16		

	#define SABER_UI_IDLE_MEDITATION

	#define SABER_CRYSTAL_START	    6
	#define SABER_CRYSTAL_BRIGHTNESS 64

	#define SABER_SOUND_DEF SABER_SOUND_DEF_BESPIN_ROGUE

	static const int32_t UVOLT_MULT = 5019;	
	#define ID_STR "Aquatic 2 Cree XPE2 RGB"

	// Heat sink compound; copper TCSS heatsink.
	static const int32_t RED_VF   = 2200;   // milli-volts
	static const int32_t RED_I    = 400;    // milli-amps
	static const int32_t RED_R    = 4700;   // milli-ohms

	static const int32_t GREEN_VF = 3200;
	static const int32_t GREEN_I  = 400;
	static const int32_t GREEN_R  = 1000;

	static const int32_t BLUE_VF  = 3100;
	static const int32_t BLUE_I   = 400;
	static const int32_t BLUE_R   = 1800;

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 204;

#elif (SABER_MODEL == SABER_MODEL_TYVOKKA)
	#define PCB_VERSION 			PCB_ITSY_1C
	#define SABER_SOUND_ON 			SABER_SOUND_FLASH
	#define SABER_VOLTMETER			
	#define SABER_BUTTON 			Button::INTERNAL_PULLUP
	#define SABER_UI_LED			SABER_LED_DOTSTAR

	#define SABER_NUM_LEDS 			4
	#define SABER_UI_START			0
	#define SABER_UI_COUNT			4
	#define SABER_UI_BRIGHTNESS	16		
	#define SABER_UI_IDLE_MEDITATION
	#define SABER_UI_REVERSE

	#define SABER_SOUND_DEF 		SABER_SOUND_DEF_BESPIN_JAINA

	static const int32_t UVOLT_MULT = 5612;	
	#define ID_STR "Tyvokka Cree XPE2 RGB"

	// Heat sink compound; LED Supply advanced heat sink.
	static const int32_t RED_VF   = 2200;   // milli-volts
	static const int32_t RED_I    = 400;    // milli-amps
	static const int32_t RED_R    = 4700;   // milli-ohms

	static const int32_t GREEN_VF = 3200;
	static const int32_t GREEN_I  = 400;
	static const int32_t GREEN_R  = 1000;

	static const int32_t BLUE_VF  = 3100;
	static const int32_t BLUE_I   = 400;
	static const int32_t BLUE_R   = 1800;

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 204;

#elif (SABER_MODEL == SABER_MODEL_LEIA)
	#define PCB_VERSION 			PCB_ITSY_2B
	#define SABER_SOUND_ON 			SABER_SOUND_FLASH
	#define SABER_VOLTMETER			
	#define SABER_BUTTON 			Button::INTERNAL_PULLUP
	#define SABER_UI_LED			SABER_LED_DOTSTAR

	#define SABER_NUM_LEDS 			    4
	#define SABER_UI_START				0
	#define SABER_UI_COUNT				4
	#define SABER_UI_BRIGHTNESS	    	16		
	#define SABER_UI_IDLE_MEDITATION
	//#define SABER_UI_REVERSE
	#define SABER_UI_COLOR_CHANGE

	#define SABER_SOUND_DEF 		SABER_SOUND_DEF_BESPIN_JAINA

	static const int32_t UVOLT_MULT = 5612;	// FIXME
	#define ID_STR "Leia Cree XPE2 RGB"

	// FIXME
	static const int32_t RED_VF   = 2200;   // milli-volts
	static const int32_t RED_I    = 400;    // milli-amps
	static const int32_t RED_R    = 4700;   // milli-ohms

	static const int32_t GREEN_VF = 3200;
	static const int32_t GREEN_I  = 400;
	static const int32_t GREEN_R  = 1000;

	static const int32_t BLUE_VF  = 3100;
	static const int32_t BLUE_I   = 400;
	static const int32_t BLUE_R   = 1800;

	static const int VOLUME_1 = 20;
	static const int VOLUME_2 = 60;
	static const int VOLUME_3 = 160;
	static const int VOLUME_4 = 256;

#elif (SABER_MODEL == SABER_MODEL_TEST)
	#define PCB_VERSION 			PCB_ITSY_2A
	#define SABER_SOUND_ON 			SABER_SOUND_FLASH
	#define SABER_VOLTMETER			
	#define SABER_BUTTON 			Button::INTERNAL_PULLUP
	#define SABER_UI_LED			SABER_LED_DOTSTAR

	#define SABER_NUM_LEDS 			4
	#define SABER_UI_START			0
	#define SABER_UI_COUNT			4
	// There are 32 leves of brightness, expressed from 0-256 in a step size of 8.
	#define SABER_UI_BRIGHTNESS		16

	#define SABER_UI_IDLE_MEDITATION

//	#define SABER_CRYSTAL_START	    7
//	#define SABER_CRYSTAL_BRIGHTNESS 24

//	#define SABER_BLACK_START		6
//	#define SABER_BLACK_COUNT		1

	#define SABER_SOUND_DEF SABER_SOUND_DEF_BESPIN_ROGUE

	static const int32_t UVOLT_MULT = 5019;	
	#define ID_STR "Test Cree XPE2 RGB"

	// Heat sink compound; copper TCSS heatsink.
	static const int32_t RED_VF   = 2200;   // milli-volts
	static const int32_t RED_I    = 400;    // milli-amps
	static const int32_t RED_R    = 4700;   // milli-ohms

	static const int32_t GREEN_VF = 3200;
	static const int32_t GREEN_I  = 400;
	static const int32_t GREEN_R  = 1000;

	static const int32_t BLUE_VF  = 3100;
	static const int32_t BLUE_I   = 400;
	static const int32_t BLUE_R   = 1800;

	static const int VOLUME_1 = 15;
	static const int VOLUME_2 = 50;
	static const int VOLUME_3 = 120;
	static const int VOLUME_4 = 204;
#endif

#if (PCB_VERSION == PCB_SHIELD_3)
	/* 
	   Grinning Lizard Shield
	   + dotstar optional
	   + SPI optional
	 */
	#define PIN_RX1           0
	#define PIN_TX1           1
	#define PIN_AMP_EN        2 
	#define PIN_SWITCH_A      3 
	#define PIN_SWITCH_B	  4 
	#define PIN_LED_B		  5	// PWM
	#define PIN_LED_A		  6 // PWM
	#define PIN_DOTSTAR_EN    7 
	#define PIN_SPI_DC		  8
	#define PIN_SPI_CE		  9
	#define PIN_SPI_CS		  10
	#define PIN_SABER_MOSI    11
	#define PIN_SABER_MISO    12
	#define PIN_SPI_CLOCK	  13
	#define PIN_ACCEL_EN      14

	#define PIN_VMETER        20
	#define PIN_EMITTER_BLUE  21
	#define PIN_EMITTER_GREEN 22
	#define PIN_EMITTER_RED   23

	// ALIAS
	#define PIN_OLED_CS		  PIN_SPI_CS
	#define PIN_OLED_RESET 	  PIN_SPI_CE
	#define PIN_OLED_DC    	  PIN_SPI_DC
	#define PIN_OLED_CLOCK    PIN_SPI_CLOCK

	#define ACCEL_BLADE_DIRECTION 0	// x is in the blade direction
#elif (PCB_VERSION == PCB_SHIELD_4 || PCB_VERSION == PCB_SHIELD_5)
	/* 
	   Grinning Lizard Shield
	   + dotstar optional
	   + SPI optional
	   Includes mounting for high-watt resistors

	   v4: ExpressPCB
	   v5: DipTrace
	 */
	#define PIN_AMP_EN        2 
	#define PIN_SWITCH_A      3 
	#define PIN_SWITCH_B	  4 
	#define PIN_LED_B		  5	// PWM
	#define PIN_LED_A		  6 // PWM
	#define PIN_DOTSTAR_EN    7 
	#define PIN_SPI_DC		  8
	#define PIN_SPI_CE		  9
	#define PIN_SPI_CS		  10
	#define PIN_SABER_MOSI    11
	#define PIN_SABER_MISO    12
	#define PIN_SPI_CLOCK	  13

	#define PIN_VMETER        23
	#define PIN_ACCEL_EN      24

	#define PIN_EMITTER_RED   38
	#define PIN_EMITTER_GREEN 37
	#define PIN_EMITTER_BLUE  36

	// ALIAS
	#define PIN_OLED_CS		  PIN_SPI_CS
	#define PIN_OLED_RESET 	  PIN_SPI_CE
	#define PIN_OLED_DC    	  PIN_SPI_DC
	#define PIN_OLED_CLOCK    PIN_SPI_CLOCK

	#define PIN_LATCH		  28
	#define PIN_CLOCK 		  27
	#if PCB_VERSION == PCB_SHIELD_4		
		#define PIN_DATA  		  29
	#elif PCB_VERSION == PCB_SHIELD_5
		#define PIN_DATA  		  30
	#endif
	#define ACCEL_BLADE_DIRECTION 0	// x is in the blade direction. not used for impact detection.
#elif (PCB_VERSION == PCB_ITSY_1 || PCB_VERSION == PCB_ITSY_1C || PCB_VERSION == PCB_ITSY_2A)
	/* Grinning Lizard Shield for ItsyBitys M0.
	   Integrated memory for sound.
	   Neopixel support (early)
	   Dotstar support (later)
	*/
	#define SABER_ACCELEROMETER 	SABER_ACCELEROMETER_LIS3DH_SPI

	#define PIN_VMETER        			A1
	#define PIN_LED_A    	  			A2 
	#define PIN_SWITCH_A				A3
	#if (PCB_VERSION == PCB_ITSY_1C || PCB_VERSION == PCB_ITSY_2A)
	#define PIN_DOTSTAR_EN				A4
	#else
	// A4 exposed
	// A5 exposed
	#endif
	#define PIN_I2S_LRCLK		0
	#define PIN_I2S_BITCLK		1
	// CLOCK	 
	// MOSI
	// MISO 
	#if (PCB_VERSION < PCB_ITSY_2A) 
	#define PIN_ACCEL_EN      	2
	#endif
	// LRCLCK - I2S
	// BITCLK - I2S
	// unused
	// unused
	#define PIN_NEOPIXEL_DATA 	5
	// unused
	#define PIN_EMITTER_RED   	9
	#define PIN_EMITTER_GREEN 	10
	#define PIN_EMITTER_BLUE   	11
	#define PIN_I2S_DATA		12
	// 13 exposed
	
	#define ACCEL_BLADE_DIRECTION 0	// The x direction is the blade.
	
#elif (PCB_VERSION == PCB_ITSY_2B)
	/* Grinning Lizard Shield for ItsyBitys M0.
	   Integrated memory for sound.
	   Neopixel support (early)
	   Dotstar support (later)
	*/
	#define SABER_ACCELEROMETER SABER_ACCELEROMETER_LSM303

	#define PIN_VMETER        	A1
	#define PIN_LED_A    	  	A2 
	#define PIN_SWITCH_A		A3
	#define PIN_DOTSTAR_EN		A4
	#define PIN_OLED_DC		    A5
	#define PIN_I2S_LRCLK		0
	#define PIN_I2S_BITCLK		1
	#define PIN_OLED_CS  	    2
	// CLOCK	 
	// MOSI
	// MISO 
	// LRCLCK - I2S
	// BITCLK - I2S
	#define PIN_EMITTER_RED   	9
	#define PIN_EMITTER_GREEN 	10
	#define PIN_EMITTER_BLUE   	11
	#define PIN_I2S_DATA		12
	#define PIN_OLED_RESET 	    13
	
	#define ACCEL_BLADE_DIRECTION 0	// The x direction is the blade. fixme
	
#else
	#error Pins not defined.
#endif

enum {
  RED,
  GREEN,
  BLUE,
  NCHANNELS
};

static const float DEFAULT_G_FORCE_MOTION = 1.3f;
static const float DEFAULT_G_FORCE_IMPACT = 3.0f;	// Was 2.3. Testing.

#endif // PINS_HEADER

