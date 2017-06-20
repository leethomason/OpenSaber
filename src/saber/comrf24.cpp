#include "comrf24.h"


ComRF24::ComRF24(RF24* rf24)
{
	m_rf24 = rf24;
}


bool ComRF24::begin(int role)
{
	static const byte ADDR[] = {
		"1Sist",
		"2Sist"
	};

	m_rode = role;

	m_rf24->begin();
	m_rf24->setPALevel(RF24_PA_LOW);

	if(m_rode){
		m_rf24->openWritingPipe(ADDR[1]);
		m_rf24->openReadingPipe(1, ADDR[0]);
	}else{
		m_rf24->openWritingPipe(ADDR[0]);
		m_rf24->openReadingPipe(1, ADDR[1]);
	}

 	// Start the radio listening for data
	m_rf24->startListening();
}


void ComRF24::process(CStr<16>* str)
{
	str->clear();
	if (radio.available()) {
		char c = 255;
		while(c) {
			radio.read(&c, 1);
			str->append(c);
		}
	}
	if (!str->empty()) {
		Log.p("ComRF24 read:").p(str->c_str()).eol();
	}
}


void ComRF24::write(const char* str)
{
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