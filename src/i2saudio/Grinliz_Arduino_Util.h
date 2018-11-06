#ifndef CSTR_INCLUDED
#define CSTR_INCLUDED

#include "Grinliz_Util.h"
#include <Arduino.h>

class SPISettings;

class LEDManager;
typedef void (*BlinkHandler)(const LEDManager&);
namespace osbr {
struct RGB;
}

class LEDManager
{
public:
	enum {
		BLINK_TRAILING,
		BLINK_LEADING,
        BLINK_BREATH
	};
    LEDManager(uint8_t pin, bool on=true);

    void set(bool on);
    bool isOn() const;

    void blink(uint8_t n, uint32_t cycle, BlinkHandler handler = 0, uint8_t style = BLINK_LEADING);
    int  numBlinks() const;
    bool blinking() const {
        return m_nBlink > 0;
    }

    void process();

private:
    uint8_t  m_pin = 0;
    uint8_t  m_nBlink = 0;
    bool 	 m_on = false;
    uint8_t  m_style = BLINK_LEADING;
    uint32_t m_cycle = 0;
    uint32_t m_startTime = 0;
    BlinkHandler m_handler = 0;
    uint8_t m_nCallbacks = 0;
    bool m_analogMode = false;   // Teensy has a bug; once it goes analog, it STAYS analog
};

// --- Interupts & Time --- //

class SPITransaction
{
public:
    SPITransaction(uint8_t cs, const SPISettings& settings);
    ~SPITransaction();

private:
    uint8_t cs;
};


#endif // CSTR_INCLUDED
