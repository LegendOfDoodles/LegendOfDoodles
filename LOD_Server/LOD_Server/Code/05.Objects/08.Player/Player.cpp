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

	m_skillCoolTimeQ += timeElapsed;
	m_skillCoolTimeW += timeElapsed;
	m_skillCoolTimeE += timeElapsed;
	m_skillCoolTimeR += timeElapsed;

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
				m_pColManager->RequestCollide(CollisionType::SPHERE, this, CONVERT_PaperUnit_to_InG(5), CONVERT_PaperUnit_to_InG(20), m_StatusInfo.Atk);
			}
		}
		else if (GetType() == ObjectType::SwordPlayer)
		{
			if (m_nCurrAnimation == Animations::Attack1 &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pColManager->RequestCollide(CollisionType::SPHERE, this, CONVERT_PaperUnit_to_InG(5), CONVERT_PaperUnit_to_InG(20), m_StatusInfo.Atk);
			}
			else if (m_nCurrAnimation == Animations::SkillQ &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pColManager->RequestCollide(CollisionType::SPHERE, this, CONVERT_PaperUnit_to_InG(8), CONVERT_PaperUnit_to_InG(8), m_StatusInfo.Atk * m_StatusInfo.QSkillPower);
			}
			else if (m_nCurrAnimation == Animations::SkillW &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pColManager->RequestCollide(CollisionType::SECTERFORM, this, CONVERT_PaperUnit_to_InG(24) * m_StatusInfo.WSkillRange, 180, m_StatusInfo.Atk * m_StatusInfo.WSkillPower);
			}
			else if (m_nCurrAnimation == Animations::SkillE &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pColManager->RequestCollide(CollisionType::SECTERFORM, this, CONVERT_PaperUnit_to_InG(16), 150, m_StatusInfo.Atk * m_StatusInfo.ESkillPower);
			}
			else if (m_nCurrAnimation == Animations::SkillR &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.666f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.666f)
			{
				m_pColManager->RequestCollide(CollisionType::SPHERE, this, 0, CONVERT_PaperUnit_to_InG(12), m_StatusInfo.Atk * m_StatusInfo.RSkillPower);
			}
		}
		else if (GetType() == ObjectType::StaffPlayer)
		{
			if (m_nCurrAnimation == Animations::Attack1 &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_Magic, m_StatusInfo.Atk);
				SendMissilePacket(FlyingObjectType::Player_Magic);
			}
			else if (m_nCurrAnimation == Animations::SkillQ &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_MagicSkill_Q, m_StatusInfo.Atk * m_StatusInfo.QSkillPower);
				SendMissilePacket(FlyingObjectType::Player_MagicSkill_Q);
			}
			else if (m_nCurrAnimation == Animations::SkillW &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pColManager->RequestCollide(CollisionType::SPHERE, this, CONVERT_PaperUnit_to_InG(8) * m_StatusInfo.WSkillRange, CONVERT_PaperUnit_to_InG(8), m_StatusInfo.Atk * m_StatusInfo.WSkillPower);
			}
			else if (m_nCurrAnimation == Animations::SkillE &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pColManager->RequestCollide(CollisionType::SECTERFORM, this, CONVERT_PaperUnit_to_InG(15) * m_StatusInfo.ESkillRange, 90, m_StatusInfo.Atk * m_StatusInfo.ESkillPower);
			}
			else if (m_nCurrAnimation == Animations::SkillR &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.666f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.666f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_MagicSkill_R, m_StatusInfo.Atk * m_StatusInfo.RSkillPower);
				SendMissilePacket(FlyingObjectType::Player_MagicSkill_R);
			}
		}
		else if (GetType() == ObjectType::BowPlayer)
		{
			if (m_nCurrAnimation == Animations::Attack1 &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_Arrow, m_StatusInfo.Atk);
				SendMissilePacket(FlyingObjectType::Player_Arrow);
			}
			else if (m_nCurrAnimation == Animations::SkillQ &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_ArrowSkill_Q, m_StatusInfo.Atk * m_StatusInfo.QSkillPower);
				SendMissilePacket(FlyingObjectType::Player_ArrowSkill_Q);
			}
			else if (m_nCurrAnimation == Animations::SkillW &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_ArrowSkill_W, m_StatusInfo.Atk * m_StatusInfo.WSkillPower);
				SendMissilePacket(FlyingObjectType::Player_ArrowSkill_W);
			}
			else if (m_nCurrAnimation == Animations::SkillE &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_ArrowSkill_E, m_StatusInfo.Atk * m_StatusInfo.ESkillPower);
				SendMissilePacket(FlyingObjectType::Player_ArrowSkill_E);
			}
			else if (m_nCurrAnimation == Animations::SkillR &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_ArrowSkill_R, m_StatusInfo.Atk * m_StatusInfo.RSkillPower);
				SendMissilePacket(FlyingObjectType::Player_ArrowSkill_R);
			}
		}
		m_fPreFrameTime = m_fFrameTime;
		m_fFrameTime += ANIMATION_SPEED * timeElapsed *m_StatusInfo.AtkSpeed;
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
		m_fPreFrameTime = m_fFrameTime;
		m_fFrameTime += ANIMATION_SPEED * timeElapsed * m_StatusInfo.WalkSpeed;
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

	if (m_curState != States::Remove &&
		m_curState != States::Walk &&
		m_curState != States::Attack)
	{
		m_fPreFrameTime = m_fFrameTime;
		m_fFrameTime += ANIMATION_SPEED * timeElapsed;
	}

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

		if (act == Animations::SkillQ &&
			m_skillCoolTimeQ >= m_StatusInfo.QSkillCoolTime)
		{
			m_skillCoolTimeQ = 0;
			m_nCurrAnimation = act;
			m_fFrameTime = 0;
		}
		else if (act == Animations::SkillW &&
			m_skillCoolTimeW >= m_StatusInfo.WSkillCoolTime)
		{
			m_skillCoolTimeW = 0;
			m_nCurrAnimation = act;
			m_fFrameTime = 0;
		}
		else if (act == Animations::SkillE &&
			m_skillCoolTimeE >= m_StatusInfo.ESkillCoolTime)
		{
			m_skillCoolTimeE = 0;
			m_nCurrAnimation = act;
			m_fFrameTime = 0;
		}
		else if (act == Animations::SkillR &&
			m_skillCoolTimeR >= m_StatusInfo.RSkillCoolTime)
		{
			m_skillCoolTimeR = 0;
			m_nCurrAnimation = act;
			m_fFrameTime = 0;
		}
		else
		{
			m_nCurrAnimation = act;
			m_fFrameTime = 0;
		}
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

void CPlayer::SetType(ObjectType newObjectType)
{
	m_ObjectType = newObjectType;

	if (m_ObjectType == ObjectType::SwordPlayer)
	{
		m_StatusInfo.QSkillPower = 2.5f;
		m_StatusInfo.WSkillPower = 3.5f;
		m_StatusInfo.ESkillPower = 5.0f;
		m_StatusInfo.RSkillPower = 7.6f;
	}
	else if (m_ObjectType == ObjectType::StaffPlayer)
	{
		m_StatusInfo.QSkillPower = 2.5f;
		m_StatusInfo.WSkillPower = 3.5f;
		m_StatusInfo.ESkillPower = 5.0f;
		m_StatusInfo.RSkillPower = 7.6f;
	}
	else if (m_ObjectType == ObjectType::BowPlayer)
	{
		m_StatusInfo.QSkillPower = 2.5f;
		m_StatusInfo.WSkillPower = 3.5f;
		m_StatusInfo.ESkillPower = 5.0f;
		m_StatusInfo.RSkillPower = 7.6f;
	}
}

void CPlayer::SendCoolTime(int id)
{
	SC_Msg_Cooltime_Percent p;
	p.Target_Tag = (short)m_tag;
	p.QPercentage = min(m_skillCoolTimeQ / m_StatusInfo.QSkillCoolTime, 1.f);
	p.WPercentage = min(m_skillCoolTimeW / m_StatusInfo.WSkillCoolTime, 1.f);
	p.EPercentage = min(m_skillCoolTimeE / m_StatusInfo.ESkillCoolTime, 1.f);
	p.RPercentage = min(m_skillCoolTimeR / m_StatusInfo.RSkillCoolTime, 1.f);
	p.size = sizeof(p);
	p.type = SC_COOLTIME;
	SendPacket(id, &p);
}

void CPlayer::ReceiveDamage(float damage, CCollisionObject * pCol)
{
	// 이미 사망한 상태인 경우 대미지 처리를 하지 않는다.
	if (m_curState == States::Die || m_curState == States::Remove) { return; }
	m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def);

	if (m_StatusInfo.HP <= 0)
	{
		SetState(States::Die);
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (g_clients[i].m_isconnected) {
				SC_Msg_Set_Player_State p;
				p.Player_State = (BYTE)StatesType::Die;
				p.Player_Tag = (short)m_tag;
				p.size = sizeof(p);
				p.type = SC_SET_PLAYER_STATE;
				SendPacket(i, &p);
			}
		}
	}


	if (m_hpSyncCoolTime > COOLTIME_HP_SYNC)
	{
		SC_Msg_Hp_Sync p;
		p.curhp = m_StatusInfo.HP;
		p.maxhp = m_StatusInfo.maxHP;
		p.size = sizeof(p);
		p.type = SC_HP_SYNC;
		p.Target_Tag = (short)m_tag;
		p.Object_Type = (short)pCol->GetType();
		p.Flying_Type = (short)pCol->GetFlyingObjectsType();
		p.updatetime = g_GameTime;
		for (int j = 0; j < MAX_USER; ++j) {
			if (g_clients[j].m_isconnected == true) {
				SendPacket(j, &p);
			}
		}
		m_hpSyncCoolTime = 0.0f;
	}
}

void CPlayer::ApplySpecialStat(SpecialType curSP)
{
	if (m_ObjectType == ObjectType::SwordPlayer)
	{
		ApplySwordSP(curSP);
	}
	else if (m_ObjectType == ObjectType::StaffPlayer)
	{
		ApplyStaffSP(curSP);
	}
	else if (m_ObjectType == ObjectType::BowPlayer)
	{
		ApplyBowSP(curSP);
	}
}

void CPlayer::ApplySwordSP(SpecialType curSP)
{
	static int index{ 0 };
	
	if (index == 0)
	{
		if (curSP == SpecialType::AttackSpecial)
		{
			m_StatusInfo.QSkillPower *= 1.3f;
		}
		else if (curSP == SpecialType::DefenceSpecial)
		{
			m_StatusInfo.Def *= 1.3f;
		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_detectRange *= 1.15f;
			m_sightRange *= 1.15f;
		}
	}
	else if (index == 1)
	{
		if (curSP == SpecialType::AttackSpecial)
		{
			m_StatusInfo.Atk *= 1.5f;
		}
		else if (curSP == SpecialType::DefenceSpecial)
		{
			m_StatusInfo.Def *= 1.25f;
		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_StatusInfo.WSkillRange *= 1.5f;
		}
	}
	else if (index == 2)
	{
		if (curSP == SpecialType::AttackSpecial)
		{
			m_StatusInfo.WSkillPower;
		}
		else if (curSP == SpecialType::DefenceSpecial)
		{
			m_StatusInfo.WalkSpeed *= 1.3f;
			SendSpeedPacket(SpeedType::WalkSpeed);
		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_StatusInfo.RSkillCoolTime *= 0.5f;
		}
	}
	else if (index == 3)
	{
		if (curSP == SpecialType::AttackSpecial)
		{
			m_StatusInfo.Atk *= 1.5f;
			m_StatusInfo.QSkillPower *= 1.5f;
			m_StatusInfo.WSkillPower *= 1.5f;
			m_StatusInfo.ESkillPower *= 1.5f;
			m_StatusInfo.RSkillPower *= 1.5f;
		}
		else if (curSP == SpecialType::DefenceSpecial)
		{
			float hpPercent{ m_StatusInfo.HP / m_StatusInfo.maxHP };
			m_StatusInfo.maxHP *= 2.0f;
			m_StatusInfo.HP = m_StatusInfo.maxHP * hpPercent;
		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_StatusInfo.WSkillCoolTime *= 0.1f;
			m_StatusInfo.WSkillPower *= 0.4f;
		}
	}

	index++;
}

void CPlayer::ApplyStaffSP(SpecialType curSP)
{
	static int index{ 0 };

	if (index == 0)
	{
		if (curSP == SpecialType::AttackSpecial)
		{
			m_StatusInfo.Atk *= 1.1f;
		}
		else if (curSP == SpecialType::DefenceSpecial)
		{
			m_StatusInfo.WalkSpeed *= 1.15f;
			SendSpeedPacket(SpeedType::WalkSpeed);

		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_detectRange *= 1.15f;
			m_sightRange *= 1.15f;
		}
	}
	else if (index == 1)
	{
		if (curSP == SpecialType::AttackSpecial)
		{
			m_StatusInfo.Atk *= 1.05f;
			m_StatusInfo.AtkSpeed *= 1.1f;
			SendSpeedPacket(SpeedType::AttackSpeed);
		}
		else if (curSP == SpecialType::DefenceSpecial)
		{
			m_StatusInfo.WalkSpeed *= 1.25f;
			SendSpeedPacket(SpeedType::WalkSpeed);
		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_StatusInfo.WSkillPower *= 1.3f;
		}
	}
	else if (index == 2)
	{
		if (curSP == SpecialType::AttackSpecial)
		{
			m_StatusInfo.AtkSpeed *= 1.13f;
			SendSpeedPacket(SpeedType::AttackSpeed);
		}
		else if (curSP == SpecialType::DefenceSpecial)
		{
			m_StatusInfo.WalkSpeed *= 1.25f;
			SendSpeedPacket(SpeedType::WalkSpeed);
		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_StatusInfo.RSkillCoolTime *= 0.5f;
		}
	}
	else if (index == 3)
	{
		if (curSP == SpecialType::AttackSpecial)
		{
			m_StatusInfo.AtkSpeed *= 1.1f;
			m_StatusInfo.Atk *= 1.1f;
			float hpPercent{ m_StatusInfo.HP / m_StatusInfo.maxHP };
			m_StatusInfo.maxHP *= 0.7f;
			m_StatusInfo.HP = m_StatusInfo.maxHP * hpPercent;
			SendSpeedPacket(SpeedType::AttackSpeed);
		}
		else if (curSP == SpecialType::DefenceSpecial)
		{
			m_StatusInfo.AtkSpeed *= 1.2f;
			float hpPercent{ m_StatusInfo.HP / m_StatusInfo.maxHP };
			m_StatusInfo.maxHP *= 0.6f;
			m_StatusInfo.HP = m_StatusInfo.maxHP * hpPercent;
			SendSpeedPacket(SpeedType::AttackSpeed);
		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_StatusInfo.WSkillCoolTime *= 0.1f;
			m_StatusInfo.WSkillPower *= 0.4f;
		}
	}

	index++;
}

void CPlayer::ApplyBowSP(SpecialType curSP)
{
	static int index{ 0 };

	if (index == 0)
	{
		if (curSP == SpecialType::AttackSpecial)
		{
			m_StatusInfo.QSkillPower *= 1.3f;
		}
		else if (curSP == SpecialType::DefenceSpecial)
		{
			m_StatusInfo.Def *= 1.2f;
		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_detectRange *= 1.15f;
			m_sightRange *= 1.15f;
		}
	}
	else if (index == 1)
	{
		if (curSP == SpecialType::AttackSpecial)
		{
			m_StatusInfo.ESkillRange *= 1.4f;
		}
		else if (curSP == SpecialType::DefenceSpecial)
		{
			m_StatusInfo.Def *= 1.1f;
			float hpPercent{ m_StatusInfo.HP / m_StatusInfo.maxHP };
			m_StatusInfo.maxHP *= 1.2f;
			m_StatusInfo.HP = m_StatusInfo.maxHP * hpPercent;
		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_StatusInfo.WSkillPower *= 1.5f;
		}
	}
	else if (index == 2)
	{
		if (curSP == SpecialType::AttackSpecial)
		{
			m_StatusInfo.QSkillPower *= 1.25f;
		}
		else if (curSP == SpecialType::DefenceSpecial)
		{
			m_StatusInfo.WalkSpeed *= 1.3f;
			SendSpeedPacket(SpeedType::WalkSpeed);
		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_StatusInfo.WSkillCoolTime *= 0.5f;
		}
	}
	else if (index == 3)
	{
		if (curSP == SpecialType::AttackSpecial)
		{
			m_StatusInfo.WSkillRange *= 1.5f;
			m_StatusInfo.ESkillRange *= 1.5f;
			float hpPercent{ m_StatusInfo.HP / m_StatusInfo.maxHP };
			m_StatusInfo.maxHP *= 0.6f;
			m_StatusInfo.HP = m_StatusInfo.maxHP * hpPercent;
		}
		else if (curSP == SpecialType::DefenceSpecial)
		{
			m_StatusInfo.Def *= 2.0f;
			float hpPercent{ m_StatusInfo.HP / m_StatusInfo.maxHP };
			m_StatusInfo.maxHP *= 0.7f;
			m_StatusInfo.HP = m_StatusInfo.maxHP * hpPercent;
		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_StatusInfo.WSkillCoolTime *= 0.1f;
			m_StatusInfo.WSkillPower *= 0.4f;
		}
	}

	index++;
}

void CPlayer::SendSpeedPacket(SpeedType type)
{
	SC_Msg_Change_Speed p;
	p.size = sizeof(p);
	p.type = SC_CHANGE_SPEED;
	p.Target_Tag = (short)m_tag;
	p.Speed_Type = (BYTE)type;
	if (type == SpeedType::WalkSpeed)
	{
		p.Changed_Speed = m_StatusInfo.WalkSpeed;
	}
	else if (type == SpeedType::AttackSpeed)
	{
		p.Changed_Speed = m_StatusInfo.AtkSpeed;
	}
	for (int j = 0; j < MAX_USER; ++j) {
		if (g_clients[j].m_isconnected == true) {
			SendPacket(j, &p);
		}
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
