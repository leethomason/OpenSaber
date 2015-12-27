#include "sfx.h"
#include "pins.h"

//#define DEBUG_DEEP

#if SERIAL_DEBUG == 1
# define ASSERT(x) if (!(x)) { Serial.print("SFX-ASSERT:"); Serial.print(#x); Serial.print(" "); Serial.print(__LINE__); }
#else
# define ASSERT(x)
#endif

SFX* SFX::instance = 0;

SFX::SFX(Stream* _streamToAudio)
  : adaAudio(_streamToAudio, 0, PIN_SFX_RST)
{
  instance = this;
  currentSound = 255;
  soundState = SOUND_NOT_PLAYING;
  bladeOn = false;
  soundOn = true;
  memset(sfxLocation, 255, sizeof(sfxLocation[0])*NUM_SFX_TYPES);
  igniteTime = 0;
  retractTime = 0;
}

bool SFX::init() 
{
  #if SERIAL_DEBUG == 1
  Serial.println(F("SFX::init()"));
  #endif
  adaAudio.reset();
  volume = adaAudio.volDown();
  pinMode(PIN_SFX_ACTIVITY, INPUT);

  if (nSoundFiles == 0) {
    nSoundFiles = adaAudio.listFiles(0, 0, listFiles);

    #if SERIAL_DEBUG == 1
    Serial.print(F("SFX::init vol:")); Serial.print(volume); Serial.print(F(" nFiles:")); Serial.println(nSoundFiles);
    dumpLocations();
    #endif
  }
  return true;
}

/*static*/
void SFX::listFiles(uint8_t id, const FileInfo& fileInfo) {
  #if SERIAL_DEBUG == 1
  Serial.print(id); Serial.print(" "); Serial.print(fileInfo.filename), Serial.print(" "); Serial.println(fileInfo.size);
  #endif

  // This relies on getting files in 1) alphabetical order and 2) all caps from the AudioFX!
  const char* name = fileInfo.filename;
  int slot = -1;

  if      (strstr(name, "BLDON") || strstr(name, "POWERON"))      slot = SFX_POWER_ON;
  else if (strstr(name, "BLDOFF") || strstr(name, "POWEROFF"))    slot = SFX_POWER_OFF;
  else if (strstr(name, "IDLE") || strstr(name, "HUM"))           slot = SFX_IDLE;
  else if (strstr(name, "IMPACT") || strstr(name, "CLASH"))       slot = SFX_IMPACT;
  else if (strstr(name, "MOTION") || strstr(name, "SWING"))       slot = SFX_MOTION;
  else if (strstr(name, "USRHOLD") || strstr(name, "LOCKUP"))     slot = SFX_USER_HOLD;
  else if (strstr(name, "USRTAP") || strstr(name, "BLASTER"))     slot = SFX_USER_TAP;

  if (slot >= 0) {
    if (instance->sfxLocation[slot].start == 255) {
      instance->sfxLocation[slot].start = id;
      instance->sfxLocation[slot].count = 1;

      if (slot == SFX_POWER_ON && instance->igniteTime == 0) {
        instance->igniteTime = fileInfo.size / int32_t(44);  // 2 channels * 22k FIXME only works for 22k sounds!!
        instance->igniteTime = instance->igniteTime * 8 / 10;
        //Serial.print("ignite "); Serial.println(instance->igniteTime);
      }
      if (slot == SFX_POWER_OFF && instance->retractTime == 0) {
        instance->retractTime = fileInfo.size / int32_t(44);
        //Serial.print("retract "); Serial.println(instance->retractTime);
      }
    }
    else {
      instance->sfxLocation[slot].count++;
    }
  }
}

void SFX::dumpLocations() 
{
  #if SERIAL_DEBUG == 1
  for (int i = 0; i < NUM_SFX_TYPES; ++i) {
    Serial.print(i); Serial.print(" "); Serial.print(sfxLocation[i].start); Serial.print(" "); Serial.println(sfxLocation[i].count);
  }
  #endif
}

bool SFX::activity() const {
  if (digitalRead(PIN_SFX_ACTIVITY) == LOW)
    return true;
  return false;
}

bool SFX::playSound(int sound, int mode) {
  if (!soundOn) return false;

  ASSERT(sound >= 0);
  ASSERT(sound < NUM_SFX_TYPES);
  #if SERIAL_DEBUG == 1
  #ifdef DEBUG_DEEP
  Serial.print(F("SFX playSound() sound:")); Serial.println(sound);
  Serial.print(F("SFX bladeOn:")); Serial.println(bladeOn);
  Serial.print(F("SFX currentSound:")); Serial.println(currentSound);
  #endif
  #endif

  if (!bladeOn && (sound != SFX_POWER_ON)) {
    return false ; // don't play sound with blade off
  }

  if (sound == SFX_POWER_ON) {
    if (bladeOn) return false;  // defensive error check.
    bladeOn = true;
  }
  else if (sound == SFX_POWER_OFF) {
    if (!bladeOn) return false;  // defensive error check.
    bladeOn = false;
  }

  if (   currentSound == 255
         || (mode == SFX_OVERRIDE)
         || (mode == SFX_GREATER && sound > currentSound)
         || (mode == SFX_GREATER_OR_EQUAL && sound >= currentSound))
  {
    ASSERT(sfxLocation[sound].start != 255);
    if (sfxLocation[sound].start != 255) {
      ASSERT(sfxLocation[sound].count < 255);
      ASSERT(sfxLocation[sound].count > 0);
      if (activity() || soundState == SOUND_QUEUED) {
        bool stoppedOkay = adaAudio.stop();
        ASSERT(stoppedOkay);
        (void)stoppedOkay;
      }
      int track = sfxLocation[sound].start + random(sfxLocation[sound].count);
      ASSERT(track >= 0);
      ASSERT(track < nSoundFiles);

      #if SERIAL_DEBUG == 1
      Serial.print(F("SFX play track ")); Serial.print(track);
      Serial.print( " "); Serial.print(currentSound); Serial.print(F("->")); Serial.println(sound);
      #endif
      bool playOkay = adaAudio.playTrack(track);
      ASSERT(playOkay);
      (void)playOkay;
      soundState = SOUND_QUEUED;
      currentSound = sound;
      return true;
    }
  }
  return false;
}

void SFX::process() {
  // Play the idle sound if the blade is on.
  if (activity()) {
    if (soundState == SOUND_QUEUED) {
      soundState = SOUND_PLAYING;
    }
  }
  else {
    if (soundState == SOUND_QUEUED) {
      // Do nothing. Sound on it's way. Activity not set yet.
    }
    else {
      currentSound = 255;
      if (bladeOn) {
        playSound(SFX_IDLE, SFX_OVERRIDE);
      }
      else {
        soundState = SOUND_NOT_PLAYING;
      }
    }
  }
}


uint8_t SFX::setVol(int vol) {
  // Only works if not playing!!!
  if (!activity()) {
    #if SERIAL_DEBUG == 1
    Serial.print("SFX setVol "); Serial.println(vol);
    #endif
    volume = adaAudio.setVol(vol);
  }
  return volume;
}

void SFX::setSoundOn(bool on) {
  if (!activity()) {
    soundOn = on;
  }
}

