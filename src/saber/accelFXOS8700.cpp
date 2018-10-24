#include "accelFXOS8700.h"
#if SABER_ACCELEROMETER == SABER_ACCELEROMETER_NXP

#include <Wire.h>
#include <math.h>

enum
{                                             // DEFAULT    TYPE
	FXOS8700_REGISTER_STATUS          = 0x00,
	FXOS8700_REGISTER_OUT_X_MSB       = 0x01,
	FXOS8700_REGISTER_OUT_X_LSB       = 0x02,
	FXOS8700_REGISTER_OUT_Y_MSB       = 0x03,
	FXOS8700_REGISTER_OUT_Y_LSB       = 0x04,
	FXOS8700_REGISTER_OUT_Z_MSB       = 0x05,
	FXOS8700_REGISTER_OUT_Z_LSB       = 0x06,
	FXOS8700_REGISTER_WHO_AM_I        = 0x0D,   // 11000111   r
	FXOS8700_REGISTER_XYZ_DATA_CFG    = 0x0E,
	FXOS8700_REGISTER_CTRL_REG1       = 0x2A,   // 00000000   r/w
	FXOS8700_REGISTER_CTRL_REG2       = 0x2B,   // 00000000   r/w
	FXOS8700_REGISTER_CTRL_REG3       = 0x2C,   // 00000000   r/w
	FXOS8700_REGISTER_CTRL_REG4       = 0x2D,   // 00000000   r/w
	FXOS8700_REGISTER_CTRL_REG5       = 0x2E,   // 00000000   r/w
	FXOS8700_REGISTER_MSTATUS         = 0x32,
	FXOS8700_REGISTER_MOUT_X_MSB      = 0x33,
	FXOS8700_REGISTER_MOUT_X_LSB      = 0x34,
	FXOS8700_REGISTER_MOUT_Y_MSB      = 0x35,
	FXOS8700_REGISTER_MOUT_Y_LSB      = 0x36,
	FXOS8700_REGISTER_MOUT_Z_MSB      = 0x37,
	FXOS8700_REGISTER_MOUT_Z_LSB      = 0x38,
	FXOS8700_REGISTER_MCTRL_REG1      = 0x5B,   // 00000000   r/w
	FXOS8700_REGISTER_MCTRL_REG2      = 0x5C,   // 00000000   r/w
	FXOS8700_REGISTER_MCTRL_REG3      = 0x5D,   // 00000000   r/w
};

AccelFXOS8700::ErrorCode AccelFXOS8700::begin(uint8_t deviceAddress)
{
	Wire.begin();

	m_range = 4;
	m_address = deviceAddress;

	uint8_t id = 0;
	AccelFXOS8700::ErrorCode e = SUCCESS;	

	read8(FXOS8700_REGISTER_WHO_AM_I, &id);
  	if (id != 0xC7) {
		#ifdef SERIAL_DEBUG
			Serial.println("FXOS8700_REGISTER_WHO_AM_I failed. line: ");
		#endif
  		return ERROR_CONNECTION;
  	}

  	// standby
  	write8(FXOS8700_REGISTER_CTRL_REG1, 0);

	// High resolution
	write8(FXOS8700_REGISTER_CTRL_REG2, 0x02);

	write8(FXOS8700_REGISTER_XYZ_DATA_CFG, 0x01);

	delay(20);

  	// 							bits
  	// 100hz 					3-5
  	// reduced noise 			2
  	// full read8 				1
  	// active 					0
	write8(FXOS8700_REGISTER_CTRL_REG1, 0x15);

	// Make sure we got 4g
	uint8_t r = 255;
	read8(FXOS8700_REGISTER_XYZ_DATA_CFG, &r);
	if (r != 1) {
		#ifdef SERIAL_DEBUG
			Serial.println("FXOS8700_REGISTER_XYZ_DATA_CFG failed. line: ");
		#endif
		return RANGE_INCORRECT;
	}

  	return e;
}


AccelFXOS8700::ErrorCode AccelFXOS8700::read8(uint8_t reg, uint8_t* value)
{
	*value = 0;

	Wire.beginTransmission(m_address);
  	Wire.write(reg);

	Wire.endTransmission(0);
 	int n = Wire.requestFrom(int(m_address), 1, 0);
 	if (n != 1) return ERROR_CONNECTION;

    *value = Wire.read();

    return SUCCESS;
}


AccelFXOS8700::ErrorCode AccelFXOS8700::write8(uint8_t reg, uint8_t value)
{
	Wire.beginTransmission(m_address);
	int written = 0;
    written += Wire.write(reg);
    written += Wire.write(value);
	Wire.endTransmission(m_address);

    return (written == 2) ? SUCCESS : ERROR_CONNECTION;
}


AccelFXOS8700::ErrorCode AccelFXOS8700::read(float* v, float* gravitySquared, float* gravity)
{
	Wire.beginTransmission(m_address);
    Wire.write(FXOS8700_REGISTER_STATUS | 0x80);
  	Wire.endTransmission();

  	static const int N = 7;
  	int n = Wire.requestFrom(int(m_address), N);
  	if (n != N) {
		#ifdef SERIAL_DEBUG
			Serial.println("request data failed. line: ");
		#endif
  		return ERROR_CONNECTION;
  	}

    Wire.read();	// status
    uint8_t axhi = Wire.read();
    uint8_t axlo = Wire.read();
    uint8_t ayhi = Wire.read();
    uint8_t aylo = Wire.read();
    uint8_t azhi = Wire.read();
    uint8_t azlo = Wire.read();

    // The stupid z-axis sticks.
    // https://community.nxp.com/thread/315538
    // I've seen the z axis sticking (stiction) in at least 2 PJRC prop shields.
    // Raw values I've seen off the accelerometer:
	// 		az hi=127 low=252
	// WHAT TO DO ABOUT THIS??
	// 		Filter out: freefall on z axis.
	// 		Use another channel? 
	//		Set to one?
	// Also, it's possible to knock it free again. I just smacked the circuit
	// on the desk and now it's working fine. Scary.
	// "if the Z-axis output is reporting exactly 0x7FFC or 0x8000 (+/-full scale)..."
	// Not sure x/y get stuck, but check just in case.
	if (    (axhi == 0x7f && axlo == 0xfc)
	     || (axhi == 0x80 && axlo == 0x00)) 
	{
		axhi = axlo = 0;
	}
	if (    (ayhi == 0x7f && aylo == 0xfc)
	     || (ayhi == 0x80 && aylo == 0x00)) 
	{
		ayhi = aylo = 0;
	}
	if (    (azhi == 0x7f && azlo == 0xfc)
	     || (azhi == 0x80 && azlo == 0x00)) 
	{
		azhi = azlo = 0;
	}

/*
    Serial.print(axhi); Serial.print(" ");
    Serial.print(axlo); Serial.print(" ");
    Serial.print(ayhi); Serial.print(" ");
    Serial.print(aylo); Serial.print(" ");
    Serial.print(azhi); Serial.print(" ");
    Serial.print(azlo); Serial.println(" ");
*/

//    for(int i=0; i<6; ++i) {
//    	Wire.read();
//    }
//    uint8_t mxhi = Wire.read();
//    uint8_t mxlo = Wire.read();
//    uint8_t myhi = Wire.read();
//    uint8_t mylo = Wire.read();
//    uint8_t mzhi = Wire.read();
//    uint8_t mzlo = Wire.read();
 
  	int16_t accelX = (int16_t)((axhi << 8) | axlo) >> 2;
  	int16_t accelY = (int16_t)((ayhi << 8) | aylo) >> 2;
  	int16_t accelZ = (int16_t)((azhi << 8) | azlo) >> 2;
 
	static const float convert = 1.0f / 2048.f;
	v[0] = float(accelX) * convert;
	v[1] = float(accelY) * convert;
	v[2] = float(accelZ) * convert;

	float g2 = 1;

	if (gravitySquared || gravity) {
		g2 = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	}
	if (gravitySquared) 
		*gravitySquared = g2;
	if (gravity) 
		*gravity = sqrt(g2);

	return SUCCESS;
}
#endif // SABER_ACCELEROMETER_NXP
