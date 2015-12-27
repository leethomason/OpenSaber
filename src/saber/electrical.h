#ifndef ELECTRICAL_SABER_INCLUDED
#define ELECTRICAL_SABER_INCLUDED

#define TEST_EMITTER 0
#define SENTRIS_EMITTER 1
#define EMITTER SENTRIS_EMITTER

// Physical values (per saber / testbed!)
static const int32_t NOMINAL_VOLTAGE = 3700;

/*
 * This is the code if using a voltage regulator.
 *
 * Converts A0 in to millivolts.
 * AnalogPin returns 0-1023
 * Reference is 2.5V
 * Vin = Vref * (R1 + R2) / R2
 * R1 = 6.8k R2 = 4.7k
 * Vin = 2.5 * 2.447 = 6.12
 * In this case, 1023 is 6117mv or 6,117,000uV
 * mult = 5980, measured 5960 
static const int32_t UVOLT_MULT = 5980;  // micro-volts
 */

/*
 * This is the code if using a voltage divider.
 * Much simpler, slightly less accurate, fewer components.
 * 
 * Vout = Vbat * R1 / (R1 + R2),
 * Vbat = Vout * (R1 + R2) / R1
 * Vref = 1.1 (internal)
 * Vout = Vref * Apin
 * Vbat = Vref * Apin * (R1 + R2) / R1
 *      R1 = 10k, R2 = 47k, (R1+R2)/R1 = 5.7
 *      = 1.1 * A * 5.7
 *      = 6.27 * A
 *  Unit conversion: 6270 = 1023 * MULT / 1000
 *  UVOLT_MULT = 6129
 *  But we measure 5.10 volts.
 *  Correcting: 
 */
 static const int32_t UVOLT_MULT = 5990;

#if EMITTER == TEST_EMITTER
#define ID_STR "Test Emitter"
static const int32_t LED_RANGE = 80;  // 255 for full range

static const int32_t RED_VF   = 2100;                         // milli-volts
static const int32_t RED_I    = 20;                           // milli-amps
static const int32_t RED_R    = int32_t(100) * int32_t(1000); // milli-ohms

static const int32_t GREEN_VF = 2100;
static const int32_t GREEN_I  = 20;
static const int32_t GREEN_R  = int32_t(100) * int32_t(1000);

static const int32_t BLUE_VF  = 2100;
static const int32_t BLUE_I   = 20;
static const int32_t BLUE_R   = int32_t(47) * int32_t(1000);

#elif EMITTER == SENTRIS_EMITTER
#define ID_STR "Gecko (Sentris Body) RGB Luxeon"
static const int32_t LED_RANGE = 255;  // 255 for full range

/*
 * Luxeon Rebel Endor (Tri-LED)
 * Nominal values. (Significant variation when measured.)
 * R 2.90 @ 350 R 331 LED 1015 75% Flux  48  [2.7 ohm 1W ]
 * G 3.15 @ 350 R 221 LED 1103 83% Flux  102 [1.8 ohm 1/2W]
 * B 3.15 @ 350 R 221 LED 1103 83% Flux  41  [1.8 ohm 1/2W]
 * Flux/Watt: (191/3.885) = 49.2
 */
static const int32_t RED_VF   = 2900;   // milli-volts
static const int32_t RED_I    = 350;    // milli-amps
static const int32_t RED_R    = 2700;   // milli-ohms

static const int32_t GREEN_VF = 3150;
static const int32_t GREEN_I  = 350;
static const int32_t GREEN_R  = 1800;

static const int32_t BLUE_VF  = 3150;
static const int32_t BLUE_I   = 350;
static const int32_t BLUE_R   = 1800;

#endif
// FIXME: needs sound
static const int BASE_AMPS   = 0;

static const int LOW_VOLTAGE = 3500;

#endif // ELECTRICAL_SABER_INCLUDED
