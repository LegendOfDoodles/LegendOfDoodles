#include "stdafx.h"
#include "SoundManager.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"

CSoundManager::CSoundManager()
{
}

CSoundManager::CSoundManager(CCamera* pCamera)
{
	m_Result = System_Create(&m_pFmod);
	ErrorCheck(m_Result);

	m_Result = m_pFmod->getVersion(&m_version);
	ErrorCheck(m_Result);

	m_Result = m_pFmod->init(CHANNEL_MAX, FMOD_INIT_NORMAL, NULL);
	ErrorCheck(m_Result);

	m_Result = m_pFmod->set3DSettings(1.0, DISTANCEFACTOR, 1.0f);
	ErrorCheck(m_Result);

	m_pCamera = pCamera;

	// Camera 넘겨받자 (리스너 포지션 조절하기)
	m_Listener.currpos.x = m_pCamera->GetPosition().x;
	m_Listener.currpos.y = m_pCamera->GetPosition().y;
	m_Listener.currpos.z = m_pCamera->GetPosition().z;

	m_Listener.up.x = m_pCamera->GetUpVector().x;
	m_Listener.up.y = m_pCamera->GetUpVector().y;
	m_Listener.up.z = m_pCamera->GetUpVector().z;

	m_Listener.forward.x = m_pCamera->GetLookVector().x;
	m_Listener.forward.y = m_pCamera->GetLookVector().y;
	m_Listener.forward.z = m_pCamera->GetLookVector().z;
}

CSoundManager::~CSoundManager()
{
	m_pFmod->release();
	m_pFmod->close();
}

void CSoundManager::loading()
{
	// 3D Sounds
	// Sword Q
	m_Result = m_pFmod->createSound("./Resource/Sound/Player/Sword/Sword_Q.mp3", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::Player_Sword_Q_Sound]);
	m_Result = m_Sound[SOUND::Player_Sword_Q_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::Player_Sword_Q_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);

	// Sword W
	m_Result = m_pFmod->createSound("./Resource/Sound/Player/Sword/Sword_W.mp3", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::Player_Sword_W_Sound]);
	m_Result = m_Sound[SOUND::Player_Sword_W_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::Player_Sword_W_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);

	// Sword E
	m_Result = m_pFmod->createSound("./Resource/Sound/Player/Sword/Sword_E.mp3", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::Player_Sword_E_Sound]);
	m_Result = m_Sound[SOUND::Player_Sword_E_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::Player_Sword_E_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);
	// Sword R
	m_Result = m_pFmod->createSound("./Resource/Sound/Player/Sword/Sword_R.mp3", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::Player_Sword_R_Sound]);
	m_Result = m_Sound[SOUND::Player_Sword_R_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::Player_Sword_R_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);

	// Staff Q
	m_Result = m_pFmod->createSound("./Resource/Sound/Player/Staff/Staff_Q.mp3", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::Player_Staff_Q_Sound]);
	m_Result = m_Sound[SOUND::Player_Staff_Q_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::Player_Staff_Q_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);

	// Staff W
	m_Result = m_pFmod->createSound("./Resource/Sound/Player/Staff/Staff_W.mp3", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::Player_Staff_W_Sound]);
	m_Result = m_Sound[SOUND::Player_Staff_W_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::Player_Staff_W_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);

	// Staff E
	m_Result = m_pFmod->createSound("./Resource/Sound/Player/Staff/Staff_E.mp3", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::Player_Staff_E_Sound]);
	m_Result = m_Sound[SOUND::Player_Staff_E_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::Player_Staff_E_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);

	// Staff R
	m_Result = m_pFmod->createSound("./Resource/Sound/Player/Staff/Staff_R.wav", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::Player_Staff_R_Sound]);
	m_Result = m_Sound[SOUND::Player_Staff_R_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::Player_Staff_R_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);

	// Bow Q
	m_Result = m_pFmod->createSound("./Resource/Sound/Player/Bow/Bow_Q.mp3", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::Player_Arrow_Q_Sound]);
	m_Result = m_Sound[SOUND::Player_Arrow_Q_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::Player_Arrow_Q_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);

	// Bow W
	m_Result = m_pFmod->createSound("./Resource/Sound/Player/Bow/Bow_W.mp3", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::Player_Arrow_W_Sound]);
	m_Result = m_Sound[SOUND::Player_Arrow_W_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::Player_Arrow_W_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);

	// Bow E
	m_Result = m_pFmod->createSound("./Resource/Sound/Player/Bow/Bow_E.mp3", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::Player_Arrow_E_Sound]);
	m_Result = m_Sound[SOUND::Player_Arrow_E_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::Player_Arrow_E_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);

	// Bow R
	m_Result = m_pFmod->createSound("./Resource/Sound/Player/Bow/Bow_R.mp3", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::Player_Arrow_R_Sound]);
	m_Result = m_Sound[SOUND::Player_Arrow_R_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::Player_Arrow_R_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);

	// TowerAttack_Explosion_Sound
	m_Result = m_pFmod->createSound("./Resource/Sound/Tower/Explosion.mp3", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::TowerAttack_Explosion_Sound]);
	m_Result = m_Sound[SOUND::TowerAttack_Explosion_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::TowerAttack_Explosion_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);

	// Golem_StandardAttack_Sound
	m_Result = m_pFmod->createSound("./Resource/Sound/Golem/Golem_StandardAttack.mp3", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::Golem_StandardAttack_Sound]);
	m_Result = m_Sound[SOUND::Golem_StandardAttack_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::Golem_StandardAttack_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);

	// Golem_FootrollAttack_Sound
	m_Result = m_pFmod->createSound("./Resource/Sound/Golem/Golem_FootroolAttack.mp3", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::Golem_FootrollAttack_Sound]);
	m_Result = m_Sound[SOUND::Golem_FootrollAttack_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::Golem_FootrollAttack_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);

	// Golem_SpecialAttack_Sound
	m_Result = m_pFmod->createSound("./Resource/Sound/Golem/Golem_SpecialAttack.mp3", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::Golem_SpecialAttack_Sound]);
	m_Result = m_Sound[SOUND::Golem_SpecialAttack_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::Golem_SpecialAttack_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);

	// Flying_PlayerArrow_Sound
	m_Result = m_pFmod->createSound("./Resource/Sound/FlyingObject/Flying_Arrow.mp3", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::Flying_PlayerArrow_Sound]);
	m_Result = m_Sound[SOUND::Flying_PlayerArrow_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::Flying_PlayerArrow_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);

	// Flying_PlayerRArrow_Sound
	m_Result = m_pFmod->createSound("./Resource/Sound/FlyingObject/Bow_R_Flying_Arrow.mp3", FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_Sound[SOUND::Flying_PlayerRArrow_Sound]);
	m_Result = m_Sound[SOUND::Flying_PlayerRArrow_Sound]->set3DMinMaxDistance(SOUND_MIN_DISTANCE, SOUND_MAX_DISTANCE);
	m_Result = m_Sound[SOUND::Flying_PlayerRArrow_Sound]->setMode(FMOD_DEFAULT);
	ErrorCheck(m_Result);

	// 2D Sounds
	// BackGround Music Copyright by Heroes of the Storm
	m_Result = m_pFmod->createSound("./Resource/Sound/BackGround/BackGround.mp3", FMOD_LOOP_NORMAL, NULL, &m_Sound[SOUND::Back_Ground]);
	ErrorCheck(m_Result);
}

void CSoundManager::Update(float fTimeElapsed)
{
	m_Listener.currpos.x = m_pCamera->GetPosition().x;
	m_Listener.currpos.y = m_pCamera->GetPosition().y;
	m_Listener.currpos.z = m_pCamera->GetPosition().z;

	m_Listener.vel.x = (m_Listener.currpos.x - m_Listener.lastpos.x) * fTimeElapsed;
	m_Listener.vel.y = (m_Listener.currpos.y - m_Listener.lastpos.y) * fTimeElapsed;
	m_Listener.vel.z = (m_Listener.currpos.z - m_Listener.lastpos.z) * fTimeElapsed;

	m_Listener.lastpos = m_Listener.currpos;

	m_pFmod->set3DListenerAttributes(0, &m_Listener.currpos, &m_Listener.vel, &m_Listener.forward, &m_Listener.up);

	m_pFmod->update();
}

// 매니저가 불려서 사운드가 재생될 위치를 넘겨줘야함 다렉에선 XMFLOAT3 넘겨줄것.
void CSoundManager::play(SOUND opt, XMFLOAT3 pos)
{
	static FMOD_VECTOR vel{ 0.0f, 0.0f, 0.0f };
	static FMOD_MODE mode;

	m_Sound[opt]->getMode(&mode);

	if (mode & FMOD_3D) {
		FMOD_VECTOR sound_pos{ pos.x * DISTANCEFACTOR, pos.y * DISTANCEFACTOR, pos.z *DISTANCEFACTOR };
		m_Result = m_pFmod->playSound(m_Sound[opt], NULL, false, &m_ppCh[NotUsingChNum]);
		m_ppCh[NotUsingChNum]->set3DAttributes(&sound_pos, &vel);
		ErrorCheck(m_Result);
		if (++NotUsingChNum >= CHANNEL_MAX) NotUsingChNum = 1;
	}
	else {
		if (opt == SOUND::Back_Ground) {
			m_Result = m_pFmod->playSound(m_Sound[opt], NULL, false, &m_ppCh[(int)opt]);
			m_ppCh[(int)opt]->setVolume(0.3f);
		}
		ErrorCheck(m_Result);
	}
}

void CSoundManager::stop(int chNum)
{
	m_ppCh[chNum]->stop();
}

void CSoundManager::ErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		printf("ErrorCheck");
	}
}


