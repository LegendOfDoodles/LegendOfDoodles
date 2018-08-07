#include "stdafx.h"
#include "SoundManager.h"

//CSoundManager::CSoundManager()
//{
//}
//
//CSoundManager::~CSoundManager()
//{
//}
//
//void CSoundManager::init()
//{
//	m_nCh = CHANNEL_MAX;
//	m_ppCh = new Channel*[m_nCh];
//
//	m_ppMusic = new Sound*[SOUND_MAX_COUNT];
//
//	m_Result = System_Create(&m_pFmod);
//	ErrorCheck(m_Result);
//
//	m_Result = m_pFmod->init(CHANNEL_MAX, FMOD_INIT_NORMAL, NULL);
//	ErrorCheck(m_Result);
//}
//
//void CSoundManager::loading()
//{
//	m_Result = m_pFmod->createSound("Sound/d.ogg", FMOD_LOOP_NORMAL, NULL, &m_ppMusic[SOUND::SOUND_0]);
//	ErrorCheck(m_Result);
//}
//
//void CSoundManager::play(int opt)
//{
//	m_pFmod->update();
//	m_Result = m_pFmod->playSound(m_ppMusic[opt], NULL, false, &m_ppCh[opt]);
//	m_ppCh[opt]->setVolume(0.5f);
//	ErrorCheck(m_Result);
//}
//
//void CSoundManager::stop(int chNum)
//{
//	m_ppCh[chNum]->stop();
//}
//
//void CSoundManager::ErrorCheck(FMOD_RESULT result)
//{
//	if (result != FMOD_OK)
//	{
//		printf("ErrorCheck");
//	}
//}


