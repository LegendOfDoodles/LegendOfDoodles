#include "stdafx.h"
#include "Golem.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

/// <summary>
/// 목적: 중립 몬스터(수호 골렘) 클래스 분할
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-10-09
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CGolem::CGolem(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CAnimatedObject(pCreateMgr, nMeshes)
{
	srand((unsigned)time(NULL));
	SetStatic(StaticType::Static);
	m_ObjectType = ObjectType::GOLEM;

	m_sightRange = CONVERT_PaperUnit_to_InG(100.0f);
	m_detectRange = CONVERT_PaperUnit_to_InG(75.0f);
	m_speed = CONVERT_cm_to_InG(7.682f);

	m_StatusInfo.HP = m_StatusInfo.maxHP = 955.0f;
	m_StatusInfo.Def = 90.0f;
	m_StatusInfo.Atk = 240.0f;
	m_StatusInfo.Exp = 7680;

	m_attackRange = CONVERT_PaperUnit_to_InG(30);

	BuildSelf(pCreateMgr);
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

void CGolem::Render(CCamera * pCamera, UINT instanceCnt)
{
	OnPrepareRender();

	if (!IsVisible(pCamera)||!m_Detected) return;

	if (m_pMaterial)
	{
		m_pMaterial->UpdateShaderVariables();
	}

	if (m_cbvGPUDescriptorHandle.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(6, m_cbvGPUDescriptorHandle);

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(instanceCnt);
		}
	}
}

void CGolem::SetState(StatesType newState, shared_ptr<CWayFinder> pWayFinder)
{
	if ((m_curState == States::Chase || m_curState == States::Attack) && newState == States::Walk)
	{
		SetEnemy(NULL);
		if (m_TeamType == TeamType::Neutral)
			GenerateSubPathToSpawnLocation(pWayFinder);
		else
			GenerateSubPathToMainPath(pWayFinder);
	}

	m_nextState = m_curState = newState;

	switch (newState)
	{
	case States::Idle:
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
		m_StatusInfo.HP = 0.f;
		SetPathToGo(NULL);
		break;
	case States::Remove:
		m_spawnCoolTime = COOLTIME_SPAWN_GOLEM;
		break;
	case States::Win:
	case States::Defeat:
		m_nCurrAnimation = Animations::Idle;
		SetPathToGo(NULL);
		break;
	default:
		assert(!"Error:: There is No State");
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
	}
}

void CGolem::PlayChase(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (Chaseable(m_pEnemy) && !FarFromSpawnLocation())
	{
		MoveToSubDestination(timeElapsed, pWayFinder);
	}
}

void CGolem::SaveCurrentState()
{
	m_xmf4x4SpawnWorld = m_xmf4x4World;
	m_spawnLocation = GetPosition();
}

void CGolem::SetCommonStatus(float maxhp, float atk, float def)
{
	m_StatusInfo.maxHP = maxhp;
	m_StatusInfo.Atk = atk;
	m_StatusInfo.Def = def;
}

void CGolem::SetHP(float maxHP, float curHP)
{
	m_StatusInfo.maxHP = maxHP;  
	m_StatusInfo.HP = curHP;

	ResetRecovery();
}

void CGolem::BuildSelf(shared_ptr<CCreateMgr> pCreateMgr)
{
	CSkinnedMesh *pGolemMesh = new CSkinnedMesh(pCreateMgr, "Resource//3D//Golem//Mesh//GuardGolem.meshinfo");

	CCubeMesh *pBoundingBoxMesh = new CCubeMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(28.0f), CONVERT_PaperUnit_to_InG(7.0f), CONVERT_PaperUnit_to_InG(20.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(11.0f));

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

	pGolemMesh->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(11.0f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(14.0f), CONVERT_PaperUnit_to_InG(7.0f), CONVERT_PaperUnit_to_InG(10.0f)));

	SetMesh(0, pGolemMesh);
	SetBoundingMesh(pBoundingBoxMesh);
	SetCollisionSize(CONVERT_PaperUnit_to_InG(28));

	SetAnimation(Animations::Sit);

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

	if (m_wayKind == 0)
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
		if (m_wayKind == 1)
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
		if (m_wayKind == 1)
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
	GenerateSubPathToPosition(pWayFinder, XMFLOAT3(pathBeg.To().x, curPos.y, pathBeg.To().y));
}

void CGolem::Respawn()
{
	m_activated = false;

	SetAnimation(Animations::Sit);
	SetTeam(TeamType::Neutral);

	m_StatusInfo.HP = m_StatusInfo.maxHP;

	m_pColManager->AddCollider(this);

	m_xmf4x4World = m_xmf4x4SpawnWorld;
}

void CGolem::SetCollisionManager(shared_ptr<CCollisionManager> manager)
{
	m_pColManager = manager;
	m_pColManager->AddNeutralCollider(this);
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
			if (m_pEnemy) LookAt(m_pEnemy->GetPosition());
		}
		if (m_nCurrAnimation == Animations::Attack1)
		{
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f
				&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) {
				
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex] / 4, EffectObjectType::Golem_StandardAttack_Effect);
				m_pSoundMgr->play(SOUND::Golem_StandardAttack_Sound, GetPosition());
			}
		}
		else if (m_nCurrAnimation == Animations::SpecialAttack1)
		{
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.666f
				&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.666f) {
				
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex] / 8, EffectObjectType::Golem_StumpAttack_Effect);
				m_pSoundMgr->play(SOUND::Golem_FootrollAttack_Sound, GetPosition());

				m_nNextAnimation = Animations::Attack1;
			}
		}
		else if (m_nCurrAnimation == Animations::SpecialAttack2)
		{
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.666f
				&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.666f) {
				
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex] / 3, EffectObjectType::Golem_SpecialAttack_Effect);
				m_pSoundMgr->play(SOUND::Golem_SpecialAttack_Sound, GetPosition());

				m_nNextAnimation = Animations::Attack1;
			}
		}

		if (m_nCurrAnimation != m_nNextAnimation)
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
		m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), 30, EffectObjectType::Recovery_Effect);
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
