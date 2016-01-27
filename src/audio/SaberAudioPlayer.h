#ifndef OPEN_SABER_AUDIO_PLAYER_INCLUDED
#define OPEN_SABER_AUDIO_PLAYER_INCLUDED

class AudioPlayer;

// SFX in priority order!
enum {
  SFX_IDLE,       // 0
  SFX_MOTION,     // 1
  SFX_IMPACT,     // 2
  SFX_USER_TAP,   // 3
  SFX_USER_HOLD,  // 4
  SFX_POWER_ON,   // 5
  SFX_POWER_OFF,  // 6

  NUM_SFX_TYPES
};

enum {
  SFX_GREATER,
  SFX_GREATER_OR_EQUAL,
  SFX_OVERRIDE
};

class SaberAudioPlayer {
  public:
  	SaberAudioPlayer(AudioPlayer* audio);

  	void scan();

  	void mute(bool muted);
  	bool isMuted();

  	void setVolume204();
  	uint8_t volume204();

  	void playSound(int16_t id, int16_t logicOp);
  	void setIdle(int16_t id);

  private:
  	int16_t m_idle;
  	int16_t m_current;
  	AudioPlayer* m_audioPlayer;
};

#endif // OPEN_SABER_AUDIO_PLAYER_INCLUDED
