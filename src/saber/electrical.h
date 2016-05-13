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

#ifndef ELECTRICAL_SABER_INCLUDED
#define ELECTRICAL_SABER_INCLUDED

#include <stdint.h>

#define TEST_EMITTER 0
#define SENTRIS_EMITTER 1
#define EMITTER SENTRIS_EMITTER

// Physical values (per saber / testbed!)
static const int32_t NOMINAL_VOLTAGE = 3700;

/*
 * This is the code if using a voltage divider.
 * Much simpler, slightly less accurate, fewer components.
 * I did have the equation for computing the voltage,
 * but I usually just take a meter, measure, and tweak 
 * the constant.
 */
 static const int32_t UVOLT_MULT = 6680;

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

#define BLADE_AXIS 1
#define NORMAL_AXIS_A 0
#define NORMAL_AXIS_B 2

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
static const int32_t RED_R    = 2400;   // milli-ohms

static const int32_t GREEN_VF = 3150;
static const int32_t GREEN_I  = 350;
static const int32_t GREEN_R  = 1800;

static const int32_t BLUE_VF  = 3150;
static const int32_t BLUE_I   = 350;
static const int32_t BLUE_R   = 1800;

#endif

static const int HIGH_VOLTAGE = 4200;
static const int LOW_VOLTAGE = 3500;

#endif // ELECTRICAL_SABER_INCLUDED

