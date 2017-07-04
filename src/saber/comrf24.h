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

	int role() const { return m_role; }

	void send(const char* str);
	void process(CStr<16>* str);

	void test();

private:
	int m_role = 0;
	RF24* m_rf24 = 0;
};

#endif // GRINLIZ_RF24_INCLUDED
