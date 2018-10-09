#ifndef OPEN_SABER_AUDIO_INCLUDED
#define OPEN_SABER_AUDIO_INCLUDED

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

    void setVolume(float v);
    float volume() const { return m_volume; }

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

#endif // OPEN_SABER_AUDIO_INCLUDED
