#ifndef OPEN_SABER_AUDIO_INCLUDED
#define OPEN_SABER_AUDIO_INCLUDED

#include <stdint.h>


/*
   Lower level audio classe.
   Called from SFX, not directly.
*/
class AudioPlayer
{
public:
    AudioPlayer();

    void init();

    void play(const char* filename);
    void stop();
    bool isPlaying() const;
    uint32_t lengthMillis() const;  // only valid if header read; else 0.

    void setVolume(float v);
    float volume() const { return m_volume; }

    void process();

private:
    void setShutdown();

    bool     m_enabled;
    uint32_t m_startPlayingTime;
    float    m_volume;
};

#endif // OPEN_SABER_AUDIO_INCLUDED
