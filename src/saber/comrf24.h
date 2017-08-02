#ifndef GRINLIZ_RF24_INCLUDED
#define GRINLIZ_RF24_INCLUDED

#include <stdint.h>
#include "Grinliz_Util.h"

class RF24;

class ComRF24
{
public:
	ComRF24(RF24* rf24);
	bool begin(int role);

	bool inUse() const { return m_instance != 0; }
	bool isConnected() const;
	int role() const { return m_role; }

	bool send(const char* str);
	void process(CStr<16>* str);

	void setComLog(bool enabled) { m_log = enabled; }
	bool isComLogging() const { return m_log; }

    static ComRF24* instance() { return m_instance; }

private:
	int m_role = 0;
	static ComRF24* m_instance;
	RF24* m_rf24 = 0;
	bool m_log = false;

	// maybe enable later.
	// error correction is nice, but adds complexity
	//int m_tries = 0;
	//CStr<10> m_resend;
};

#endif // GRINLIZ_RF24_INCLUDED
