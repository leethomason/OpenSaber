#pragma once

#include <stdint.h>
#include "./src/util/Grinliz_Util.h"

// WARNING: this is intended to be used from Win32 for testing.
// No arduino specific code.

class SFXCalc 
{
public:
    static constexpr uint32_t STILL_TIME = 200; // ms until still is fired

    struct TrackVolume {
        bool still = false;
        bool stillReset = false;
        int hum = 0;    // 0-256
        int swing0 = 0; // 0-256
        int swing1 = 0; // 0-256
    };
    // Called on a regular interval, as time is important to volumeEnvelope and stillness.
    TrackVolume sm_processTrackVolume(uint32_t delta, float radPerSec, int blend256);

    void ignite(uint32_t time);
    void retract(uint32_t time);

    static bool Test();

private:
    int sm_swingToVolume(float radPerSec);

    AnimateProp m_volumeEnvelope;
    uint32_t m_timeStill = 0;
};