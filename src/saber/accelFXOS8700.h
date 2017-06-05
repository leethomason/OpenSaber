#ifndef ACCEL_FXOS8700_INCLUDED
#define ACCEL_FXOS8700_INCLUDED

#include <stdint.h>

class AccelFXOS8700
{
public:
	enum ErrorCode {
		SUCCESS = 0,
		ERROR_RANGE_INVALID,

		// The following should be hardware failures.
		// Not wired correctly, likely.
		ERROR_CONNECTION
	};

	enum Range {
		RANGE_2G,
		RANGE_4G,
		RANGE_8G
	};

	AccelFXOS8700() {}

	enum {
		ADDRESS_ADAFRUIT = 0x1f,
		ADDRESS_PROP_SHIELD = 0x1e
	};

	/** Start it up!
		@param range 2, 4, or 8g
		@param deviceAddress the IC address of the device. 
			   0x1f for Adafruit (?) and 0x01e for the Teensy Prop Shield.
	*/
	ErrorCode begin(Range range = Range::RANGE_4G, uint8_t deviceAddress = ADDRESS_ADAFRUIT);

	/**
		Read the accelerometer.
		@param v 				A vector of 3 floats for the x, y, z result.
		@param gravitySquared 	The gravity value squared. Fast to calculate.
		@param gravity 			Gravity (in m/s^2) a little slower to calculate. However,
								in practice, most of the time goes to waiting for
								the relatively slow I2C interface.
	*/
	ErrorCode read(float* v, float* gravitySquared = 0, float* gravity = 0);

private:
	ErrorCode read8(uint8_t reg, uint8_t* value);
	ErrorCode write8(uint8_t reg, uint8_t value);

	uint8_t m_range = 2;
	uint8_t m_address = 0x1f; 
};

#endif