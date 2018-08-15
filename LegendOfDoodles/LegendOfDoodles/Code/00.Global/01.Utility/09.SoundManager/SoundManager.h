#pragma once
#include "../library/inc/fmod.hpp"
#include "../library/inc/fmod_errors.h"

using namespace FMOD;

#define CHANNEL_MAX 10

// TEST ¿ëµµ
enum class SOUND {
	Back_Ground,

	Player_Sword_Attack_Sound,
	Player_Sword_Q_Sound,
	Player_Sword_W_Sound,
	Player_Sword_E_Sound,
	Player_Sword_R_Sound,

	Player_Staff_Attack_Sound,
	Player_Staff_Q_Sound,
	Player_Staff_W_Sound,
	Player_Staff_E_Sound,
	Player_Staff_R_Sound,

	Player_Arrow_Attack_Sound,
	Player_Arrow_Q_Sound,
	Player_Arrow_W_Sound,
	Player_Arrow_E_Sound,
	Player_Arrow_R_Sound,

	TowerAttack_Explosion_Sound,

	Golem_StandardAttack_Sound,
	Golem_FootrollAttack_Sound,
	Golem_SpecialAttack_Sound,

	Flying_PlayerArrow_Sound,
	Flying_PlayerRArrow_Sound
};

struct Listener {
	FMOD_VECTOR currpos = { 0.f, 0.f, -10.f };
	FMOD_VECTOR lastpos = { 0.f, 0.f, 0.f };
	FMOD_VECTOR forward = { 0.f, 0.f, 1.f };
	FMOD_VECTOR up = { 0.f, 1.f, 0.f };
	FMOD_VECTOR vel = { 0.f, 0.f, 0.f };
};

class CSoundManager {
public:
	CSoundManager();
	CSoundManager(CCamera* pCamera);
	~CSoundManager();

	void loading();

	void Update(float fTimeElapsed);

	void play(SOUND opt, XMFLOAT3 pos);
	void stop(int chNum);
	void ErrorCheck(FMOD_RESULT result);

public:
	static constexpr float DISTANCEFACTOR{ 1.f };
	static constexpr float SOUND_MIN_DISTANCE{ 400.f };
	static constexpr float SOUND_MAX_DISTANCE{ 1800.f };

private:
	System * m_pFmod;
	Channel*		m_ppCh[CHANNEL_MAX];

	FMOD_RESULT		m_Result;
	unsigned int    m_version;

	int NotUsingChNum{ 1 };

	Listener		m_Listener;

	CCamera *m_pCamera;

	std::unordered_map<SOUND, Sound*> m_Sound;
};