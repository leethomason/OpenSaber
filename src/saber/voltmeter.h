#ifndef SABER_VOLTMETER_INCLUDED
#define SABER_VOLTMETER_INCLUDED

#include "Grinliz_Util.h"
#include <stdint.h>

class Voltmeter
{
public:
    enum {SAMPLE_INTERVAL = 12};

    Voltmeter() : m_averagePower(NOMINAL_VOLTAGE) { 
        ASSERT(_instance == 0);
        _instance = this; 
    }
    static Voltmeter* instance() { return _instance; }

    void begin();

    /// Instantaneous power. (Noisy).
    uint32_t readVBat();
    /// Average power.
    uint32_t averagePower() const { return m_averagePower.average(); }

    /// Add a sample to the average power.
    uint32_t takeSample();

    static int vbatToPowerLevel(int32_t vbat, int nLevels);

private:
    static Voltmeter* _instance;
    AverageSample<uint16_t, uint32_t, 64> m_averagePower;
};

#endif // SABER_VOLTMETER_INCLUDED
