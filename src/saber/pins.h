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
#define SABER_SOUND_ON
//#define SABER_VOLTMETER
//#define SABER_LOGGING
// Don't forget to set the emmitter in electrical.h!
  
/*
 * Actions:
 *  Blade on/off:   Hold A
 *  Parry blaster:  Tap B
 *  Lock-up         Hold B
 *  Switch color:   Hold B, Tap A (Blade on)
 *  Toggle sound:   Hold B (Blade off)
 */

/*
 * Timer Notes:
 * Timer0 -> Pins 5, 6 fast PWM
 * Timer1 -> Pins 9, 10 phase correct, in setup() TCNT1=0x7FFF; to shift phase
 * Timer2 -> Pins 11, 3 phase correct
 * Set Timer1: phase
 */ 

/* Teensy 3.2 */
// Reserve RX1            0
// Reserve TX1            1
// shutdown               2
#define PIN_SWITCH_A      3   // switch - power (also PWM)
#define PIN_SWITCH_B      4
#define PIN_LED_A         5   // switch LED - power
#define PIN_LED_B         6   // switch LED - audio
// reserve rx3            7
// reserve tx3            8
// RX2, pwm               9
// SD: CS                 10
// SD: DOUT               11
// SD: DIN                12
// SD: SCK                13
#define PIN_LED_LOW_POWER 14
// 15
// 16
// 17
// 18
// 19
// 20
#define PIN_EMITTER_BLUE  21
#define PIN_EMITTER_GREEN 22
#define PIN_EMITTER_RED   23

//#define PIN_VMETER        A0  // read analog voltage
//#define PIN_ACCEL_SDA     A4  // general I2C (blue)
//#define PIN_ACCEL_SCL     A5  // general I2C (green)

enum {
  RED,
  GREEN,
  BLUE,
  NCHANNELS
};

static const float DEFAULT_G_FORCE_MOTION = 1.3f;
static const float DEFAULT_G_FORCE_IMPACT = 2.2f;

#endif // PINS_HEADER

