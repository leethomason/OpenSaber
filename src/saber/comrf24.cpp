#include "comrf24.h"
#include "Grinliz_Util.h"
#include "Grinliz_Arduino_Util.h"
#include "RF24.h"


ComRF24::ComRF24(RF24* rf24)
{
	m_rf24 = rf24;
}


bool ComRF24::begin(int role)
{
	if (!m_rf24) return false;

	static const uint8_t* ADDR0 = (const uint8_t*) "1Sist";
	static const uint8_t* ADDR1 = (const uint8_t*) "2Sist";

	m_role = role;

	m_rf24->begin();
	m_rf24->setPALevel(RF24_PA_LOW);

	if(m_role){
		m_rf24->openWritingPipe(ADDR1);
		m_rf24->openReadingPipe(1, ADDR0);
	}else{
		m_rf24->openWritingPipe(ADDR0);
		m_rf24->openReadingPipe(1, ADDR1);
	}

	m_rf24->startListening();
	return true;
}


void ComRF24::process(CStr<16>* str)
{
	if (!m_rf24) return;

	str->clear();
	if (m_rf24->available()) {
		char c = 255;
		while(c) {
			m_rf24->read(&c, 1);
			str->append(c);
		}
	}
	if (!str->empty()) {
		Log.p("ComRF24 read:").p(str->c_str()).eol();
	}
}


void ComRF24::write(const char* str)
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
    	Log.p("ComRF24 write:").p(str).eol();
    }
    m_rf24->startListening();
}


void ComRF24::test()
{

}