#include "stdafx.h"
#include "Player.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

/// <summary>
/// 목적: 플레이어 관리 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-09-06
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CPlayer::CPlayer() : CAnimatedObject()
{
	m_detectRange = CONVERT_PaperUnit_to_InG(80.0f);
	m_sightRange = CONVERT_PaperUnit_to_InG(80.0f);
	SetSpeed(CONVERT_cm_to_InG(3.285f));
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
		if (GetType() == ObjectType::StickPlayer)
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
				m_pColManager->RequestCollide(CollisionType::SECTERFORM, this, CONVERT_PaperUnit_to_InG(24), 180, m_StatusInfo.Atk * m_StatusInfo.WSkillPower);
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
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_Magic, m_StatusInfo.Atk, m_StatusInfo.AtkRange);
			}
			else if (m_nCurrAnimation == Animations::SkillQ &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_MagicSkill_Q, m_StatusInfo.Atk * m_StatusInfo.QSkillPower, m_StatusInfo.QSkillRange);
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
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_MagicSkill_R, m_StatusInfo.Atk * m_StatusInfo.RSkillPower, m_StatusInfo.RSkillRange);
			}
		}
		else if (GetType() == ObjectType::BowPlayer)
		{
			if (m_nCurrAnimation == Animations::Attack1 &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_Arrow, m_StatusInfo.Atk, m_StatusInfo.AtkRange);
			}
			else if (m_nCurrAnimation == Animations::SkillQ &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_ArrowSkill_Q, m_StatusInfo.Atk * m_StatusInfo.QSkillPower, m_StatusInfo.QSkillRange);
			}
			else if (m_nCurrAnimation == Animations::SkillW &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_ArrowSkill_W, m_StatusInfo.Atk * m_StatusInfo.WSkillPower, m_StatusInfo.WSkillRange);
			}
			else if (m_nCurrAnimation == Animations::SkillE &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_ArrowSkill_E, m_StatusInfo.Atk * m_StatusInfo.ESkillPower, m_StatusInfo.ESkillRange);
			}
			else if (m_nCurrAnimation == Animations::SkillR &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_ArrowSkill_R, m_StatusInfo.Atk * m_StatusInfo.RSkillPower, m_StatusInfo.RSkillRange);
			}
		}
		m_fPreFrameTime = m_fFrameTime;
		m_fFrameTime += ANIMATION_SPEED * timeElapsed * m_StatusInfo.AtkSpeed;

		if (m_fFrameTime >= m_nAniLength[m_nAniIndex] - 1)
		{
			SetState(States::Idle);
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

ProcessType CPlayer::MoveToDestination(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (m_curState != States::Walk) return States::Processing;
	if (!m_mainPath) return States::Done;

	if (NoneDestination() || IsArrive(m_speed * timeElapsed * m_StatusInfo.WalkSpeed))	//  도착 한 경우
	{
		if (m_mainPath->empty())
		{
			Safe_Delete(m_mainPath);
			ResetDestination();
			return States::Done;
		}
		else
		{
			m_destination = m_mainPath->front().To();
			m_mainPath->pop_front();
			LookAt(m_destination);
		}
	}
	else  // 아직 도착하지 않은 경우
	{
		MoveForward(m_speed * timeElapsed * m_StatusInfo.WalkSpeed);
		XMFLOAT3 position = GetPosition();
		position.y = m_pTerrain->GetHeight(position.x, position.z);
		CBaseObject::SetPosition(position);
		CheckRightWay(PathType::Main, pWayFinder);
	}
	return States::Processing;
}

void CPlayer::MoveToSubDestination(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (pWayFinder != NULL)
	{
		m_availableTime -= timeElapsed;
		if (m_availableTime <= 0.0f)
		{
			XMFLOAT3 myPos{ GetPosition() };
			XMFLOAT3 enemyPos{ m_pEnemy->GetPosition() };
			m_availableTime = TIME_AVAILABILITY_CHECK;
			ResetSubPath();
			m_subPath = pWayFinder->GetPathToPosition(
				myPos,
				enemyPos);
			if (m_subPath)
			{
				m_subPath->push_back(CPathEdge(XMFLOAT2(enemyPos.x, enemyPos.z), Vector3::ToVector2(Vector3::Add(enemyPos, Vector3::Subtract(enemyPos, myPos)))));
			}
		}
	}

	if (NoneDestination(PathType::Sub) || IsArrive(m_speed * timeElapsed * m_StatusInfo.WalkSpeed, PathType::Sub))	//  도착 한 경우
	{
		if (m_subPath == NULL || m_subPath->empty())
		{
			Safe_Delete(m_subPath);
			ResetDestination(PathType::Sub);
			LookAt(m_destination);
		}
		else
		{
			m_subDestination = m_subPath->front().To();
			m_subPath->pop_front();
			LookAt(m_subDestination);
		}
	}

	MoveForward(m_speed * timeElapsed * m_StatusInfo.WalkSpeed);
	XMFLOAT3 position = GetPosition();
	position.y = m_pTerrain->GetHeight(position.x, position.z);
	CBaseObject::SetPosition(position);
	CheckRightWay(PathType::Sub, pWayFinder);
}

void CPlayer::ActiveSkill(AnimationsType act)
{
	if (m_curState != States::Attack) {
		m_curState = States::Attack;
		m_nCurrAnimation = act;
		m_fFrameTime = 0;

		if (act == Animations::SkillQ)
		{
			m_skillCoolTimeQ = 0;
		}
		else if (act == Animations::SkillW)
		{
			m_skillCoolTimeW = 0;
		}
		else if (act == Animations::SkillE)
		{
			m_skillCoolTimeE = 0;
		}
		else if (act == Animations::SkillR)
		{
			m_skillCoolTimeR = 0;
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
	float evationAbsorbChecker{ RandInRange(0.f, 1.f) };
	if (evationAbsorbChecker < m_StatusInfo.EvationRate)
	{
		// Warning! 회피 처리 -> 이펙트 생성 요청 패킷 전송
	}
	else if (evationAbsorbChecker < m_StatusInfo.AbsorptionRate)
	{
		// Warning! 흡수 처리 -> 이펙트 생성 요청 패킷 전송
		m_StatusInfo.HP += damage * Compute_Defence(m_StatusInfo.Def) * m_StatusInfo.AbsorptionAmount;
		// 흡수 이후 HP량 최대값 넘지 않도록 조정
		if (m_StatusInfo.HP > m_StatusInfo.maxHP)
			m_StatusInfo.HP = m_StatusInfo.maxHP;
	}
	else
	{
		m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def);
	}

	if (m_StatusInfo.HP <= 0)
	{
		if (pCol->GetTag() >= 10000 && pCol->GetTag() < 20000)
		{
			pCol->GetPlayerStatus()->Kill++;
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (g_clients[i].m_isconnected) {
					SC_Msg_Set_Player_Kill p;
					p.Killer_Tag = (short)pCol->GetTag();
					p.size = sizeof(p);
					p.type = SC_SET_PLAYER_KILL;
					SendPacket(i, &p);
				}
			}
		}
		SetState(States::Die);
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (g_clients[i].m_isconnected) {
				SC_Msg_Set_Player_Death p;
				p.Player_Tag = (short)m_tag;
				p.size = sizeof(p);
				p.type = SC_SET_PLAYER_DEATH;
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

void CPlayer::ChangeWeapon(UINT weaponNum, ObjectType type)
{
	m_StatusInfo.Weapon = weaponNum;
	m_ObjectType = type;

	if (type == ObjectType::SwordPlayer)
	{
		m_StatusInfo.Atk *= 0.9f;

		m_StatusInfo.QSkillPower = 1.13f;
		m_StatusInfo.WSkillPower = 1.27f;
		m_StatusInfo.ESkillPower = 1.42f;
		m_StatusInfo.RSkillPower = 2.3f;
	}
	else if (type == ObjectType::StaffPlayer)
	{
		m_StatusInfo.Atk *= 1.4f;

		m_StatusInfo.QSkillPower = 1.02f;
		m_StatusInfo.WSkillPower = 1.15f;
		m_StatusInfo.ESkillPower = 1.29f;
		m_StatusInfo.RSkillPower = 1.64f;
	}
	else if (type == ObjectType::BowPlayer)
	{
		m_StatusInfo.Atk *= 1.5f;

		m_StatusInfo.QSkillPower = 1.12f;
		m_StatusInfo.WSkillPower = 0.9f;
		m_StatusInfo.ESkillPower = 0.82f;
		m_StatusInfo.RSkillPower = 2.3f;
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
			m_StatusInfo.Def *= 1.5f;
		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_detectRange *= 1.5f;
			m_sightRange *= 1.5f;
			SendSightPacket();
		}
	}
	else if (index == 1)
	{
		if (curSP == SpecialType::AttackSpecial)
		{
			m_StatusInfo.Atk *= 1.25f;
		}
		else if (curSP == SpecialType::DefenceSpecial)
		{
			m_StatusInfo.Def *= 2.5f;
		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_StatusInfo.QSkillCoolTime *= 0.5f;
		}
	}
	else if (index == 2)
	{
		if (curSP == SpecialType::AttackSpecial)
		{
			m_StatusInfo.WSkillPower *= 1.4f;
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

void CPlayer::ApplyBowSP(SpecialType curSP)
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
			m_StatusInfo.EvationRate += 0.05f;
			SendSpeedPacket(SpeedType::WalkSpeed);
		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_detectRange *= 1.5f;
			m_sightRange *= 1.5f;
			SendSightPacket();
		}
	}
	else if (index == 1)
	{
		if (curSP == SpecialType::AttackSpecial)
		{
			m_StatusInfo.AtkSpeed *= 1.1f;
			SendSpeedPacket(SpeedType::AttackSpeed);
		}
		else if (curSP == SpecialType::DefenceSpecial)
		{
			m_StatusInfo.EvationRate += 0.1f;
			SendSpeedPacket(SpeedType::WalkSpeed);
		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_StatusInfo.QSkillCoolTime *= 0.5f;
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

void CPlayer::ApplyStaffSP(SpecialType curSP)
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
			m_StatusInfo.AbsorptionRate += 0.03f;
			m_StatusInfo.AbsorptionAmount += 0.1f;
		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_detectRange *= 1.5f;
			m_sightRange *= 1.5f;
			SendSightPacket();
		}
	}
	else if (index == 1)
	{
		if (curSP == SpecialType::AttackSpecial)
		{
			m_StatusInfo.AtkRange *= 1.4f;
			SendRangePacket(RangeType::AtkRange);
		}
		else if (curSP == SpecialType::DefenceSpecial)
		{
			m_StatusInfo.AbsorptionRate += 0.07f;
			m_StatusInfo.AbsorptionAmount += 0.15f;
		}
		else if (curSP == SpecialType::TechnicSpecial)
		{
			m_StatusInfo.QSkillCoolTime *= 0.5f;
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

void CPlayer::SendSightPacket()
{
	SC_Msg_Change_Sight p;
	p.size = sizeof(p);
	p.type = SC_CHANGE_SIGHT;
	p.Target_Tag = (short)m_tag;
	p.Changed_Detect_Range = m_detectRange;
	p.Changed_Sight_Range = m_sightRange;
	for (int j = 0; j < MAX_USER; ++j) {
		if (g_clients[j].m_isconnected == true) {
			SendPacket(j, &p);
		}
	}
}

void CPlayer::SendRangePacket(RangeType type)
{
	SC_Msg_Change_Range p;
	p.size = sizeof(p);
	p.type = SC_CHANGE_RANGE;
	p.Target_Tag = (short)m_tag;
	p.Range_Type = (BYTE)type;

	if (type == RangeType::AtkRange)
	{
		p.Changed_Range = m_StatusInfo.AtkRange;
	}
	else if (type == RangeType::QSkillRange)
	{
		p.Changed_Range = m_StatusInfo.QSkillRange;
	}
	else if (type == RangeType::WSkillRange)
	{
		p.Changed_Range = m_StatusInfo.WSkillRange;
	}
	else if (type == RangeType::ESkillRange)
	{
		p.Changed_Range = m_StatusInfo.ESkillRange;
	}
	else if (type == RangeType::RSkillRange)
	{
		p.Changed_Range = m_StatusInfo.RSkillRange;
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
