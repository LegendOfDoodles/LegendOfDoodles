#include "stdafx.h"
#include "Golem.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

/// <summary>
/// 목적: 중립 몬스터(수호 골렘) 클래스 분할
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-11
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CGolem::CGolem() : CAnimatedObject()
{
	srand((unsigned)time(NULL));

	m_ObjectType = ObjectType::GOLEM;

	m_sightRange = CONVERT_PaperUnit_to_InG(100.0f);
	m_detectRange = CONVERT_PaperUnit_to_InG(80.0f);
	m_speed = CONVERT_cm_to_InG(7.682f);

	m_StatusInfo.HP = m_StatusInfo.maxHP = 2800.0f;
	m_StatusInfo.Def = 90.0f;
	m_StatusInfo.Atk = 240.0f;
	m_StatusInfo.Exp = 900;

	m_attackRange = CONVERT_PaperUnit_to_InG(30);

	BuildSelf();
}

CGolem::~CGolem()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CGolem::Animate(float timeElapsed)
{
	if (m_curState != States::Die && m_curState != States::Remove)
		Recovery(timeElapsed);

	m_hpSyncCoolTime += timeElapsed;

	AdjustAnimationIndex();
	AnimateByCurState();

	// 스킬 쿨타임 감소 적용
	if (m_curState != States::Idle)
	{
		m_spAttack1CoolTime -= timeElapsed;
		m_spAttack2CoolTime -= timeElapsed;
	}

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


void CGolem::SetState(StatesType newState)
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
		m_spawnCoolTime = COOLTIME_SPAWN_GOLEM;
		break;
	case States::Win:
	case States::Defeat:
		SetPathToGo(NULL);
		break;
	default:
		assert(!"Error:: There is No State");
	}
}

void CGolem::PlayIdle(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);

	if (!m_activated) return;

	if (m_activated && m_TeamType == TeamType::Neutral)
	{
		m_deactiveTime += timeElapsed;
		if (m_deactiveTime > TIME_ACTIVATE_CHECK_GOLEM)
		{
			m_activated = false;
			m_deactiveTime = 0.0f;
			m_nCurrAnimation = Animations::IdleToSit;
			for (int i = 0; i < MAX_USER; ++i) {
				if (g_clients[i].m_isconnected) {
					SC_Msg_Boss_Idle_Animation p;
					p.Animation = (BYTE)m_nCurrAnimation;
					p.Monster_Tag = (short)m_tag;
					p.size = sizeof(p);
					p.type = SC_BOSS_SET_CURR_ANIM;
					SendPacket(i, &p);
				}
			}

			
			// Warning! 회복 처리
			// 방안 1: 전체 회복
			// 방안 2: 일정 시간동안 몇 %의 체력 회복
		}
	}

	if (m_nCurrAnimation == Animations::Idle ||
		m_curState == States::Walk)
	{
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
		else SetState(States::Chase);
	}
	if (static_cast<int>(g_GameTime) % 60 == 0)
	{
		m_StatusInfo.maxHP += 180;
		m_StatusInfo.Atk += 8;
		m_StatusInfo.Def += 2;
		SC_Msg_Update_Golem_Stat p;
		p.Monster_Tag = (short)m_tag;
		p.maxHP = m_StatusInfo.maxHP;
		p.atk = m_StatusInfo.Atk;
		p.def = m_StatusInfo.Def;
		p.size = sizeof(p);
		p.type = SC_UPDATE_GOLEM_STAT;
		for (int i = 0; i < MAX_USER; ++i) {
			if (g_clients[i].m_isconnected) SendPacket(i, &p);
		}
	}
}

void CGolem::PlayWalk(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (NoneDestination(PathType::Sub))
	{
		if (MoveToDestination(timeElapsed, pWayFinder) == States::Done) SetState(States::Idle);
	}
	else
	{
		MoveToSubDestination(timeElapsed);
		// 중립인데 원래 위치로 돌아갔으면 상태를 아이들 상태로 전환한다.
		if (m_TeamType == TeamType::Neutral && NoneDestination(PathType::Sub))
		{
			SetState(States::Idle);
		}
	}
	if(!m_returning) PlayIdle(timeElapsed);
}

void CGolem::PlayChase(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
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
		SetState(States::Walk);
	}
	else
	{
		MoveToSubDestination(timeElapsed, pWayFinder);
	}

	if (Attackable(m_pEnemy))
	{
		SetState(States::Attack);
	}
}

void CGolem::PlayAttack(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
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
		SetNextState(States::Walk);
	}
	else if (Attackable(m_pEnemy))
	{
		int usingSkill{ rand() % 2 };
		if (usingSkill == 1 && m_spAttack1CoolTime <= 0.0f)
		{
			m_nNextAnimation = Animations::SpecialAttack1;
			m_spAttack1CoolTime = COOLTIME_SPECIAL_ATTACK1;
			for (int i = 0; i < MAX_USER; ++i) {
				if (g_clients[i].m_isconnected) {
					SC_Msg_Boss_Idle_Animation p;
					p.Animation = (BYTE)m_nNextAnimation;
					p.Monster_Tag = (short)m_tag;
					p.size = sizeof(p);
					p.type = SC_BOSS_SET_NEXT_ANIM;
					SendPacket(i, &p);
				}
			}
		}
		else if (usingSkill == 1 && m_spAttack2CoolTime <= 0.0f)
		{
			m_nNextAnimation = Animations::SpecialAttack2;
			m_spAttack2CoolTime = COOLTIME_SPECIAL_ATTACK2;
			for (int i = 0; i < MAX_USER; ++i) {
				if (g_clients[i].m_isconnected) {
					SC_Msg_Boss_Idle_Animation p;
					p.Animation = (BYTE)m_nNextAnimation;
					p.Monster_Tag = (short)m_tag;
					p.size = sizeof(p);
					p.type = SC_BOSS_SET_NEXT_ANIM;
					SendPacket(i, &p);
				}
			}
		}
	}
	else
	{
		SetNextState(States::Chase);
	}
}

void CGolem::PlayRemove(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	m_spawnCoolTime -= timeElapsed;

	if (m_TeamType == TeamType::Neutral)
	{
		ReadyToAtk(pWayFinder);
	}
	else if (m_spawnCoolTime < 0.0f)
	{
		Respawn();
	}
}

void CGolem::SaveCurrentState()
{
	m_xmf4x4SpawnWorld = m_xmf4x4World;
	m_spawnLocation = GetPosition();
}

void CGolem::UpdateNeutralStatus()
{
	// 10분 이전 스탯 증가량 적용
	if (g_GameTime < 600.f)
	{
		m_StatusInfo.maxHP += 110;
		m_StatusInfo.Atk += 8;
		m_StatusInfo.Def += 2;
	}
	// 10분 이후 스탯 증가량 적용
	else
	{
		m_StatusInfo.maxHP += 130;
		m_StatusInfo.Atk += 16;
		m_StatusInfo.Def += 3;
	}
	m_StatusInfo.Exp += 5;
}

void CGolem::ReceiveDamage(float damage, CCollisionObject * pCol)
{
	// 이미 사망한 상태인 경우 데미지 처리를 하지 않는다.
	if (m_curState == States::Die || m_curState == States::Remove) { return; }

	m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def);
	if (m_StatusInfo.HP <= 0 && m_pEnemy) {
		PlayerInfo* PlayerStatus{ m_pEnemy->GetPlayerStatus() };
		if (m_pEnemy->GetTag() >= 10000 && m_pEnemy->GetTag() < 20000)
		{
			PlayerStatus->Exp += 900;
			while (PlayerStatus->Exp > PlayerStatus->Level * 110 + 170) {
				if (PlayerStatus->Level * 110 + 170 <= PlayerStatus->Exp) {
					PlayerStatus->Exp -= PlayerStatus->Level * 110 + 170;
					m_pEnemy->LevelUP(m_pEnemy);
					SC_Msg_Level_Up p;
					p.Target_Tag = (short)m_pEnemy->GetTag();
					p.size = sizeof(p);
					p.type = SC_LEVEL_UP;
					for (int j = 0; j < MAX_USER; ++j) {
						if (g_clients[j].m_isconnected == true) {
							SendPacket(j, &p);
						}
					}
				}
			}
			SC_Msg_Exp_Up p;
			p.Target_Tag = (short)m_pEnemy->GetTag();
			p.exp = 900;
			p.size = sizeof(p);
			p.type = SC_EXP_UP;
			for (int j = 0; j < MAX_USER; ++j) {
				if (g_clients[j].m_isconnected == true) {
					SendPacket(j, &p);
				}
			}
		}
	}
	ResetRecovery();

	if (m_hpSyncCoolTime > COOLTIME_HP_SYNC)
	{
		SC_Msg_Hp_Sync hpPacket;
		hpPacket.curhp = m_StatusInfo.HP;
		hpPacket.maxhp = m_StatusInfo.maxHP;
		hpPacket.size = sizeof(hpPacket);
		hpPacket.type = SC_HP_SYNC;
		hpPacket.Target_Tag = (short)m_tag;
		hpPacket.Object_Type = (short)pCol->GetType();
		hpPacket.Flying_Type = (short)pCol->GetFlyingObjectsType();
		hpPacket.updatetime = g_GameTime;
		for (int j = 0; j < MAX_USER; ++j) {
			if (g_clients[j].m_isconnected == true) {
				SendPacket(j, &hpPacket);
			}
		}
		m_hpSyncCoolTime = 0.0f;
	}

	if (m_StatusInfo.HP <= 0 && m_curState != States::Die) {
		SetState(States::Die);
	}

	if (!m_activated)
	{
		m_activated = true;
		if (m_nCurrAnimation == Animations::Sit)
		{
			m_nCurrAnimation = Animations::SitToIdle;
			for (int i = 0; i < MAX_USER; ++i) {
				if (g_clients[i].m_isconnected) {
					SC_Msg_Boss_Idle_Animation p;
					p.Animation = (BYTE)m_nCurrAnimation;
					p.Monster_Tag = (short)m_tag;
					p.size = sizeof(p);
					p.type = SC_BOSS_SET_CURR_ANIM;
					SendPacket(i, &p);
				}
			}
		}
	}
}

void CGolem::BuildSelf()
{
	CSkeleton *pSIdle = new CSkeleton("Resource//3D//Golem//Animation//Golem_Idle.aniinfo");
	CSkeleton *pSIdleToSit = new CSkeleton("Resource//3D//Golem//Animation//Golem_Idle_To_Sit.aniinfo");
	CSkeleton *pSSitToIdle = new CSkeleton("Resource//3D//Golem//Animation//Golem_Sit_To_Idle.aniinfo");
	CSkeleton *pSSit = new CSkeleton("Resource//3D//Golem//Animation//Golem_Sit.aniinfo");
	CSkeleton *pSStartWalk = new CSkeleton("Resource//3D//Golem//Animation//Golem_Start_Walk.aniinfo");
	CSkeleton *pSWalk = new CSkeleton("Resource//3D//Golem//Animation//Golem_Walk.aniinfo");
	CSkeleton *pSSmash = new CSkeleton("Resource//3D//Golem//Animation//Golem_Basic_Attack.aniinfo");
	CSkeleton *pSpAttack1 = new CSkeleton("Resource//3D//Golem//Animation//Golem_Special_Attack1.aniinfo");
	CSkeleton *pSpAttack2 = new CSkeleton("Resource//3D//Golem//Animation//Golem_Special_Attack2.aniinfo");
	CSkeleton *pSDie = new CSkeleton("Resource//3D//Golem//Animation//Golem_Die.aniinfo");

	SetCollisionSize(CONVERT_PaperUnit_to_InG(14));

	SetAnimation(AnimationsType::Sit);

	SetSkeleton(pSIdle);
	SetSkeleton(pSIdleToSit);
	SetSkeleton(pSSitToIdle);
	SetSkeleton(pSSit);
	SetSkeleton(pSStartWalk);
	SetSkeleton(pSWalk);
	SetSkeleton(pSSmash);
	SetSkeleton(pSpAttack1);
	SetSkeleton(pSpAttack2);
	SetSkeleton(pSDie);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CGolem::AdjustAnimationIndex()
{
	switch (m_nCurrAnimation)
	{
	case Animations::Idle:
		m_nAniIndex = 0;
		break;
	case Animations::IdleToSit:
		m_nAniIndex = 1;
		break;
	case Animations::SitToIdle:
		m_nAniIndex = 2;
		break;
	case Animations::Sit:
		m_nAniIndex = 3;
		break;
	case Animations::StartWalk:
		m_nAniIndex = 4;
		break;
	case Animations::Walking:
		m_nAniIndex = 5;
		break;
	case Animations::Attack1:
		m_nAniIndex = 6;
		break;
	case Animations::SpecialAttack1:
		m_nAniIndex = 7;
		break;
	case Animations::SpecialAttack2:
		m_nAniIndex = 8;
		break;
	case Animations::Die:
		m_nAniIndex = 9;
		break;
	}
}

void CGolem::AnimateByCurState()
{
	switch (m_curState) {
	case States::Idle:
		if (m_nCurrAnimation == Animations::IdleToSit &&
			GetAnimTimeRemainRatio() <= 0.05f)
		{
			m_nCurrAnimation = Animations::Sit;
		}
		else if (m_nCurrAnimation == Animations::SitToIdle &&
			GetAnimTimeRemainRatio() <= 0.05f)
		{
			m_nCurrAnimation = Animations::Idle;
		}
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
				m_pColManager->RequestCollide(CollisionType::SPHERE, this, m_fCollisionSize, m_fCollisionSize * 0.5f, m_StatusInfo.Atk);
			}
		}
		else if (m_nCurrAnimation == Animations::SpecialAttack1)
		{
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.666f
				&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.666f) {
				m_pColManager->RequestCollide(CollisionType::SPHERE, this, 0, m_attackRange, m_StatusInfo.Atk * 2);
				m_nNextAnimation = Animations::Attack1;
			}
		}
		else if (m_nCurrAnimation == Animations::SpecialAttack2)
		{
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.666f
				&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.666f) {
				m_pColManager->RequestCollide(CollisionType::SPHERE, this, 0, m_attackRange, m_StatusInfo.Atk * 3);
				m_nNextAnimation = Animations::Attack1;
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

void CGolem::ReadyToAtk(shared_ptr<CWayFinder> pWayFinder)
{
	if (m_lastDamageTeam == TeamType::Red)
		SetTeam(TeamType::Red);
	else if (m_lastDamageTeam == TeamType::Blue)
		SetTeam(TeamType::Blue);
	
	m_StatusInfo.HP = m_StatusInfo.maxHP;

	m_pColManager->AddCollider(this);

	CPathEdge pathBeg{ m_pathes[WayType::Golem_Exit].front() };
	XMFLOAT3 curPos{ GetPosition() };

	int wayKind = rand() % 2; // 0: 아래, 1: 위
	if (wayKind == 0)
	{
		pathBeg = m_pathes[WayType::Golem_Exit].front();
	}
	else
	{
		pathBeg = m_pathes[WayType::Golem_Exit].back();
	}

	Path *newPath{ NULL };
	if (m_TeamType == TeamType::Blue)
	{
		// 위로 가야함
		if (wayKind == 1)
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
		if (wayKind == 1)
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
			p.Way_Kind = (BYTE)wayKind;
			p.type = SC_MONSTER_CHANGE_TEAM;
			SendPacket(i, &p);
		}
	}
}

void CGolem::Respawn()
{
	m_activated = false;

	SetState(StatesType::Idle);
	SetAnimation(Animations::Sit);
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

void CGolem::GenerateSubPathToSpawnLocation(shared_ptr<CWayFinder> pWayFinder)
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

bool CGolem::FarFromSpawnLocation()
{
	if (m_TeamType != TeamType::Neutral) return false;
	float dstSqr = Vector3::DistanceSquare(GetPosition(), m_spawnLocation);
	return (dstSqr > MAX_RANGE_FROM_SPAWN_GOLEM * MAX_RANGE_FROM_SPAWN_GOLEM);
}

bool CGolem::Heal(float timeElapsed)
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
