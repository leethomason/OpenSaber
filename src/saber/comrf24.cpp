#include "comrf24.h"
#include "Grinliz_Util.h"
#include "Grinliz_Arduino_Util.h"
#include "RF24.h"
#include "pins.h"


ComRF24::ComRF24(RF24* rf24)
{
	m_rf24 = rf24;
}


bool ComRF24::begin(int role)
{
	if (!m_rf24) return false;

	static const uint8_t* ADDR0 = (const uint8_t*) SABER_ADDR_0;
	static const uint8_t* ADDR1 = (const uint8_t*) SABER_ADDR_1;

	m_role = role;

	m_rf24->begin();
	m_rf24->setPALevel(RF24_PA_LOW);
	#if (SERIAL_DEBUG == 1) 
	//m_rf24->printDetails();
	Log.p("PA Level (0-3) : ").p(m_rf24->getPALevel()).eol();
	Log.p("Data rate (0-2): ").p((int)m_rf24->getDataRate()).eol();
	Log.p("isConnected    : ").p(m_rf24->isChipConnected() ? "true" : "false").eol();
	#endif

	if(m_role){
		m_rf24->openWritingPipe(ADDR1);
		m_rf24->openReadingPipe(1, ADDR0);
	}else{
		m_rf24->openWritingPipe(ADDR0);
		m_rf24->openReadingPipe(1, ADDR1);
	}

	m_rf24->startListening();

	return m_rf24->isChipConnected();
}


void ComRF24::process(CStr<16>* str)
{
	if (!m_rf24) return;

	static const int BUF = 16;
	char buf[BUF];

	if (m_rf24->available()) {
		uint8_t s = m_rf24->getPayloadSize();
		if ( s > BUF) s = BUF;

		m_rf24->read(&buf, s);
		if (s < BUF)
			buf[s] = 0;
		else
			buf[BUF-1] = 0;
		*str = buf;
		Log.p("ComRF24 read:").p(str->c_str()).eol();
	}
}


void ComRF24::send(const char* str)
{
	if (!m_rf24) return;
	if (!str || !*str) return;

	int len = 0;
	for(const char* p = str; *p; ++p, ++len) {}

    m_rf24->stopListening();
    if (!m_rf24->write(str, len + 1)) {
    	Log.p("ComRF24 write failed.").eol();
    }
    else {
    	Log.p("ComRF24 write:").p(str).p(" len=").p(len).eol();
    }
    m_rf24->startListening();
}


void ComRF24::test()
{

}