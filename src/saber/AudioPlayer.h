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

    void mute(bool m);
    bool isMuted() const { return m_muted; }

    //void doLoop();

    //int ID() const {
   //   return m_id;
    //}

  private:
    void setShutdown();

    bool     m_muted;
    bool     m_shutdown;
    //int      m_id;
    uint32_t m_startPlayingTime;
    //uint32_t m_stopPlayingTime;
};

#endif // OPEN_SABER_AUDIO_INCLUDED
