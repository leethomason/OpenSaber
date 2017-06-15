#ifndef GRINLIZ_RF24_INCLUDED
#define GRINLIZ_RF24_INCLUDED

#include <stdint.h>
#include "Grinliz_Util.h"

class ComRF24
{
public:
	ComRF24(uint8_t cePin, uint8_t csPin);
	bool begin(int role, const char* addr0, const char* addr1);

	void write(const char* str);
	void process(CStr<16>* str);

	void test();

private:


};

#endif // GRINLIZ_RF24_INCLUDED
