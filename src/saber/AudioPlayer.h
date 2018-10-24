#ifndef OPEN_SABER_AUDIO_INCLUDED
#define OPEN_SABER_AUDIO_INCLUDED

#ifdef CORE_TEENSY

#include <stdint.h>
#include "iaudio.h"


/*
   Lower level audio classe.
   Called from SFX, not directly.
*/
class AudioPlayer : public IAudio
{
public:
    AudioPlayer();

    void init();

    bool play(const char* filename);
    void stop();
    bool isPlaying() const;

    void setVolume(int v);
    int volume() const { return int(m_volume * 256.0 + 0.5); }

    void process();

    // for testing
    uint16_t nEnabled() const { return m_nEnabled; }
    uint16_t nDisabled() const { return m_nDisabled; }

private:
    void setShutdown();

    bool     m_enabled;
    uint16_t m_nEnabled;
    uint16_t m_nDisabled;
    uint32_t m_startPlayingTime;    
    float    m_volume;
};

#endif // CORE_TEENSY
#endif // OPEN_SABER_AUDIO_INCLUDED
