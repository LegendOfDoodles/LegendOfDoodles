#include "stdafx.h"
#include "Roider.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

/// <summary>
/// 목적: 중립 몬스터(로이더) 클래스 분할
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-11
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CRoider::CRoider() : CAnimatedObject()
{
	m_ObjectType = ObjectType::Roider;

	m_sightRange = CONVERT_PaperUnit_to_InG(80.0f);
	m_detectRange = CONVERT_PaperUnit_to_InG(40.0f);
	m_speed = CONVERT_cm_to_InG(3.237f);

	m_StatusInfo.HP = m_StatusInfo.maxHP = 980.0f;
	m_StatusInfo.Def = 30.0f;
	m_StatusInfo.Atk = 80.0f;
	m_StatusInfo.Exp = 153;

	m_attackRange = CONVERT_PaperUnit_to_InG(16);
	m_farAttackRange = CONVERT_PaperUnit_to_InG(30);
}

CRoider::~CRoider()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CRoider::Animate(float timeElapsed)
{
	if (m_curState != States::Die && m_curState != States::Remove)
		Recovery(timeElapsed);

	m_hpSyncCoolTime += timeElapsed;

	AdjustAnimationIndex();
	AnimateByCurState();

	if (m_curState != States::Remove)
	{
		m_fPreFrameTime = m_fFrameTime;
		m_fFrameTime += 30 * timeElapsed;
	}

	if (m_fFrameTime > m_nAniLength[m_nAniIndex]) {
		while (m_fFrameTime > m_nAniLength[m_nAniIndex])
			m_fFrameTime -= m_nAniLength[m_nAniIndex];
	}

	CAnimatedObject::Animate(timeElapsed);
}

void CRoider::SetState(StatesType newState)
{
	m_nextState = m_curState = newState;

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (g_clients[i].m_isconnected) {
			SC_Msg_Set_Monster_State p;
			p.Monster_State = (BYTE)newState;
			p.Monster_Tag = (short)m_tag;
			p.size = sizeof(p);
			p.type = SC_MONSTER_STATE;
			SendPacket(i, &p);
		}
	}

	switch (newState)
	{
	case States::Idle:
		m_returning = false;
		SetAnimation(Animations::Idle);
		break;
	case States::Walk:
		RegenerateLookAt();
		m_availableTime = 0.0f;
		m_nCurrAnimation = Animations::StartWalk;
		break;
	case States::Chase:
		m_availableTime = 0.0f;
		m_nCurrAnimation = Animations::StartWalk;
		break;
	case States::Attack:
		ResetRecovery();
		SetAnimation(Animations::Attack1);
		m_fFrameTime = 0;
		break;
	case States::Die:
		m_nCurrAnimation = Animations::Die;
		m_fFrameTime = 0;
		SetPathToGo(NULL);
		break;
	case States::Remove:
		m_spawnCoolTime = COOLTIME_SPAWN_ROIDER;
		break;
	case States::Win:
	case States::Defeat:
		SetPathToGo(NULL);
		break;
	default:
		assert(!"Error:: There is No State");
	}
}

void CRoider::PlayIdle(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);

	if (!m_activated) return;

	if (m_activated && m_TeamType == TeamType::Neutral)
	{
		m_deactiveTime += timeElapsed;
		if (m_deactiveTime > TIME_ACTIVATE_CHECK)
		{
			m_activated = false;
			m_deactiveTime = 0.0f;
			// Warning! 회복 처리
			// 방안 1: 전체 회복
			// 방안 2: 일정 시간동안 몇 %의 체력 회복
		}
	}

	CCollisionObject* enemy{ m_pColManager->RequestNearObject(this, m_detectRange, m_TeamType) };

	if (!enemy) return;
	if (!Chaseable(enemy)) return;

	SetEnemy(enemy);

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (g_clients[i].m_isconnected)
		{
			SC_Msg_Enemy_Tag_Neutral p;
			p.Monster_Tag = (short)m_tag;
			p.Enemy_Tag = (short)enemy->GetTag();
			p.size = sizeof(p);
			p.type = SC_MONSTER_SET_ENEMY;
			SendPacket(i, &p);
		}
	}

	if (Attackable(enemy))
	{
		SetState(States::Attack);
	}
	else if (AttackableFarRange(enemy)) {
		SetState(States::Attack);
		SetAnimation(Animations::Attack2);
	}
	else SetState(States::Chase);
}

void CRoider::PlayWalk(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (NoneDestination(PathType::Sub))
	{
		if(MoveToDestination(timeElapsed, pWayFinder) == States::Done) SetState(States::Idle);
	}
	else
	{
		MoveToSubDestination(timeElapsed);
		// 중립인데 원래 위치로 돌아갔으면 상태를 아이들 상태로 전환한다.
		if(m_TeamType == TeamType::Neutral && NoneDestination(PathType::Sub))
		{ 
			SetState(States::Idle);
		}
	}
	if (!m_returning) PlayIdle(timeElapsed);
}

void CRoider::PlayChase(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (!Chaseable(m_pEnemy) || FarFromSpawnLocation())
	{
		SetEnemy(NULL);
		if (m_TeamType == TeamType::Neutral)
		{
			GenerateSubPathToSpawnLocation(pWayFinder);
			m_returning = true;
		}
		else
			GenerateSubPathToMainPath(pWayFinder);
		SetNextState(States::Walk);
	}
	else
	{
		MoveToSubDestination(timeElapsed, pWayFinder);
	}

	if (Attackable(m_pEnemy))
	{
		SetState(States::Attack);
	}
	else if (AttackableFarRange(m_pEnemy)) {
		SetState(States::Attack);
		SetAnimation(Animations::Attack2);
	}
}

void CRoider::PlayAttack(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	UNREFERENCED_PARAMETER(timeElapsed);

	if (!CheckEnemyState(m_pEnemy))
	{
		SetEnemy(NULL);
		if (m_TeamType == TeamType::Neutral)
		{
			GenerateSubPathToSpawnLocation(pWayFinder);
			m_returning = true;
		}
		else
			GenerateSubPathToMainPath(pWayFinder);
		SetState(States::Walk);
	}
	else if (Attackable(m_pEnemy))
	{
		if (m_nCurrAnimation != Animations::Attack1)
			m_nNextAnimation = Animations::Attack1;
	}
	else if (AttackableFarRange(m_pEnemy))
	{
		if (m_nCurrAnimation != Animations::Attack2)
			m_nNextAnimation = Animations::Attack2;
	}
	else
	{
		SetNextState(States::Chase);
	}
}

void CRoider::PlayRemove(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	m_spawnCoolTime -= timeElapsed;

	if (m_TeamType == TeamType::Neutral)
	{
		ReadyToAtk(pWayFinder);
	}
	else if(m_spawnCoolTime < 0.0f)
	{
		Respawn();
	}
}

void CRoider::SaveCurrentState()
{
	m_xmf4x4SpawnWorld = m_xmf4x4World;
	m_spawnLocation = GetPosition();
}

void CRoider::ReceiveDamage(float damage, CCollisionObject * pCol)
{
	// 이미 사망한 상태인 경우 대미지 처리를 하지 않는다.
	if (m_curState == States::Die || m_curState == States::Remove) { return; }

	ResetRecovery();

	m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def);

	if (m_StatusInfo.HP <= 0)
	{
		SetState(States::Die);
		if(m_lastDamageTeam == TeamType::Blue)
			m_pColManager->RequestIncreaseExp(this, m_sightRange, TeamType::Red, m_StatusInfo.Exp);
		else if(m_lastDamageTeam == TeamType::Red)
			m_pColManager->RequestIncreaseExp(this, m_sightRange, TeamType::Blue, m_StatusInfo.Exp);
	}
	else if (m_hpSyncCoolTime > COOLTIME_HP_SYNC)
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
	
	m_activated = true;
}

void CRoider::UpdateNeutralStatus()
{
	// 10분 이전 스탯 증가량 적용
	if (g_GameTime < 600.f)
	{
		m_StatusInfo.maxHP += INCREASE_COMMON_ROIDER_BF_HP;
		m_StatusInfo.Atk += INCREASE_COMMON_ROIDER_BF_ATK;
		m_StatusInfo.Def += INCREASE_COMMON_ROIDER_BF_DEF;
	}
	// 10분 이후 스탯 증가량 적용
	else
	{
		m_StatusInfo.maxHP += INCREASE_COMMON_ROIDER_AF_HP;
		m_StatusInfo.Atk += INCREASE_COMMON_ROIDER_AF_ATK;
		m_StatusInfo.Def += INCREASE_COMMON_ROIDER_AF_DEF;
	}
	m_StatusInfo.Exp += INCREASE_STATICOBJECT_EXP;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CRoider::AdjustAnimationIndex()
{
	switch (m_nCurrAnimation)
	{
	case Animations::Idle:
		m_nAniIndex = 0;
		break;
	case Animations::Attack1:
		m_nAniIndex = 3;
		break;
	case Animations::Attack2:
		m_nAniIndex = 4;
		break;
	case Animations::StartWalk:
		m_nAniIndex = 1;
		break;
	case Animations::Walking:
		m_nAniIndex = 2;
		break;
	case Animations::Die:
		m_nAniIndex = 5;
		break;
	}
}

void CRoider::AnimateByCurState()
{
	switch (m_curState) {
	case States::Idle:
		if (m_nCurrAnimation != Animations::Idle) m_nCurrAnimation = Animations::Idle;
		break;
	case States::Attack:
		if (GetAnimTimeRemainRatio() <= 0.05f)
		{
			LookAt(m_pEnemy->GetPosition());
		}
		if (m_nCurrAnimation == Animations::Attack1) 
		{
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f
				&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) {
				m_pColManager->RequestCollide(CollisionType::SECTERFORM, this, m_attackRange, 180, m_StatusInfo.Atk);
			}
		}
		else if (m_nCurrAnimation == Animations::Attack2)
		{
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f
				&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) {
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Roider_Dumbel, m_StatusInfo.Atk);
			}
		}
		if (m_curState != m_nextState)
		{
			if (GetAnimTimeRemainRatio() > 0.05f) break;
			SetState(m_nextState);
		}
		else if (m_nCurrAnimation != m_nNextAnimation)
		{
			if (GetAnimTimeRemainRatio() > 0.05f) break;
			m_nCurrAnimation = m_nNextAnimation;
			m_fFrameTime = 0;
		}
		break;
	case States::Walk:
	case States::Chase:
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
		if (m_nCurrAnimation != Animations::Die) m_nCurrAnimation = Animations::Die;
		if (GetAnimTimeRemainRatio() < 0.05)
		{
			SetState(States::Remove);
		}
		break;
	default:
		break;
	}
}

void CRoider::ReadyToAtk(shared_ptr<CWayFinder> pWayFinder)
{
	if(m_lastDamageTeam == TeamType::Red)
		SetTeam(TeamType::Red);
	else if (m_lastDamageTeam == TeamType::Blue)
		SetTeam(TeamType::Blue);

	m_StatusInfo.HP = m_StatusInfo.maxHP;

	m_pColManager->AddCollider(this);

	CPathEdge pathBeg{ m_pathes[WayType::Roider_Exit].front() };
	XMFLOAT3 curPos{ GetPosition() };
	float distSqr{ FLT_MAX };

	for (Path::iterator iter = m_pathes[WayType::Roider_Exit].begin(); iter != m_pathes[Minion_Species::Data_Prepare].end(); ++iter)
	{
		float curDistSqr{ Vector3::DistanceSquare(curPos, XMFLOAT3((*iter).To().x, curPos.y, (*iter).To().y)) };
		if (curDistSqr < distSqr)
		{
			pathBeg = (*iter);
			distSqr = curDistSqr;
		}
	}

	Path *newPath{ NULL };
	if (m_TeamType == TeamType::Blue)
	{
		// 위로 가야함
		if (curPos.z > TERRAIN_SIZE_HEIGHT * 0.5f)
		{
			newPath = new Path(m_pathes[WayType::Blue_Up]);
		}
		else
		{
			newPath = new Path(m_pathes[WayType::Blue_Down]);
		}
		newPath->remove_if([pathBeg](CPathEdge& edge) {
			return edge.To().x < pathBeg.To().x;
		});
	}
	else if (m_TeamType == TeamType::Red)
	{
		// 위로 가야함
		if (curPos.z > TERRAIN_SIZE_HEIGHT * 0.5f)
		{
			newPath = new Path(m_pathes[WayType::Red_Up]);
		}
		else
		{
			newPath = new Path(m_pathes[WayType::Red_Down]);
		}
		newPath->remove_if([pathBeg](CPathEdge& edge) {
			return edge.To().x > pathBeg.To().x;
		});
	}
	
	SetPathToGo(newPath);
	GenerateSubPathToMainPath(pWayFinder);
	SetState(StatesType::Walk);
	for (int i = 0; i < MAX_USER; ++i) {
		if (g_clients[i].m_isconnected) {
			SC_Msg_Monster_Ready_to_Attak p;
			p.Monster_Tag = (short)m_tag;
			p.size = sizeof(p);
			p.Team_Type = (BYTE)m_TeamType;
			p.type = SC_MONSTER_CHANGE_TEAM;
			SendPacket(i, &p);
		}
	}
}

void CRoider::Respawn()
{
	m_activated = false;

	SetState(StatesType::Idle);
	SetTeam(TeamType::Neutral);

	m_StatusInfo.HP = m_StatusInfo.maxHP;

	m_pColManager->AddCollider(this);

	m_xmf4x4World = m_xmf4x4SpawnWorld;

	for (int i = 0; i < MAX_USER; ++i) {
		if (g_clients[i].m_isconnected) {
			SC_Msg_Monster_Respawn p;
			p.Monster_Tag = (short)m_tag;
			p.size = sizeof(p);
			p.type = SC_MONSTER_RESPAWN;
			SendPacket(i, &p);
		}
	}

}

void CRoider::GenerateSubPathToSpawnLocation(shared_ptr<CWayFinder> pWayFinder)
{
	ResetSubPath();
	m_subPath = pWayFinder->GetPathToPosition(
		GetPosition(),
		m_spawnLocation);
	if (m_subPath)
	{
		m_subDestination = m_subPath->front().To();
		m_subPath->pop_front();
		LookAt(m_subDestination);
	}
}

bool CRoider::FarFromSpawnLocation()
{
	if (m_TeamType != TeamType::Neutral) return false;
	float dstSqr = Vector3::DistanceSquare(GetPosition(), m_spawnLocation);
	return (dstSqr > MAX_RANGE_FROM_SPAWN_ROIDER * MAX_RANGE_FROM_SPAWN_ROIDER);
}

bool CRoider::Heal(float timeElapsed)
{
	// 최대 체력 보다 작은 경우 진행
	if (m_StatusInfo.HP >= m_StatusInfo.maxHP) return false;
	// 1초에 10%씩 회복 / 1초에 한번 회복 이펙트 생성
	if (m_recoveryTime - m_lastRecoveryTime >= 1.f)
	{
		m_lastRecoveryTime = m_recoveryTime;
		// Warning! 회복 이펙트 생성
	}
	// 전체 체력의 10%씩 회복
	m_StatusInfo.HP += m_StatusInfo.maxHP * MAX_RECOVERY_PER_SEC * timeElapsed;

	// 최대 체력보다 많이 찼으면 최대 체력으로 보정
	if (m_StatusInfo.HP > m_StatusInfo.maxHP)
	{
		m_StatusInfo.HP = m_StatusInfo.maxHP;
	}

	return true;
}