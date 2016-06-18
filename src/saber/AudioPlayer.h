#ifndef OPEN_SABER_AUDIO_INCLUDED
#define OPEN_SABER_AUDIO_INCLUDED

#include <stdint.h>

class AudioPlayer
{
public:
    AudioPlayer();

    void init();

    void play(const char* filename);
    void stop();
    bool isPlaying() const;
    uint32_t lengthMillis() const;  // only valid if header read; else 0.

    void mute(bool m);
    bool isMuted() const {
        return m_muted;
    }

    void setVolume(float v);
    float volume() const {
        return m_volume;
    }

private:
    void setShutdown();

    bool     m_muted;
    bool     m_shutdown;
    uint32_t m_startPlayingTime;
    float    m_volume;
};

#endif // OPEN_SABER_AUDIO_INCLUDED
