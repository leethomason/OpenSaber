#ifndef OPEN_SABER_AUDIO_INCLUDED
#define OPEN_SABER_AUDIO_INCLUDED

class AudioPlayer
{
public:
	AudioPlayer();

	void init();

  void play(const char* filename);
  void stop();
  bool isPlaying() const;

private:
  uint32_t startPlayingTime;
};

#endif // OPEN_SABER_AUDIO_INCLUDED
