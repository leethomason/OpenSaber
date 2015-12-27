#ifndef SFX_HEADER
#define SFX_HEADER

#include <Arduino.h>
#include <Adafruit_Soundboard.h>

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

class SFX
{
public:
  SFX(Stream* streamToAudio);

  bool init();
  bool playSound(int sfx, int mode);
  void process();

  const uint32_t getIgniteTime() const { return igniteTime ? igniteTime : 1000; }
  const uint32_t getRetractTime() const { return retractTime ? retractTime : 1000; }

  // Only works if no sound playing
  void setSoundOn(bool on);
  bool isSoundOn() const { return soundOn; }
  
  // Only works if no sound playing. And slow.
  uint8_t setVol(int vol);
  
private:
  static void listFiles(uint8_t id, const FileInfo& fileInfo);
  void dumpLocations();
  bool activity() const;

  // note: initialize to 255
  struct SFXLocation {
    uint8_t start;
    uint8_t count;
  
    const bool InUse() const { return start < 255 && count < 255; }
  };

  enum {
    SOUND_NOT_PLAYING,
    SOUND_QUEUED,
    SOUND_PLAYING
  };

  uint8_t currentSound;
  uint8_t nSoundFiles;
  uint8_t soundState;
  uint8_t volume;
  bool    bladeOn;
  bool    soundOn;
  uint32_t  igniteTime;
  uint32_t  retractTime;

  Adafruit_Soundboard adaAudio;
  SFXLocation sfxLocation[NUM_SFX_TYPES];

  static SFX* instance;
};

#endif // SFX_HEADER
