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

LEDManager::LEDManager(uint8_t pin) 
{
	m_pin = pin;
  	pinMode(m_pin, OUTPUT);
  	digitalWrite(m_pin, HIGH);
}


void LEDManager::set(bool on) 
{
	m_on = on;
  	digitalWrite(m_pin, m_on ? HIGH : LOW );
  	m_nBlink = 0;
}

void LEDManager::blink(uint8_t n, uint32_t cycle)
{
	m_nBlink = n;
	m_cycle = cycle > 0 ? cycle : 1;
	m_startTime = millis();
}

void LEDManager::process()
{
	if (m_nBlink) {
		uint32_t n = (millis() - m_startTime) / m_cycle;
		const uint32_t half = m_cycle / 2;
		uint32_t p = (millis() - m_startTime) / half;

		if (n > m_nBlink) {
			m_nBlink = 0;
		  	digitalWrite(m_pin, m_on ? HIGH : LOW );
		}
		else {
		  	digitalWrite(m_pin, (p & 1) ? HIGH : LOW );
		}
	}
}

int LEDManager::numBlinks() const
{
	if (m_startTime == 0) return 0;

	uint32_t n = (millis() - m_startTime) / m_cycle;
	return n + 1;
}