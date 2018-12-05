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
    void push(uint32_t milliVolts);
    uint32_t power() {
        return m_power;
    }
    enum { NUM_SAMPLES = 8};

    static int vbatToPowerLevel(int32_t vbat, int nLevels);

private:
    uint32_t m_power;
    uint32_t m_sample[NUM_SAMPLES];
};


class Voltmeter
{
public:
    Voltmeter() {}

    void begin();

    /// Instantaneous power. (Noisy).
    uint32_t readVBat();
    /// Average power.
    uint32_t averagePower() { return m_averagePower.power(); }

    /// Add a sample to the average power.
    void takeSample();

private:
    AveragePower m_averagePower;
};

#endif // SABER_VOLTMETER_INCLUDED
