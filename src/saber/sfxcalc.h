#pragma once

#include <stdint.h>
#include "./src/util/Grinliz_Util.h"

// WARNING: this is intended to be used from Win32 for testing.
// No arduino specific code.

class SFXCalc 
{
public:
    struct TrackVolume {
        bool still = false;
        bool stillReset = false;
        int hum = 0;    // 0-256
        int swing0 = 0; // 0-256
        int swing1 = 0; // 0-256
    };
    // Called on a regular interval, as time is important to volumeEnvelope and stillness.
    TrackVolume sm_processTrackVolume(uint32_t delta, float radPerSec, int blend256);
    int SFXCalc::sm_swingToVolume(float radPerSec);

	AnimateProp volumeEnvelope;

private:
    bool m_still = false;
};