#include "sfxcalc.h"
#include "./src/util/fixed.h"
#include "./src/util/Grinliz_Util.h"

bool SFXCalc::Test()
{
    SFXCalc calc;
    TrackVolume tv;

    const float RAD_PER_SEC = 3.1f;

    // -- Turned off, still. -- 
    // Warm up.
    tv = calc.sm_processTrackVolume(100, 0, 0);
    TEST(tv.stillReset == false);

    tv = calc.sm_processTrackVolume(100, 0, 0);
    TEST(tv.still == true);
    TEST(tv.stillReset == true);
    TEST(tv.hum == 0);
    TEST(tv.swing0 == 0);
    TEST(tv.swing1 == 0);

    // Ignite, still
    calc.ignite(1000);
    int prevHum = 0;
    for (int i = 0; i < 10; ++i) {
        tv = calc.sm_processTrackVolume(100, 0, 0);
        TEST(tv.still == true);
        TEST(tv.stillReset == false);
        TEST(tv.hum > prevHum);
        prevHum = tv.hum;
        TEST(tv.swing0 == 0);
        TEST(tv.swing1 == 0);
    }
    // Should be still and fully ignited.
    TEST(tv.still == true);
    TEST(tv.hum == 256);

    // Spin.
    for (int i = 0; i < 10; ++i) {
        tv = calc.sm_processTrackVolume(100, RAD_PER_SEC, i * 10);
        TEST(tv.still == false);
        TEST(tv.stillReset == false);
        TEST(tv.hum > 0);
        TEST(tv.swing0 + tv.swing1 > 0);
    }

    // Still again
    tv = calc.sm_processTrackVolume(100, 0, 100);
    TEST(tv.still == true);
    TEST(tv.stillReset == false);
    TEST(tv.hum == 256);
    TEST(tv.swing0 == 0);
    TEST(tv.swing1 == 0);

    tv = calc.sm_processTrackVolume(100, 0, 100);
    TEST(tv.stillReset == true);
    tv = calc.sm_processTrackVolume(100, 0, 100);
    TEST(tv.stillReset == false);

    // Spin and retract.
    calc.retract(1000);
    for (int i = 0; i < 10; ++i) {
        tv = calc.sm_processTrackVolume(100, RAD_PER_SEC, i * 10);
        TEST(tv.still == false);
        TEST(tv.stillReset == false);
    }
    TEST(tv.still == false);
    TEST(tv.stillReset == false);
    TEST(tv.hum == 0);
    TEST(tv.swing0 == 0);
    TEST(tv.swing1 == 0);

    return true;
}


void SFXCalc::ignite(uint32_t time)
{
    m_volumeEnvelope.start(time, 0, 256);
}


void SFXCalc::retract(uint32_t time)
{
    m_volumeEnvelope.start(time, 256, 0);
}


SFXCalc::TrackVolume SFXCalc::sm_processTrackVolume(uint32_t delta, float radPerSec, int blend256)
{
    TrackVolume tv;

    // Animates from 0->256 on ignite, and back to 0 on retract.
    // 256 or 0 at steady state.
    m_volumeEnvelope.tick(delta);

    int swing = sm_swingToVolume(radPerSec);
    tv.hum = 256 - swing * 192 / 256;

    if (swing == 0) {
        uint32_t old = m_timeStill;
        m_timeStill += delta;

        tv.stillReset = (m_timeStill >= STILL_TIME && old < STILL_TIME);
        tv.still = true;
    }
    else {
        m_timeStill = 0;
        tv.still = false;
        tv.stillReset = false;
    }

    FixedNorm blend = divToFixed<FixedNorm>(blend256, 256);
    tv.swing0 = scale(cosLerp(blend), swing);
    tv.swing1 = scale(sinLerp(blend), swing);

    tv.hum = tv.hum * m_volumeEnvelope.value() / 256;
    tv.swing0 = tv.swing0 * m_volumeEnvelope.value() / 256;
    tv.swing1 = tv.swing1 * m_volumeEnvelope.value() / 256;

    return tv;
}


int SFXCalc::sm_swingToVolume(float radPerSec)
{
    static constexpr int SWING_MAX = 14;

    // 10 figure 8s in 13s is med-fast
    // 6.1 * 2 * 10 rad / 13 s = 9.6 rad / sec
    // Decent movement is 6.1 rad / sec

    // Keep experimenting with this. *sigh.*
    static const int16_t VOLUME[SWING_MAX+1] = {
        0, 0, 0, 0, 8, 16, 32, 48, 64, 96, 128, 160, 192, 256,
        256
    };

    // Less than "three zeros" makes the magnetometer unstable, I think?
    // So hard to test.
    //static const int16_t VOLUME[SWING_MAX+1] = {
    //    0, 0, 0, 8, 16, 32, 48, 64, 96, 128, 160, 192, 256, 256,
    //    256
    //};
    if (radPerSec >= float(SWING_MAX)) 
        return 256;

    int low = (int)radPerSec;
    float fraction = radPerSec - low;

    return (int) lerp((float)VOLUME[low], (float)VOLUME[low+1], fraction);
}
