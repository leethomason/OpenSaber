#ifndef SABER_UTIL_INCLUDED
#define SABER_UTIL_INCLUDED

#include <stdint.h>

enum {  
	BLADE_OFF,
    BLADE_IGNITE,
    BLADE_ON,
    BLADE_FLASH,
    BLADE_RETRACT
 };

class BladeState
{
public:
	BladeState() {}

     void change(uint8_t newState);
     const int state() const { return m_currentState; }
     bool bladeOn() const;
     const uint32_t startTime() const { return m_startTime; }

private:
	uint8_t  m_currentState = BLADE_OFF;
	uint32_t m_startTime = 0;
};


#endif // SABER_UTIL_INCLUDED
