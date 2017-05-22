#ifndef CSTR_INCLUDED
#define CSTR_INCLUDED

#include "Grinliz_Util.h"
#include <Arduino.h>

class SPISettings;

class LEDManager;
typedef void (*BlinkHandler)(const LEDManager&);
struct RGB;

class LEDManager
{
public:
	enum {
		BLINK_TRAILING,
		BLINK_LEADING,
	};
    LEDManager(uint8_t pin, bool on=true);

    void set(bool on);
    void blink(uint8_t n, uint32_t cycle, BlinkHandler handler = 0, uint8_t leadOrTrail = BLINK_LEADING);
    void process();
    int  numBlinks() const;
    bool blinking() const {
        return m_nBlink > 0;
    }

private:
    uint8_t  m_pin = 0;
    uint8_t  m_nBlink = 0;
    bool 	 m_on = false;
    uint8_t  m_bias = BLINK_LEADING;
    uint32_t m_cycle = 0;
    uint32_t m_startTime = 0;
    BlinkHandler m_handler = 0;
    uint8_t m_nCallbacks = 0;
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

class SPLog
{
public:
	void attachSerial(Stream* stream);
	void attachLog(Stream* stream);

	const SPLog& p(const char v[]) const;
    const SPLog& p(char v) const;
    const SPLog& p(unsigned char v, int p = DEC) const;
    const SPLog& p(int v, int p = DEC) const;
    const SPLog& p(unsigned int v, int p = DEC) const;
    const SPLog& p(long v, int p = DEC) const;
    const SPLog& p(unsigned long v, int p = DEC) const;
    const SPLog& p(double v, int p = 2) const;
    const SPLog& p(const RGB& rgb) const;
    void eol() const;

    void event(const char* event);
    void event(const char* event, const char* data);
    void event(const char* event, int data);

    const char* popEvent(const char** name, const char** data = 0, int* dataI = 0);
    bool hasEvent() const { return _hasEvent; }

private:
	Stream* serialStream = 0;
	Stream* logStream = 0;  

    bool     _hasEvent = false;
	CStr<40> eventName;
	CStr<40> eventStrData;
	int      eventIData;
};

extern SPLog Log;
bool TestEvent();

#endif // CSTR_INCLUDED
