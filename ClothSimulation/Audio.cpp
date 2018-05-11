#include "Audio.h"

Audio::Audio()
{
	InitFmod();
	LoadAudio();
}


Audio::~Audio()
{
}

bool Audio::InitFmod() {
	FMOD_RESULT result;
	result = FMOD::System_Create(&m_audioMgr);
	if (result != FMOD_OK) return false;
	result = m_audioMgr->init(50, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK) return false;
	return true;
}

const bool Audio::LoadAudio() {
	FMOD_RESULT result;
	result = m_audioMgr->createSound("Assets/Audio/enemy_shoot.wav", FMOD_DEFAULT, 0, &m_enemyShoot);
	result = m_audioMgr->createSound("Assets/Audio/score_pickup.wav", FMOD_DEFAULT, 0, &m_scorePickup);
	result = m_audioMgr->createSound("Assets/Audio/player_descending.wav", FMOD_DEFAULT, 0, &m_playerDescending);
	result = m_audioMgr->createSound("Assets/Audio/player_spawning.wav", FMOD_DEFAULT, 0, &m_playerSpawning);
	result = m_audioMgr->createSound("Assets/Audio/player_shoot.wav", FMOD_DEFAULT, 0, &m_playerShoot);
	result = m_audioMgr->createSound("Assets/Audio/player_dead.wav", FMOD_DEFAULT, 0, &m_playerDead);
	result = m_audioMgr->createSound("Assets/Audio/next_level.wav", FMOD_DEFAULT, 0, &m_nextLevel);
	result = m_audioMgr->createSound("Assets/Audio/enemy_dead.wav", FMOD_DEFAULT, 0, &m_enemyDead);
	result = m_audioMgr->createSound("Assets/Audio/enemy_hit.wav", FMOD_DEFAULT, 0, &m_enemyHit);
	result = m_audioMgr->createSound("Assets/Audio/button_click.wav", FMOD_DEFAULT, 0, &m_buttonClick);
	result = m_audioMgr->createSound("Assets/Audio/background_music.mp3", FMOD_DEFAULT, 0, &m_bgMusic);
	m_bgMusic->setMode(FMOD_LOOP_NORMAL);
	m_buttonClick->setMode(FMOD_LOOP_OFF);
	return true;
}

void  Audio::playBgMusic()
{
	// Play the background music
	m_audioMgr->playSound(m_bgMusic, 0, false, &m_bgMusicChannel);
}

void Audio::playButtonClick()
{
	// Play the click music
	m_audioMgr->playSound(m_buttonClick, 0, false, &m_sfxChannel);
}

void Audio::playSFX(Sound sound)
{
	if(sound == ENEMY_DEAD)
		m_audioMgr->playSound(m_enemyDead, 0, false, &m_sfxChannel);
	else if (sound == ENEMY_HIT)
		m_audioMgr->playSound(m_enemyHit, 0, false, &m_sfxChannel);
	else if (sound == ENEMY_SHOOT)
		m_audioMgr->playSound(m_enemyShoot, 0, false, &m_enemyChannel);
	else if (sound == SCORE_PICKUP)
		m_audioMgr->playSound(m_scorePickup, 0, false, &m_sfxChannel);
	else if (sound == NEXT_LEVEL)
		m_audioMgr->playSound(m_nextLevel, 0, false, &m_sfxChannel);
	else if (sound == PLAYER_DEAD)
		m_audioMgr->playSound(m_playerDead, 0, false, &m_sfxChannel);
	else if (sound == PLAYER_SHOOT)
		m_audioMgr->playSound(m_playerShoot, 0, false, &m_bulletChannel);
	else if (sound == PLAYER_SPAWNING)
		m_audioMgr->playSound(m_playerSpawning, 0, false, &m_sfxChannel);
	else if (sound == PLAYER_DESCENDING)
		m_audioMgr->playSound(m_playerDescending, 0, false, &m_sfxChannel);
}

void Audio::setNetworkAudioServer(NetworkServerSystem* audioServer)
{
	m_networkAudioServer = audioServer;
}

