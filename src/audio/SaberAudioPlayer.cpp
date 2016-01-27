#include "SaberAudioPlayer.h"

SaberAudioPlayer::SaberAudioPlayer(AudioPlayer* audio) {
	m_idle = -1;
	m_current = -1;
	m_audioPlayer = audio;
}


void SaberAudioPlayer::scan() {
	
}
