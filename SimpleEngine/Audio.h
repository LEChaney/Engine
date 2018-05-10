//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : Controls the audio in the game
//				  
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#pragma once

#include <fmod.hpp>

class NetworkServerSystem;

enum Sound
{
	ENEMY_DEAD,
	ENEMY_HIT,
	ENEMY_SHOOT,
	SCORE_PICKUP,
	NEXT_LEVEL,
	PLAYER_DEAD,
	PLAYER_SHOOT,
	PLAYER_SPAWNING,
	PLAYER_DESCENDING,
};

class Audio
{
public:
	Audio();
	~Audio();

	void playBgMusic();
	void playButtonClick();

	// Play a game play Sound effect
	void playSFX(Sound sound);

	void setNetworkAudioServer(NetworkServerSystem* audioServer);

private:
	bool InitFmod();
	const bool LoadAudio();

	FMOD::System* m_audioMgr;
	FMOD::Channel* m_bulletChannel;
	FMOD::Channel* m_enemyChannel;
	FMOD::Channel* m_bgMusicChannel;
	FMOD::Channel* m_sfxChannel;

	FMOD::Sound* m_bgMusic;
	FMOD::Sound* m_buttonClick;

	FMOD::Sound* m_enemyDead;
	FMOD::Sound* m_enemyHit;
	FMOD::Sound* m_nextLevel;
	FMOD::Sound* m_playerDead;
	FMOD::Sound* m_playerShoot;
	FMOD::Sound* m_playerSpawning;
	FMOD::Sound* m_playerDescending;
	FMOD::Sound* m_enemyShoot;
	FMOD::Sound* m_scorePickup;

	NetworkServerSystem* m_networkAudioServer;
};

