#include "sfxcalc.h"
#include "./src/util/fixed.h"

SFXCalc::TrackVolume SFXCalc::sm_processTrackVolume(uint32_t delta, float radPerSec, int blend256)
{
    TrackVolume tv;

    // Animates from 0->256 on ignite, and back to 0 on retract.
    // 256 or 0 at steady state.
    volumeEnvelope.tick(delta);

    int swing = sm_swingToVolume(radPerSec);
    tv.hum = 256 - swing * 192 / 256;

    if (swing == 0) {
        tv.stillReset = !m_still;
        m_still = true;
        tv.still = true;
    }
    else {
        m_still = false;
        tv.still = false;
        tv.stillReset = false;
    }

    FixedNorm blend = divToFixed<FixedNorm>(blend256, 256);
    tv.swing0 = scale(cosLerp(blend), swing);
    tv.swing1 = scale(sinLerp(blend), swing);
    return tv;
}


int SFXCalc::sm_swingToVolume(float radPerSec)
{
    static constexpr int SWING_MAX = 14;

    // 10 figure 8s in 13s is med-fast
    // 6.1 * 2 * 10 rad / 13 s = 9.6 rad / sec
    // Decent movement is 6.1 rad / sec

    // Keep experimenting with this. *sigh.*
    //static const int16_t VOLUME[SWING_MAX+1] = {
    //    0, 0, 0, 0, 8, 16, 32, 48, 64, 96, 128, 160, 192, 256,
    //    256
    //};
    static const int16_t VOLUME[SWING_MAX+1] = {
        0, 0, 8, 16, 32, 48, 64, 96, 128, 160, 192, 256, 256, 256,
        256
    };
    if (radPerSec >= float(SWING_MAX)) 
        return 256;

    int low = (int)radPerSec;
    float fraction = radPerSec - low;

    return (int) lerp((float)VOLUME[low], (float)VOLUME[low+1], fraction);
}
