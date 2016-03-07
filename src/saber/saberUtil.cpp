#include "saberUtil.h"
#include <Grinliz_Arduino_Util.h>

void BladeState::change(uint8_t state)
{
	ASSERT(state >= BLADE_OFF && state <= BLADE_RETRACT);
  	m_currentState   = state;
  	m_startTime = millis();
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

