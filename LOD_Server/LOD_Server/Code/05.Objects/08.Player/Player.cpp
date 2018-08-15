#include "stdafx.h"
#include "Player.h"

/// <summary>
/// 목적: 플레이어 관리 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-08-01
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CPlayer::CPlayer() : CAnimatedObject()
{
	m_detectRange = CONVERT_PaperUnit_to_InG(80.0f);
	m_sightRange = CONVERT_PaperUnit_to_InG(80.0f);
	SetSpeed(CONVERT_cm_to_InG(3.285f * 10));
}


CPlayer::~CPlayer()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CPlayer::Animate(float timeElapsed)
{
	m_hpSyncCoolTime += timeElapsed;

	switch (m_curState) {
	case States::Idle:
		if (m_nCurrAnimation != Animations::Idle) m_nCurrAnimation = Animations::Idle;
		if (m_mainPath) SetState(States::Walk);
		break;
	case States::Attack:
		if (m_fFrameTime >= m_nAniLength[m_nAniIndex] - 1)
		{
			SetState(States::Idle);
		}
		else if (GetType() == ObjectType::StickPlayer)
		{
			if (m_nCurrAnimation == Animations::Attack1 &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pColManager->RequestCollide(CollisionType::SPHERE, this, CONVERT_PaperUnit_to_InG(5), CONVERT_PaperUnit_to_InG(20), 100);
			}
		}
		else if (GetType() == ObjectType::SwordPlayer)
		{
			if (m_nCurrAnimation == Animations::Attack1 &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pColManager->RequestCollide(CollisionType::SPHERE, this, CONVERT_PaperUnit_to_InG(5), CONVERT_PaperUnit_to_InG(20), 100);
			}
			else if (m_nCurrAnimation == Animations::SkillQ &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				// CoolTime 초기화
				m_StatusInfo.QSkillCoolTime = 0.0f;

				m_pColManager->RequestCollide(CollisionType::SPHERE, this, CONVERT_PaperUnit_to_InG(8), CONVERT_PaperUnit_to_InG(8), 100);
			}
			else if (m_nCurrAnimation == Animations::SkillW &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_StatusInfo.WSkillCoolTime = 0.0f;

				m_pColManager->RequestCollide(CollisionType::SECTERFORM, this, CONVERT_PaperUnit_to_InG(24), 180, 500);
			}
			else if (m_nCurrAnimation == Animations::SkillE &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_StatusInfo.ESkillCoolTime = 0.0f;

				m_pColManager->RequestCollide(CollisionType::SECTERFORM, this, CONVERT_PaperUnit_to_InG(16), 150, 100);
			}
			else if (m_nCurrAnimation == Animations::SkillR &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.666f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.666f)
			{
				m_StatusInfo.RSkillCoolTime = 0.0f;

				m_pColManager->RequestCollide(CollisionType::SPHERE, this, 0, CONVERT_PaperUnit_to_InG(12), 500);
			}
		}
		else if (GetType() == ObjectType::StaffPlayer)
		{
			if (m_nCurrAnimation == Animations::Attack1 &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_Magic, 100);
				SendMissilePacket(FlyingObjectType::Player_Magic);
				//// EffectMgr
				//m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_SwordSkill_Q_Effect);
				//m_pSoundMgr->play(SOUND::Player_Sword_Q_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillQ &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				// CoolTime 초기화
				m_StatusInfo.QSkillCoolTime = 0.0f;

				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_MagicSkill_Q, 100);
				SendMissilePacket(FlyingObjectType::Player_MagicSkill_Q);
			}
			else if (m_nCurrAnimation == Animations::SkillW &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_StatusInfo.WSkillCoolTime = 0.0f;
			}
			else if (m_nCurrAnimation == Animations::SkillE &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_StatusInfo.ESkillCoolTime = 0.0f;
			}
			else if (m_nCurrAnimation == Animations::SkillR &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.666f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.666f)
			{
				m_StatusInfo.RSkillCoolTime = 0.0f;

				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_MagicSkill_R, 100);
				SendMissilePacket(FlyingObjectType::Player_MagicSkill_R);
			}
		}
		else if (GetType() == ObjectType::BowPlayer)
		{
			if (m_nCurrAnimation == Animations::Attack1 &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_Arrow, 100);
				SendMissilePacket(FlyingObjectType::Player_Arrow);
				//// EffectMgr
				//m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_SwordSkill_Q_Effect);
				//m_pSoundMgr->play(SOUND::Player_Sword_Q_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillQ &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				// CoolTime 초기화
				m_StatusInfo.QSkillCoolTime = 0.0f;

				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_ArrowSkill_Q, 100);
				SendMissilePacket(FlyingObjectType::Player_ArrowSkill_Q);
			}
			else if (m_nCurrAnimation == Animations::SkillW &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_StatusInfo.WSkillCoolTime = 0.0f;

				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_ArrowSkill_W, 100);
				SendMissilePacket(FlyingObjectType::Player_ArrowSkill_W);
			}
			else if (m_nCurrAnimation == Animations::SkillE &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_StatusInfo.ESkillCoolTime = 0.0f;

				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_ArrowSkill_E, 100);
				SendMissilePacket(FlyingObjectType::Player_ArrowSkill_E);
			}
			else if (m_nCurrAnimation == Animations::SkillR &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.666f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.666f)
			{
				m_StatusInfo.RSkillCoolTime = 0.0f;

				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_ArrowSkill_R, 100);
				SendMissilePacket(FlyingObjectType::Player_ArrowSkill_R);
			}
		}
		break;

	case States::Walk:
		if (m_nCurrAnimation != Animations::StartWalk&&
			m_nCurrAnimation != Animations::Walking)
			m_nCurrAnimation = Animations::StartWalk;

		if (m_nCurrAnimation == Animations::StartWalk) {
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] - 1)
			{
				m_nCurrAnimation = Animations::Walking;
				m_fFrameTime = 0;
			}
		}
		break;
	case States::Die:
		if (GetAnimTimeRemainRatio() < 0.05)
		{
			SetState(States::Remove);
		}
		break;
	case States::Win:
		if (m_nCurrAnimation != Animations::Win) m_nCurrAnimation = Animations::Win;
		break;
	case States::Defeat:
		if (m_nCurrAnimation != Animations::Defeat&&
			m_nCurrAnimation != Animations::Defeat2)
			m_nCurrAnimation = Animations::Defeat;

		if (m_nCurrAnimation == Animations::Defeat) {
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] - 1)
			{
				m_nCurrAnimation = Animations::Defeat2;
				m_fFrameTime = 0;
			}
		}
		break;
	case States::Remove:
		m_spawnCoolTime -= timeElapsed;
		if (m_spawnCoolTime < 0.0f)
		{
			Respawn();
		}
		break;
	default:
		break;
	}

	if (m_curState != States::Remove)
	{
		m_fPreFrameTime = m_fFrameTime;
		m_fFrameTime += ANIMATION_SPEED * timeElapsed;
	}

	m_StatusInfo.QSkillCoolTime = min(m_StatusInfo.QSkillCoolTime += timeElapsed * 0.1f, 1.f);
	m_StatusInfo.WSkillCoolTime = min(m_StatusInfo.WSkillCoolTime += timeElapsed * 0.1f, 1.f);
	m_StatusInfo.ESkillCoolTime = min(m_StatusInfo.ESkillCoolTime += timeElapsed * 0.1f, 1.f);
	m_StatusInfo.RSkillCoolTime = min(m_StatusInfo.RSkillCoolTime += timeElapsed * 0.1f, 1.f);


	AdjustAnimationIndex();

	if (m_fFrameTime > m_nAniLength[m_nAniIndex]) {
		while (m_fFrameTime > m_nAniLength[m_nAniIndex])
			m_fFrameTime -= m_nAniLength[m_nAniIndex];
	}

	if (MoveToDestination(timeElapsed) == States::Done) SetState(States::Idle);

	CAnimatedObject::Animate(timeElapsed);
}


void CPlayer::LookAt(XMFLOAT3 objPosition)
{
	if (m_curState == States::Attack) return;
	CAnimatedObject::LookAt(objPosition);
}

void CPlayer::LookAt(XMFLOAT2 objPosition)
{
	if (m_curState == States::Attack) return;
	CAnimatedObject::LookAt(objPosition);
}

void CPlayer::ActiveSkill(AnimationsType act)
{
	if (m_curState != States::Attack) {
		m_curState = States::Attack;
		m_nCurrAnimation = act;
		m_fFrameTime = 0;
	}
}

void CPlayer::SetState(StatesType newState)
{
	m_curState = newState;

	switch (newState)
	{
	case States::Idle:
		m_nCurrAnimation = Animations::Idle;
		break;
	case States::Walk:
		RegenerateLookAt();
		if (m_nCurrAnimation != Animations::StartWalk &&
			m_nCurrAnimation != Animations::Walking)
			m_nCurrAnimation = Animations::StartWalk;
		break;
	case States::Chase:
		m_nCurrAnimation = Animations::StartWalk;
		break;
	case States::Attack:
		m_fFrameTime = 0;
		break;
	case States::Die:
		m_nCurrAnimation = Animations::Die;
		m_fFrameTime = 0;
		m_spawnCoolTime = COOLTIME_SPAWN_PLAYER + static_cast<float>(m_StatusInfo.Level);
		SetPathToGo(NULL);
		break;
	case States::Remove:
		break;
	case States::Win:
		m_nCurrAnimation = Animations::Win;
		m_fFrameTime = 0;
		SetPathToGo(NULL);
		break;
	case States::Defeat:
		m_nCurrAnimation = Animations::Defeat;
		m_fFrameTime = 0;
		SetPathToGo(NULL);
		break;
	default:
		assert(!"Error:: There is No State");
	}
}

void CPlayer::ChangeSkillSet(CSkeleton ** ppskill)
{
	for (int j = 0; j < 8; j++)
	{
		m_nAniLength[j + 4] = ppskill[j]->GetAnimationLength();
		m_pSkeleton[j + 4] = *ppskill[j];
	}
}

void CPlayer::SaveCurrentState()
{
	m_xmf4x4SpawnWorld = m_xmf4x4World;
	m_spawnLocation = GetPosition();
}

void CPlayer::Respawn()
{
	SetState(StatesType::Idle);

	m_StatusInfo.HP = m_StatusInfo.maxHP;

	m_pColManager->AddCollider(this);

	m_xmf4x4World = m_xmf4x4SpawnWorld;

	SC_Msg_Player_Respawn p;
	p.size = sizeof(p);
	p.type = SC_PLAYER_RESPAWN;
	p.Target_Tag = (short)m_tag;
	for (int j = 0; j < MAX_USER; ++j) {
		if (g_clients[j].m_isconnected == true) {
			SendPacket(j, &p);
		}
	}
}

void CPlayer::ReceiveDamage(float damage)
{
	// 이미 사망한 상태인 경우 대미지 처리를 하지 않는다.
	if (m_curState == States::Die || m_curState == States::Remove) { return; }
	m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def);

	if (m_StatusInfo.HP <= 0) 
	{
		SetState(States::Die);
	}

	if (m_hpSyncCoolTime > COOLTIME_HP_SYNC)
	{
		SC_Msg_Hp_Sync p;
		p.curhp = m_StatusInfo.HP;
		p.maxhp = m_StatusInfo.maxHP;
		p.size = sizeof(p);
		p.type = SC_HP_SYNC;
		p.Target_Tag = (short)m_tag;
		p.updatetime = g_GameTime;
		for (int j = 0; j < MAX_USER; ++j) {
			if (g_clients[j].m_isconnected == true) {
				SendPacket(j, &p);
			}
		}
		m_hpSyncCoolTime = 0.0f;
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CPlayer::AdjustAnimationIndex()
{
	switch (m_nCurrAnimation)
	{
	case Animations::Win:
		m_nAniIndex = 0;
		break;
	case Animations::Defeat:
		m_nAniIndex = 1;
		break;
	case Animations::Defeat2:
		m_nAniIndex = 2;
		break;
	case Animations::Die:
		m_nAniIndex = 3;
		break;
	case Animations::Idle:
		m_nAniIndex = 4;
		break;
	case Animations::StartWalk:
		m_nAniIndex = 5;
		break;
	case Animations::Walking:
		m_nAniIndex = 6;
		break;
	case Animations::SkillQ:
		m_nAniIndex = 7;
		break;
	case Animations::SkillW:
		m_nAniIndex = 8;
		break;
	case Animations::SkillE:
		m_nAniIndex = 9;
		break;
	case Animations::SkillR:
		m_nAniIndex = 10;
		break;
	case Animations::Attack1:
		m_nAniIndex = 11;
		break;
	}
}

void CPlayer::SendMissilePacket(FlyingObjectType type)
{
	SC_Msg_Player_Missile p;
	p.size = sizeof(p);
	p.type = SC_PLAYER_MISSILE;
	p.Target_Tag = (short)m_tag;
	p.Missile_Type = (short)type;
	for (int j = 0; j < MAX_USER; ++j) {
		if (g_clients[j].m_isconnected == true) {
			SendPacket(j, &p);
		}
	}
}
