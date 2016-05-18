#include "pins.h"
#include "electrical.h"
#include "saberUtil.h"

#include <Grinliz_Arduino_Util.h>

void BladeState::change(uint8_t state)
{
    ASSERT(state >= BLADE_OFF && state <= BLADE_RETRACT);
    m_currentState   = state;
    m_startTime = millis();

    switch(m_currentState) {
    case BLADE_OFF:
        Log.event("[BLADE_OFF]");
        break;
    case BLADE_IGNITE:
        Log.event("[BLADE_IGNITE]");
        break;
    case BLADE_ON:
        Log.event("[BLADE_ON]");
        break;
    case BLADE_FLASH:
        Log.event("[BLADE_FLASH]");
        break;
    case BLADE_RETRACT:
        Log.event("[BLADE_RETRACT]");
        break;
    default:
        ASSERT(false);
        break;
    }
}

bool BladeState::bladeOn() const
{
    ASSERT(m_currentState >= BLADE_OFF && m_currentState <= BLADE_RETRACT);
    return m_currentState >= BLADE_ON && m_currentState < BLADE_RETRACT;
}

void ButtonMode::nextMode()
{
    m_mode = (m_mode + 1) % NUM_BUTTON_MODES;
}

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


