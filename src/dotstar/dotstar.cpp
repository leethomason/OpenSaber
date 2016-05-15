#include "dotstar.h"
#include <Arduino.h>
#include <SPI.h>

static const SPISettings dotstarSettings(8000000, MSBFIRST, SPI_MODE0);

DotStar::DotStar(uint8_t enable)
{
	m_enable = enable;
	pinMode(m_enable, OUTPUT);
	digitalWrite(m_enable, LOW);	// opposite of normal CS
}

void DotStar::begin()
{
	SPI.begin();
}

void DotStar::display()
{
	Serial.print("display nLeds="); Serial.println(m_nLEDs);
	SPI.beginTransaction(dotstarSettings);
	digitalWrite(m_enable, HIGH);

	// Frame marker.
	for(int i=0; i<4; ++i) {
		SPI.transfer(0);
	}

	const RGB* led = m_leds;
	for(int i=0; i<m_nLEDs; ++i, ++led) {
		// Pixel start:
		SPI.transfer(0xff);

		// Value:
		SPI.transfer((uint16_t(led->blue)  * m_brightness) >> 8);
		SPI.transfer((uint16_t(led->green) * m_brightness) >> 8);
		SPI.transfer((uint16_t(led->red)   * m_brightness) >> 8);
	}
	// End frame.
	for(int i=0; i<4; ++i) {
		SPI.transfer(0xff);
	}

	digitalWrite(m_enable, LOW);
    SPI.endTransaction();
}



