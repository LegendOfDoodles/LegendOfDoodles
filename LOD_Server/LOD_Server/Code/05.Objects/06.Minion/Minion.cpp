#include "stdafx.h"
#include "Minion.h"

/// <summary>
/// 목적: 미니언 클래스 분할
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-10-07
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CMinion::CMinion() : CAnimatedObject()
{
	m_sightRange = CONVERT_PaperUnit_to_InG(80.0f);
	m_detectRange = CONVERT_PaperUnit_to_InG(57.0f);
	m_speed = CONVERT_cm_to_InG(2.3465f);
}

CMinion::~CMinion()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CMinion::Animate(float timeElapsed)
{
	m_hpSyncCoolTime += timeElapsed;

	AdjustAnimationIndex();

	m_fPreFrameTime = m_fFrameTime;
	m_fFrameTime += 30 * timeElapsed;

	if (m_fFrameTime > m_nAniLength[m_nAniIndex]) {
		while (m_fFrameTime > m_nAniLength[m_nAniIndex])
			m_fFrameTime -= m_nAniLength[m_nAniIndex];
	}

	CAnimatedObject::Animate(timeElapsed);
}

void CMinion::SetState(StatesType newState)
{
	m_nextState = m_curState = newState;
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (g_clients[i].m_isconnected) {
			if (newState != StatesType::Remove) {
				SC_Msg_Set_Minion_State p;
				p.Minion_State = (BYTE)newState;
				p.Minion_Tag = (short)m_tag;
				p.size = sizeof(p);
				p.type = SC_MINION_STATE;
				SendPacket(i, &p);
			}
		}
	}

	switch (newState)
	{
	case States::Idle:
		m_nCurrAnimation = Animations::Idle;
		break;
	case States::Walk:
		RegenerateLookAt();
		m_nCurrAnimation = Animations::StartWalk;
		break;
	case States::Chase:
		m_availableTime = 0.0f;
		m_nCurrAnimation = Animations::StartWalk;
		break;
	case States::Attack:
		m_nCurrAnimation = Animations::Attack1;
		m_fFrameTime = 0;
		break;
	case States::Die:
		m_nCurrAnimation = Animations::Die;
		m_fFrameTime = 0;
		SetPathToGo(NULL);
		break;
	case States::Remove:
		break;
	case States::Win:
	case States::Defeat:
		SetPathToGo(NULL);
		break;
	default:
		assert(!"Error:: There is No State");
	}
}

void CMinion::PlayIdle(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);

	CCollisionObject* enemy{ m_pColManager->RequestNearObject(this, m_detectRange, m_TeamType) };

	if (enemy)
	{
		if (!Chaseable(enemy)) return;
		
		SetEnemy(enemy);

		// 적 태그 정보 보내기
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (g_clients[i].m_isconnected)
			{
				SC_Msg_Enemy_Tag p;
				p.Minion_Tag = (short)m_tag;
				p.Enemy_Tag = (short)enemy->GetTag();
				p.size = sizeof(p);
				p.type = SC_SET_ENEMY;
				SendPacket(i, &p);
			}
		}
		if (Attackable(enemy)) SetState(States::Attack);
		else SetState(States::Chase);
	}
	else if (Walkable()) SetState(States::Walk);
}

void CMinion::PlayWalk(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (NoneDestination(PathType::Sub))
	{
		if (MoveToDestination(timeElapsed, pWayFinder) == States::Done) SetState(States::Idle);
	}
	else
	{
		MoveToSubDestination(timeElapsed);
	}
	PlayIdle(timeElapsed);
}

void CMinion::PlayChase(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (!Chaseable(m_pEnemy))
	{
		SetEnemy(NULL);
		GenerateSubPathToMainPath(pWayFinder);
		SetState(States::Walk);
	}
	else
	{
		MoveToSubDestination(timeElapsed, pWayFinder);
	}

	if (Attackable(m_pEnemy)) SetState(States::Attack);
}

void CMinion::PlayAttack(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	UNREFERENCED_PARAMETER(timeElapsed);

	if (!CheckEnemyState(m_pEnemy))
	{
		SetEnemy(NULL);
		GenerateSubPathToMainPath(pWayFinder);
		SetState(States::Walk);
	}
	else if (!Attackable(m_pEnemy))
	{
		SetNextState(States::Chase);
	}
}

void CMinion::ReceiveDamage(float damage, CCollisionObject * pCol)
{
	// 이미 사망한 상태인 경우 대미지 처리를 하지 않는다.
	if (m_curState == States::Die || m_curState == States::Remove) { return; }

	m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def);

	if (m_StatusInfo.HP <= 0)
	{
		SetState(States::Die);
		m_pColManager->RequestIncreaseExp(this, m_sightRange, m_TeamType, m_StatusInfo.Exp);
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
}

void CMinion::SetCommonStatus(CommonInfo * status)
{
	m_StatusInfo.maxHP = status->maxHP;
	m_StatusInfo.Atk = status->Atk;
	m_StatusInfo.Def = status->Def;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CMinion::AdjustAnimationIndex()
{
	switch (m_nCurrAnimation)
	{
	case Animations::Idle:
		m_nAniIndex = 0;
		break;
	case Animations::Attack1:
		m_nAniIndex = 1;
		break;
	case Animations::Attack2:
		m_nAniIndex = 2;
		break;
	case Animations::StartWalk:
		m_nAniIndex = 3;
		break;
	case Animations::Walking:
		m_nAniIndex = 4;
		break;
	case Animations::Die:
		m_nAniIndex = 5;
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//근접 미니언
// 생성자, 소멸자
CSwordMinion::CSwordMinion() : CMinion()
{
	SetType(ObjectType::SwordMinion);
	m_StatusInfo = g_SwordMinionStat;

	m_attackRange = CONVERT_PaperUnit_to_InG(10);
}

CSwordMinion::~CSwordMinion()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CSwordMinion::Animate(float timeElapsed)
{
	switch (m_curState) {
	case States::Idle:
		if (m_nCurrAnimation != Animations::Idle) m_nCurrAnimation = Animations::Idle;
		break;
	case States::Attack:
		if (GetAnimTimeRemainRatio() <= 0.05f)
		{
			if(m_pEnemy) LookAt(m_pEnemy->GetPosition());
		}
		if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f
			&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) {
			m_pColManager->RequestCollide(CollisionType::SECTERFORM, this, m_attackRange, 180, m_StatusInfo.Atk);
		}
		if (m_nCurrAnimation == Animations::Attack1) {
			if (m_curState == m_nextState)
			{
				if (m_fFrameTime < m_nAniLength[m_nAniIndex] / 2) break;
				m_nCurrAnimation = Animations::Attack2;
				m_fFrameTime = 0;
			}
			else
			{
				if (GetAnimTimeRemainRatio() > 0.05) break;
				SetState(m_nextState);
			}
		}
		else if (m_nCurrAnimation == Animations::Attack2)
		{
			if (m_curState == m_nextState)
			{
				if (m_fFrameTime < m_nAniLength[m_nAniIndex]) break;
				m_nCurrAnimation = Animations::Attack1;
				m_fFrameTime = 0;
			}
			else
			{
				if (GetAnimTimeRemainRatio() > 0.05) break;
				SetState(m_nextState);
			}
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
	CMinion::Animate(timeElapsed);
}

//////////////////////////////////////////////////////////////////////////
//마법 미니언
// 생성자, 소멸자
CMagicMinion::CMagicMinion() : CMinion()
{
	SetType(ObjectType::StaffMinion);
	m_StatusInfo = g_StaffMinionStat;

	m_attackRange = CONVERT_PaperUnit_to_InG(24);
}

CMagicMinion::~CMagicMinion()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CMagicMinion::Animate(float timeElapsed)
{
	switch (m_curState) {
	case States::Idle:
		if (m_nCurrAnimation != Animations::Idle) m_nCurrAnimation = Animations::Idle;
		break;
	case States::Attack:
		if (GetAnimTimeRemainRatio() <= 0.05f)
		{
			if(m_pEnemy) LookAt(m_pEnemy->GetPosition());
		}
		if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f
			&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) {
			m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Minion_Magic, m_StatusInfo.Atk);
		}
		if (m_nCurrAnimation == Animations::Attack1) {
			if (m_curState == m_nextState)
			{
				if (m_fFrameTime < m_nAniLength[m_nAniIndex] / 2) break;
				m_nCurrAnimation = Animations::Attack2;
				m_fFrameTime = 0;
			}
			else
			{
				if (GetAnimTimeRemainRatio() > 0.05) break;
				SetState(m_nextState);
			}
		}
		else if (m_nCurrAnimation == Animations::Attack2)
		{
			if (m_curState == m_nextState)
			{
				if (m_fFrameTime < m_nAniLength[m_nAniIndex]) break;
				m_nCurrAnimation = Animations::Attack1;
				m_fFrameTime = 0;
			}
			else
			{
				if (GetAnimTimeRemainRatio() > 0.05) break;
				SetState(m_nextState);
			}
		}
		break;
	case States::Walk:
		if (m_nCurrAnimation != Animations::StartWalk &&
			m_nCurrAnimation != Animations::Walking)
			m_nCurrAnimation = Animations::StartWalk;
		if (m_nCurrAnimation == Animations::StartWalk) {
			if (m_fFrameTime > m_nAniLength[m_nAniIndex] - 1)
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
	CMinion::Animate(timeElapsed);
}

//////////////////////////////////////////////////////////////////////////
//활 미니언
// 생성자, 소멸자
CBowMinion::CBowMinion() : CMinion()
{
	SetType(ObjectType::BowMinion);
	m_StatusInfo = g_BowMinionStat;

	m_attackRange = CONVERT_PaperUnit_to_InG(30);
}

CBowMinion::~CBowMinion()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CBowMinion::Animate(float timeElapsed)
{
	switch (m_curState) {
	case States::Idle:
		if (m_nCurrAnimation != Animations::Idle) m_nCurrAnimation = Animations::Idle;
		break;
	case States::Attack:
		if (GetAnimTimeRemainRatio() <= 0.05f)
		{
			if(m_pEnemy) LookAt(m_pEnemy->GetPosition());
		}
		if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f
			&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) {
			m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Minion_Arrow, m_StatusInfo.Atk);
		}
		if (m_nCurrAnimation == Animations::Attack1)
		{
			if (m_curState == m_nextState)
			{
				if (m_fFrameTime < m_nAniLength[m_nAniIndex]) break;
				m_fFrameTime = 0;
			}
			else
			{
				if (GetAnimTimeRemainRatio() > 0.05) break;
				SetState(m_nextState);
			}
		}
		break;
	case States::Walk:
		if (m_nCurrAnimation != Animations::StartWalk &&
			m_nCurrAnimation != Animations::Walking)
			m_nCurrAnimation = Animations::StartWalk;

		if (m_nCurrAnimation == Animations::StartWalk) {
			if (m_fFrameTime > m_nAniLength[m_nAniIndex] - 1)
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
	CMinion::Animate(timeElapsed);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CBowMinion::AdjustAnimationIndex()
{
	switch (m_nCurrAnimation)
	{
	case Animations::Idle:
		m_nAniIndex = 0;
		break;
	case Animations::Attack1:
		m_nAniIndex = 1;
		break;
	case Animations::StartWalk:
		m_nAniIndex = 3;
		break;
	case Animations::Walking:
		m_nAniIndex = 4;
		break;
	case Animations::Die:
		m_nAniIndex = 5;
		break;
	}
}
