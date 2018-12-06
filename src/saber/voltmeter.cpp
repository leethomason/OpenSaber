#ifndef _WIN32
#include <Arduino.h>
#include "Grinliz_Arduino_Util.h"
#endif

#include "Grinliz_Util.h"
#include "pins.h"
#include "voltmeter.h"

AveragePower::AveragePower()
{
    for(int i=0; i<NUM_SAMPLES; ++i) {
        m_sample[i] = NOMINAL_VOLTAGE;
    }
    m_power = NOMINAL_VOLTAGE;
}

void AveragePower::push(uint32_t milliVolts)
{
    uint32_t total = milliVolts;
    for(int i=0; i<NUM_SAMPLES-1; ++i) {
        m_sample[i] = m_sample[i+1];
        total += m_sample[i];
    }
    m_sample[NUM_SAMPLES-1] = milliVolts;
    m_power = total / NUM_SAMPLES;
}

#ifndef _WIN32
void Voltmeter::begin()
{
    #ifdef SABER_VOLTMETER
    #ifdef CORE_TEENSY
    analogReference(INTERNAL);  // 1.1 volts
    #else
    analogReference(AR_INTERNAL1V0);    // FIXME: also 1.65 - need to check range.
    #endif
    analogRead(PIN_VMETER);     // warm up the ADC to avoid spurious initial value.
    Log.p("Voltmeter open.").eol();
    delay(50);
    #endif

    for(int i=0; i<AveragePower::NUM_SAMPLES; ++i) {
        delay(10);
        m_averagePower.push(readVBat());
    }
    Log.p("Vbat: ").p(readVBat()).p(" Ave:").p(averagePower()).eol();
    #ifdef SABER_VOLTMETER
    Log.p("Voltmeter initialized.").eol();
    #endif
}


uint32_t Voltmeter::readVBat() 
{
    #ifdef SABER_VOLTMETER
        uint32_t analog = analogRead(PIN_VMETER);
        uint32_t mV = analog * UVOLT_MULT / uint32_t(1000);
        return mV;
    #else
        return NOMINAL_VOLTAGE;
    #endif
}

void Voltmeter::takeSample()
{
    m_averagePower.push(readVBat());
}
#endif


int AveragePower::vbatToPowerLevel(int32_t vbat, int maxLevel)
{
    const int32_t round = (HIGH_VOLTAGE - LOW_VOLTAGE) / (2 * maxLevel);
    int32_t level = maxLevel * (round + vbat - LOW_VOLTAGE) / (HIGH_VOLTAGE - LOW_VOLTAGE);

    if (level < 0) level = 0;
    if (level > maxLevel) level = maxLevel;
    return level;
}
