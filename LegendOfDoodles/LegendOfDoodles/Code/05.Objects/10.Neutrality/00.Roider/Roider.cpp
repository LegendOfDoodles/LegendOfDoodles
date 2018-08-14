#include "stdafx.h"
#include "Roider.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

/// <summary>
/// 목적: 중립 몬스터(로이더) 클래스 분할
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-07
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CRoider::CRoider(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CAnimatedObject(pCreateMgr, nMeshes)
{
	m_ObjectType = ObjectType::Roider;

	m_sightRange = CONVERT_PaperUnit_to_InG(80.0f);
	m_detectRange = CONVERT_PaperUnit_to_InG(40.0f);
	m_speed = CONVERT_cm_to_InG(3.237f);

	// Warning! 로이더 스테이터스 설정 필요
	m_StatusInfo.HP = m_StatusInfo.maxHP = 100.0f;
	m_StatusInfo.Def = 10.0f;
	m_StatusInfo.Atk = 200.0f;
	m_StatusInfo.Exp = 100;

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

void CRoider::Render(CCamera * pCamera, UINT instanceCnt)
{
	OnPrepareRender();

	if (!IsVisible(pCamera) || !m_Detected) return;

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

void CRoider::SetState(StatesType newState, shared_ptr<CWayFinder> pWayFinder)
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
		if (Attackable(m_pEnemy))
		{
			SetAnimation(Animations::Attack1);
		}
		else if (AttackableFarRange(m_pEnemy))
		{
			SetAnimation(Animations::Attack2);
		}
		m_fFrameTime = 0;
		break;
	case States::Die:
		m_nCurrAnimation = Animations::Die;
		m_fFrameTime = 0;
		m_StatusInfo.HP = 0.f;
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

void CRoider::PlayWalk(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (NoneDestination(PathType::Sub))
	{
		if(MoveToDestination(timeElapsed, pWayFinder) == States::Done) SetState(States::Idle);
	}
	else
	{
		MoveToSubDestination(timeElapsed);
	}
}

void CRoider::PlayChase(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (Chaseable(m_pEnemy) && !FarFromSpawnLocation())
	{
		MoveToSubDestination(timeElapsed, pWayFinder);
	}
}

void CRoider::PlayAttack(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	UNREFERENCED_PARAMETER(timeElapsed);
	UNREFERENCED_PARAMETER(pWayFinder);

	if (Attackable(m_pEnemy))
	{
		if (m_nCurrAnimation != Animations::Attack1)
			m_nNextAnimation = Animations::Attack1;
	}
	else if (AttackableFarRange(m_pEnemy))
	{
		if (m_nCurrAnimation != Animations::Attack2)
			m_nNextAnimation = Animations::Attack2;
	}
}

void CRoider::SaveCurrentState()
{
	m_xmf4x4SpawnWorld = m_xmf4x4World;
	m_spawnLocation = GetPosition();
}

void CRoider::ReadyToAtk(shared_ptr<CWayFinder> pWayFinder)
{
	if (m_lastDamageTeam == TeamType::Red)
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
	GenerateSubPathToPosition(pWayFinder, XMFLOAT3(pathBeg.To().x, curPos.y, pathBeg.To().y));
}

void CRoider::Respawn()
{
	m_activated = false;

	SetTeam(TeamType::Neutral);

	m_StatusInfo.HP = m_StatusInfo.maxHP;

	m_pColManager->AddCollider(this);

	m_xmf4x4World = m_xmf4x4SpawnWorld;
}

void CRoider::SetCollisionManager(shared_ptr<CCollisionManager> manager)
{
	m_pColManager = manager;
	m_pColManager->AddNeutralCollider(this);
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
			if (m_pEnemy) LookAt(m_pEnemy->GetPosition());
		}
		if (m_nCurrAnimation == Animations::Attack2)
		{
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f
				&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) {
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Roider_Dumbel);
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

void CRoider::GenerateSubPathToSpawnLocation(shared_ptr<CWayFinder> pWayFinder)
{
	ResetSubPath();
	m_subPath = pWayFinder->GetPathToPosition(
		GetPosition(),
		m_spawnLocation);
	m_subDestination = m_subPath->front().To();
	m_subPath->pop_front();
	LookAt(m_subDestination);
}

bool CRoider::FarFromSpawnLocation()
{
	if (m_TeamType != TeamType::Neutral) return false;
	float dstSqr = Vector3::DistanceSquare(GetPosition(), m_spawnLocation);
	return (dstSqr > MAX_RANGE_FROM_SPAWN_ROIDER * MAX_RANGE_FROM_SPAWN_ROIDER);
}
