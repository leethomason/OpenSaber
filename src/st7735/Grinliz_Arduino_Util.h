#ifndef CSTR_INCLUDED
#define CSTR_INCLUDED

#include <Arduino.h>
#include "Grinliz_Util.h"

class SPISettings;

class LEDManager;
typedef void (*BlinkHandler)(const LEDManager&);

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

struct ProfileData
{
    ProfileData(const char* _name) { 
        name = _name;
        next = root;
        root = this; 
    }

    const char* name = 0;
    ProfileData* next = 0;

    uint32_t nCalls;
    uint32_t totalTime;
    uint32_t maxTime;
    uint32_t startTime;

    static ProfileData* root;
};

void ProfileStart(ProfileData* data);
void ProfileEnd(ProfileData* data);
void DumpProfile();

class ProfileBlock
{
public:
    ProfileBlock(ProfileData* data) {
        m_data = data;
        ProfileStart(m_data);
    }

    ~ProfileBlock() {
        ProfileEnd(m_data);
    }

private:
    ProfileData* m_data;
};

#endif // CSTR_INCLUDED
