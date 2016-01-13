#ifndef PINS_HEADER
#define PINS_HEADER

static const int EEPROM_SIZE = 512;

// --- Configuration ---
// Note: Serial connection should be set to 19200 baud with a newline after commands.
#define SERIAL_DEBUG 1



#define SABER_ACCELEROMETER
#define SABER_SOUND_ON
//#define SABER_VOLTMETER
#define SABER_LOGGING
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

/* Adafruit Trinket Pro 3V */
// Reserve serial         0
// Reserve serial         1
// Trinket no pin 2
#define PIN_SWITCH_A      3   // switch - power (also PWM)
#define PIN_LED_A         4   // switch LED - power
// Unused                 5
#define PIN_LED_B         6   // switch LED - audio
// Trinket no pin 7
#define PIN_SFX_RST       8
#define PIN_EMITTER_BLUE  9   // T1 (get blue and green in different phase!)
#define PIN_EMITTER_RED   10  // T1
#define PIN_EMITTER_GREEN 11  // T2 
#define PIN_SFX_ACTIVITY  12
#define PIN_LED_LOW_POWER 13
#define PIN_VMETER        A0  // read analog voltage
#define PIN_SWITCH_B      A1  // switch - audio
#define PIN_SFX_RX        A2
#define PIN_SFX_TX        A3
#define PIN_ACCEL_SDA     A4  // general I2C (blue)
#define PIN_ACCEL_SCL     A5  // general I2C (green)
// Avoid A6                   // UNO doesn't have A6 and A7. Trinket does...
// Avoid A7

/* Adafruit Audio FX */
// Reset                  to PIN_SFX_RST
// Activity               to PIN_SFX_ACTIVITY
// UG                     to ground (UART mode)


enum {
  RED,
  GREEN,
  BLUE,
  NCHANNELS
};

static const float DEFAULT_G_FORCE_MOTION = 1.3f;
static const float DEFAULT_G_FORCE_IMPACT = 2.2f;

#endif // PINS_HEADER

