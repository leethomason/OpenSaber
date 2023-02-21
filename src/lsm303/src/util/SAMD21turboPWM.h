#include <Arduino.h>

#define GCLK_GENCTRL_SRC_DPLL96M_Val 0x8ul
#define GCLK_GENCTRL_SRC_DPLL96M (GCLK_GENCTRL_SRC_DPLL96M_Val << GCLK_GENCTRL_SRC_Pos)

class TurboPWM {
  public:
    void setClockDivider(unsigned int GCLKDiv, bool turbo);
    int timer(unsigned int timernumber, unsigned int TCCDiv, unsigned long long int steps, bool fastPWM);
    int analogWrite(int pin, unsigned int dutyCycle);
    int enable(unsigned int timerNumber, bool enabled);
    float frequency(unsigned int timerNumber);
  private:
    unsigned int _GCLKDiv = 1;                // Main clock divider: 1 to 255 for both TCC0 and TCC1
    bool _turbo = false;                      // False for 48MHz clock, true for 96MHz clock
    const unsigned int _maxDutyCycle = 1000;  // The maximum duty cycle number; duty cycle will be (dutyCycle / _maxDutyCycle) * 100%
};

//Table for looking up and storing values for TCCx
typedef struct {
  const Tcc* TCCx;                      // Pointer to timer
  const RwReg* REG_TCCx_CTRLA;          // Pointer to timer's CTRLA register
  const RwReg* REG_TCCx_WAVE;           // Pointer to timer's WAVE register
  const RwReg* REG_TCCx_PERB;           // Pointer to timer's PERB register
  const unsigned long int counterSize;  // Timer's counter size: 24 bits for TCC0 and TCC1, 16 bits for TCC2
  unsigned int TCCDiv;                  // Timer's clock divider: 1, 2, 4, 8, 16, 64, 256, or 1024
  unsigned long long int steps;         // Timer's PWM steps (resolution): 2 to counterSize
  bool fastPWM;                         // False for phase-correct aka dual-slope PWM, true for fast aka normal aka single-slope PWM
  bool enabled;                         // Shows if TCCx should be enabled
} TimerLookup;

static TimerLookup timerTable[] = {
  {TCC0, &REG_TCC0_CTRLA, &REG_TCC0_WAVE, &REG_TCC0_PERB, 0xFFFFFF, 1, 500000, false, true},
  {TCC1, &REG_TCC1_CTRLA, &REG_TCC1_WAVE, &REG_TCC1_PERB, 0xFFFFFF, 1, 500000, false, true},
  {TCC2, &REG_TCC2_CTRLA, &REG_TCC2_WAVE, &REG_TCC2_PERB,   0xFFFF, 1,  50000, false, true}
};
static const unsigned int timerTableSize = sizeof(timerTable) / sizeof(timerTable[0]);

// Tables for looking up pin mappings etc. for different boards
typedef struct { 
  const int arduinoPin;                 // Arduino pin number
  const unsigned int port;              // Port of the SAMD21 pin
  const unsigned int samd21Pin;         // SAMD21 pin
  const unsigned int timer;             // Timer used for this pin
  const RwReg* REG_TCCx_CCBy;           // Pointer to count register used for this pin
  const unsigned long int pMux;         // Pin multiplexer for this pin
} PinLookup;

static const PinLookup pinTable[] = {
#if defined (ARDUINO_SAMD_NANO_33_IOT)
//Table begin
{-1, 0, 0, 0, 0, 0},
{-1, 0, 0, 0, 0, 0}, 
{-1, 0, 0, 0, 0, 0}, 
{-1, 0, 0, 0, 0, 0},
{ 4, PORTA,  7, 1, &REG_TCC1_CCB1, PORT_PMUX_PMUXO_E},
{ 5, PORTA,  5, 0, &REG_TCC0_CCB1, PORT_PMUX_PMUXO_E},
{ 6, PORTA,  4, 0, &REG_TCC0_CCB0, PORT_PMUX_PMUXE_E},
{ 7, PORTA,  6, 1, &REG_TCC1_CCB0, PORT_PMUX_PMUXE_E},
{ 8, PORTA, 18, 0, &REG_TCC0_CCB2, PORT_PMUX_PMUXE_F},
{-1, 0, 0, 0, 0, 0}, 
{-1, 0, 0, 0, 0, 0}, 
{11, PORTA, 16, 2, &REG_TCC2_CCB0, PORT_PMUX_PMUXE_E}, 
{12, PORTA, 19, 0, &REG_TCC0_CCB3, PORT_PMUX_PMUXO_F},
{13, PORTA, 17, 2, &REG_TCC2_CCB1, PORT_PMUX_PMUXO_E}
//Table end

#elif defined (ARDUINO_SAMD_ZERO) || \
      defined (ADAFRUIT_ITSYBITSY_M0) || \
      defined (ARDUINO_SAMD_FEATHER_M0)
//Table begin
{-1, 0, 0, 0, 0, 0},
{-1, 0, 0, 0, 0, 0},
{-1, 0, 0, 0, 0, 0},
{ 3, PORTA,  9, 0, &REG_TCC0_CCB1, PORT_PMUX_PMUXO_E},
{ 4, PORTA,  8, 0, &REG_TCC0_CCB0, PORT_PMUX_PMUXE_E},
{-1, 0, 0, 0, 0, 0},
{-1, 0, 0, 0, 0, 0},
{-1, 0, 0, 0, 0, 0},
{ 8, PORTA,  6, 1, &REG_TCC1_CCB0, PORT_PMUX_PMUXE_E},
{ 9, PORTA,  7, 1, &REG_TCC1_CCB1, PORT_PMUX_PMUXO_E},
{10, PORTA, 18, 0, &REG_TCC0_CCB2, PORT_PMUX_PMUXE_F},
{11, PORTA, 16, 2, &REG_TCC2_CCB0, PORT_PMUX_PMUXE_E},
{12, PORTA, 19, 0, &REG_TCC0_CCB3, PORT_PMUX_PMUXO_F},
{13, PORTA, 17, 2, &REG_TCC2_CCB1, PORT_PMUX_PMUXO_E}
//Table end

#elif defined (ARDUINO_SAMD_MKRZERO) || \
      defined (ARDUINO_SAMD_MKR1000) || \
      defined (ARDUINO_SAMD_MKRWIFI1010) || \
      defined (ARDUINO_SAMD_MKRFox1200) || \
      defined (ARDUINO_SAMD_MKRWAN1300) || \
      defined (ARDUINO_SAMD_MKRWAN1310) || \
      defined (ARDUINO_SAMD_MKRGSM1400) || \
      defined (ARDUINO_SAMD_MKRNB1500) || \
      defined (ARDUINO_SAMD_MKRVIDOR4000)
//Table begin
{-1, 0, 0, 0, 0, 0},
{-1, 0, 0, 0, 0, 0},
{ 2, PORTA, 10, 1, &REG_TCC1_CCB0, PORT_PMUX_PMUXE_E}, 
{ 3, PORTA, 11, 1, &REG_TCC1_CCB1, PORT_PMUX_PMUXO_E},
{ 4, PORTB, 10, 0, &REG_TCC0_CCB0, PORT_PMUX_PMUXE_F},
{ 5, PORTB, 11, 0, &REG_TCC0_CCB1, PORT_PMUX_PMUXO_F},
{ 6, PORTA, 20, 0, &REG_TCC0_CCB2, PORT_PMUX_PMUXE_F},
{ 7, PORTA, 21, 0, &REG_TCC0_CCB3, PORT_PMUX_PMUXO_F},
{ 8, PORTA, 16, 2, &REG_TCC2_CCB0, PORT_PMUX_PMUXE_E},
{ 9, PORTA, 17, 2, &REG_TCC2_CCB1, PORT_PMUX_PMUXO_E}
//Table end

#elif defined (ADAFRUIT_TRINKET_M0)
//Table begin
{ 0, PORTA,  8, 0, &REG_TCC0_CCB0, PORT_PMUX_PMUXE_E},
{-1, 0, 0, 0, 0, 0}, //DAC
{ 2, PORTA,  9, 0, &REG_TCC0_CCB1, PORT_PMUX_PMUXO_E},
{ 3, PORTA,  7, 1, &REG_TCC1_CCB1, PORT_PMUX_PMUXO_E},
{ 4, PORTA,  6, 1, &REG_TCC1_CCB0, PORT_PMUX_PMUXE_E}
//Table end

#else
  #error Board not supported by Turbo PWM Library
#endif
};
static const unsigned int pinTableSize = sizeof(pinTable) / sizeof(pinTable[0]);
