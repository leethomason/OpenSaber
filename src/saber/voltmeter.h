#ifndef SABER_VOLTMETER_INCLUDED
#define SABER_VOLTMETER_INCLUDED

#include <stdint.h>

/**
 * Power changes over time, and higher
 * draw changes the power. A small class
 * to average out power changes.
 */
class AveragePower
{
public:
    AveragePower();
    void push(uint32_t milliVolts);     // circular buffer of samples; seeded with nominal

    uint32_t power() const;
    enum { NUM_SAMPLES = 100 };
    enum { SAMPLE_INTERVAL = 12 };

    static int vbatToPowerLevel(int32_t vbat, int nLevels);

private:
    mutable uint32_t m_power = 0;
    int m_pos = 0;
    uint32_t m_sample[NUM_SAMPLES];
};


class Voltmeter
{
public:
    Voltmeter() { _instance = this; }
    static Voltmeter* instance() { return _instance; }

    void begin();

    /// Instantaneous power. (Noisy).
    uint32_t readVBat();
    /// Average power.
    uint32_t averagePower() { return m_averagePower.power(); }

    /// Add a sample to the average power.
    uint32_t takeSample();

private:
    static Voltmeter* _instance;

    AveragePower m_averagePower;
};

#endif // SABER_VOLTMETER_INCLUDED
