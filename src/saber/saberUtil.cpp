//#include <Adafruit_LIS3DH.h>

#include "pins.h"
#include "saberUtil.h"
#include "Grinliz_Arduino_Util.h"
#include "blade.h"
#include "saberDB.h"
#include "sfx.h"
#include "accelFXOS8700.h"

//#define USE_PS_NXP

void BladeState::change(uint8_t state)
{
    ASSERT(state >= BLADE_OFF && state <= BLADE_RETRACT);
    m_currentState   = state;
    m_startTime = millis();

    switch(m_currentState) {
    case BLADE_OFF:
        Log.event("[BLADE_OFF]");
        break;
    case BLADE_IGNITE:
        Log.event("[BLADE_IGNITE]");
        break;
    case BLADE_ON:
        Log.event("[BLADE_ON]");
        break;
    case BLADE_FLASH:
        Log.event("[BLADE_FLASH]");
        break;
    case BLADE_RETRACT:
        Log.event("[BLADE_RETRACT]");
        break;
    default:
        ASSERT(false);
        break;
    }
}

bool BladeState::bladeOn() const
{
    ASSERT(m_currentState >= BLADE_OFF && m_currentState <= BLADE_RETRACT);
    return m_currentState >= BLADE_ON && m_currentState < BLADE_RETRACT;
}

void BladeState::process(Blade* blade, const SaberDB& saberDB, uint32_t time)
{
    static const uint32_t FLASH_TIME = 120;
    SFX* sfx = SFX::instance();

    switch (state()) {
    case BLADE_OFF:
        break;

    case BLADE_ON:
        blade->setRGB(saberDB.bladeColor());
        break;

    case BLADE_IGNITE:
    {
        uint32_t igniteTime = 1000;
        #ifdef SABER_SOUND_ON
        igniteTime = sfx->getIgniteTime();
        #endif

        bool done = blade->setInterp(millis() - startTime(), igniteTime, RGB(RGB::BLACK), saberDB.bladeColor());
        if (done) {
            change(BLADE_ON);
        }
    }
    break;

    case BLADE_RETRACT:
    {
        uint32_t retractTime = 1000;
        #ifdef SABER_SOUND_ON
            retractTime = sfx->getRetractTime();
        #endif
        bool done = blade->setInterp(millis() - startTime(), retractTime, saberDB.bladeColor(), RGB(RGB::BLACK));
        if (done) {
            change(BLADE_OFF);
        }
    }
    break;

    case BLADE_FLASH:
        // interpolate?
    {
        uint32_t delta = time - startTime();
        if (delta > FLASH_TIME) {
            blade->setRGB(saberDB.bladeColor());
            change(BLADE_ON);
        }
        else {
            blade->setRGB(saberDB.impactColor());
        }
    }
    break;

    default:
        ASSERT(false);
        break;
    }
}


void UIModeUtil::nextMode()
{
    switch(m_mode) {
        case UIMode::NORMAL:        m_mode = UIMode::PALETTE;       Log.p("mode: palette").eol();    break;
        case UIMode::PALETTE:       m_mode = UIMode::VOLUME;        Log.p("mode: volume").eol();     break;

#if (MEDITATION_MODE)
        case UIMode::VOLUME:        m_mode = UIMode::MEDITATION;    Log.p("mode: meditation").eol(); break;
        case UIMode::MEDITATION:    m_mode = UIMode::NORMAL;        Log.p("mode: normal").eol();     break;
#else
        case UIMode::VOLUME:        m_mode = UIMode::NORMAL;        Log.p("mode: normal").eol();     break;
#endif
        default:
            ASSERT(false);
            m_mode = UIMode::NORMAL;
            break;
    }
}

AveragePower::AveragePower()
{
    for(int i=0; i<NUM_SAMPLES; ++i) {
        m_sample[i] = NOMINAL_VOLTAGE;
    }
    m_power = NOMINAL_VOLTAGE;
}

void AveragePower::push(uint32_t milliVolts)
{
    uint32_t total = milliVolts;
    for(int i=0; i<NUM_SAMPLES-1; ++i) {
        m_sample[i] = m_sample[i+1];
        total += m_sample[i];
    }
    m_sample[NUM_SAMPLES-1] = milliVolts;
    m_power = total / NUM_SAMPLES;
}

#if SABER_ACCELEROMETER == SABER_ACCELEROMETER_LIS3DH
Adafruit_LIS3DH localAccel;
#elif SABER_ACCELEROMETER == SABER_ACCELEROMETER_NXP
AccelFXOS8700 localAccel;
#endif

Accelerometer* Accelerometer::_instance = 0;

Accelerometer::Accelerometer()
{
    _instance = this;
}

void Accelerometer::begin()
{
    #if SABER_ACCELEROMETER == SABER_ACCELEROMETER_LIS3DH
        Log.p("LIS3DH Accelerometer starting.").eol();
        if (!localAccel.begin()) {
            Log.p("Accelerometer ERROR.").eol();
        }
        else {
            Log.p("Accelerometer open.").eol();
            localAccel.setRange(LIS3DH_RANGE_4_G);
            localAccel.setDataRate(LIS3DH_DATARATE_100_HZ);
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

        // Y is blade direction!
        *ax = localAccel.x_g;
        *ay = localAccel.y_g;
        *az = localAccel.z_g;
        *g2Normal = (*ax)*(*ax) + (*az)*(*az);
        *g2 = *g2Normal + (*ay)*(*ay);

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

void Voltmeter::begin()
{
    #ifdef SABER_VOLTMETER
    analogReference(INTERNAL);  // 1.1 volts
    analogRead(PIN_VMETER);     // warm up the ADC to avoid spurious initial value.
    Log.p("Voltmeter open.").eol();
    delay(50);
    #endif

    for(int i=0; i<AveragePower::NUM_SAMPLES; ++i) {
        delay(10);
        m_averagePower.push(readVBat());
    }
    Log.p("Vbat: ").p(readVBat()).p(" Ave:").p(averagePower()).eol();
    #ifdef SABER_VOLTMETER
    Log.p("Voltmeter initialized.").eol();
    #endif
}


uint32_t Voltmeter::readVBat() 
{
    #ifdef SABER_VOLTMETER
        uint32_t analog = analogRead(PIN_VMETER);
        uint32_t mV = analog * UVOLT_MULT / uint32_t(1000);
        return mV;
    #else
        return NOMINAL_VOLTAGE;
    #endif
}


void Voltmeter::takeSample()
{
    m_averagePower.push(readVBat());
}

