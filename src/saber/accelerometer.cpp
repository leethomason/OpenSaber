#include <Arduino.h>
#include "pins.h"

#if SABER_ACCELEROMETER == SABER_ACCELEROMETER_LIS3DH || SABER_ACCELEROMETER == SABER_ACCELEROMETER_LIS3DH_SPI
#   include <Adafruit_LIS3DH.h>
#elif SABER_ACCELEROMETER == SABER_ACCELEROMETER_NXP
#   include "accelFXOS8700.h"
#endif

#include "accelerometer.h"
#include "Grinliz_Arduino_Util.h"

#if SABER_ACCELEROMETER == SABER_ACCELEROMETER_LIS3DH
Adafruit_LIS3DH localAccel;
#elif SABER_ACCELEROMETER == SABER_ACCELEROMETER_LIS3DH_SPI
Adafruit_LIS3DH localAccel(PIN_ACCEL_EN);
#elif SABER_ACCELEROMETER == SABER_ACCELEROMETER_NXP
AccelFXOS8700 localAccel;
#else
    #error Accelerometer not defined.
#endif

Accelerometer* Accelerometer::_instance = 0;

Accelerometer::Accelerometer()
{
    _instance = this;
}

void Accelerometer::begin()
{
    m_range = 4.0;

    #if SABER_ACCELEROMETER == SABER_ACCELEROMETER_LIS3DH
        Log.p("LIS3DH Accelerometer starting: I2C mode.").eol();
        if (!localAccel.begin(0x19)) {
            Log.p("Accelerometer ERROR.").eol();
        }
        else {
            delay(20);
            localAccel.setRange(LIS3DH_RANGE_4_G);
            localAccel.setDataRate(LIS3DH_DATARATE_100_HZ);
            Log.p("Accelerometer open.").eol();
        }
    #elif SABER_ACCELEROMETER == SABER_ACCELEROMETER_LIS3DH_SPI
        Log.p("LIS3DH Accelerometer starting: SPI mode").eol();
        bool success = false;
        for(int i=0; i<5; ++i) {
            // Buggy buggy buggy SPI code.
            delay(20);
            if (localAccel.begin(0)) {
                Log.p("Accelerometer open on attempt: ").p(i).eol();
                success = true;
                break;
            }   
        }

        if (!success) {
            Log.p("Accelerometer ERROR.").eol();
        }
        else {
            bool success = false;
            int n=0;
            for(; n<4; n++) {
                localAccel.setRange(LIS3DH_RANGE_4_G);
                localAccel.setDataRate(LIS3DH_DATARATE_100_HZ);
                delay(20);
                lis3dh_range_t range = localAccel.getRange();
                lis3dh_dataRate_t rate = localAccel.getDataRate();

                if (range == LIS3DH_RANGE_4_G && rate == LIS3DH_DATARATE_100_HZ) {
                    success = true;
                    break;
                }
                delay(100);
            }
            Log.p("Accelerometer open. Configuration success=").p(success ? "true" : "false").p(" on attempt=").p(n).eol();
        }
    #elif SABER_ACCELEROMETER == SABER_ACCELEROMETER_NXP
        Log.p("Accelerometer starting.").eol();
        AccelFXOS8700::ErrorCode rc = localAccel.begin(AccelFXOS8700::ADDRESS_PROP_SHIELD);
        if (rc != AccelFXOS8700::SUCCESS) {
            Log.p("Accelerometer ERROR. rc=").p(int(rc)).eol();
        }
        else {
            Log.p("Accelerometer open.").eol();            
        }
    #endif
}


void Accelerometer::read(float* ax, float* ay, float* az, float* g2, float* g2Normal)
{
    #if SABER_ACCELEROMETER == SABER_ACCELEROMETER_LIS3DH 
        localAccel.read();

        // Y is blade direction
        *ax = localAccel.x_g;
        *ay = localAccel.y_g;
        *az = localAccel.z_g;
        *g2Normal = (*ax)*(*ax) + (*az)*(*az);
        *g2 = *g2Normal + (*ay)*(*ay);

    #elif SABER_ACCELEROMETER == SABER_ACCELEROMETER_LIS3DH_SPI
        /*  This code path correctly uses the ACCEL_BLADE_DIRECTION
            which is set by the circuit design. (Not saber model.)
            Should be ported to the other code paths if they are
            brought back in use.
        */
        localAccel.read();

        *ax = localAccel.x_g;
        *ay = localAccel.y_g;
        *az = localAccel.z_g;

        *g2 = (*ax)*(*ax) + (*ay)*(*ay) + (*az)*(*az);

        *g2Normal = 0;
        #ifndef ACCEL_BLADE_DIRECTION
            #error Need ACCEL_BLADE_DIRECTION to be defined.
        #endif
        #if ACCEL_BLADE_DIRECTION != 0
            *g2Normal += (*ax)*(*ax);
        #endif
        #if ACCEL_BLADE_DIRECTION != 1
            *g2Normal += (*ay)*(*ay);
        #endif
        #if ACCEL_BLADE_DIRECTION != 2
            *g2Normal += (*az)*(*az);
        #endif

    #elif SABER_ACCELEROMETER == SABER_ACCELEROMETER_NXP
        // Using the prop shield: x is in the blade direction,
        // y & z are normal.

        float v[3] = {0};
        localAccel.read(v, 0, 0);
        *ax = v[0];
        *ay = v[1];
        *az = v[2];

        *g2Normal = (*ay) * (*ay) + (*az) * (*az);
        *g2 = *g2Normal + (*ax) * (*ax);
    #else
        *ax = 0;
        *ay = 0;
        *az = 1;
        *g2 = 1;
        *g2Normal = 0;
    #endif
}

