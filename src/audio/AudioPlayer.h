#ifndef OPEN_SABER_AUDIO_INCLUDED
#define OPEN_SABER_AUDIO_INCLUDED

#include <stdint.h>

// - Shutdown pin when not playing
//

class AudioPlayer
{
public:
  AudioPlayer();

  void init();

  void play(const char* filename);
  void stop();
  bool isPlaying() const;
  void mute(bool m);
  bool isMuted() const;

  void doLoop();

private:
  void setShutdown();

  uint32_t m_startPlayingTime;
  uint32_t m_stopPlayingTime;
  bool m_muted;
  bool m_shutdown;
};

#endif // OPEN_SABER_AUDIO_INCLUDED
