#include "dotstar.h"
#include <Arduino.h>
#include <SPI.h>

// 8 MHz seems to work for the prop shield.
// With a level shifter, about 1 MHz is needed. (Possibly faster with testing.)
static const SPISettings dotstarSettings(8000000, MSBFIRST, SPI_MODE0);

DotStar::DotStar()
{
}

void DotStar::beginSPI(uint8_t enable)
{
	m_enable = enable;
	pinMode(m_enable, OUTPUT);
	digitalWrite(m_enable, LOW);
	SPI.begin();
}

void DotStar::beginSW(uint8_t clockPin, uint8_t dataPin)
{
	m_clockPin = clockPin;
	m_dataPin = dataPin;

	pinMode(m_dataPin , OUTPUT);
	pinMode(m_clockPin, OUTPUT);
	digitalWrite(m_dataPin , LOW);
	digitalWrite(m_clockPin, LOW);	
}

void DotStar::swOut(uint8_t n) 
{
	for(uint8_t i=8; i--; n <<= 1) {
    	if(n & 0x80) 
    		digitalWrite(m_dataPin, HIGH);
    	else         
    		digitalWrite(m_dataPin, LOW);

    	digitalWrite(m_clockPin, HIGH);
    	digitalWrite(m_clockPin, LOW);
	}
}

void DotStar::display()
{
	if (swMode()) displaySW();
	else displaySPI();
}

void DotStar::displaySPI()
{
	//Serial.print("display nLeds="); Serial.println(m_nLEDs);
	SPI.beginTransaction(dotstarSettings);
	if (m_enable != 255)
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
		SPI.transfer((uint16_t(led->b) * m_brightness) >> 8);
		SPI.transfer((uint16_t(led->g) * m_brightness) >> 8);
		SPI.transfer((uint16_t(led->r) * m_brightness) >> 8);
	}
	// End frame.
	for(int i=0; i<4; ++i) {
		SPI.transfer(0xff);
	}

	if (m_enable != 255)
		digitalWrite(m_enable, LOW);
    SPI.endTransaction();
}


void DotStar::displaySW()
{
	//Serial.print("display nLeds="); Serial.println(m_nLEDs);
	if (m_enable != 255)
		digitalWrite(m_enable, HIGH);

	// Frame marker.
	for(int i=0; i<4; ++i) {
		swOut(0);
	}

	const RGB* led = m_leds;
	for(int i=0; i<m_nLEDs; ++i, ++led) {
		// Pixel start:
		swOut(0xff);

		// Value:
		swOut((uint16_t(led->b) * m_brightness) >> 8);
		swOut((uint16_t(led->g) * m_brightness) >> 8);
		swOut((uint16_t(led->r) * m_brightness) >> 8);
	}
	// End frame.
	for(int i=0; i<4; ++i) {
		swOut(0xff);
	}

	if (m_enable != 255)
		digitalWrite(m_enable, LOW);
}


