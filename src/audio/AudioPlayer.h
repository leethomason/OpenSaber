#ifndef OPEN_SABER_AUDIO_INCLUDED
#define OPEN_SABER_AUDIO_INCLUDED

class AudioPlayer
{
public:
	AudioPlayer();

	void init();

  void play(const char* filename);
  void stop();

};

#endif // OPEN_SABER_AUDIO_INCLUDED
