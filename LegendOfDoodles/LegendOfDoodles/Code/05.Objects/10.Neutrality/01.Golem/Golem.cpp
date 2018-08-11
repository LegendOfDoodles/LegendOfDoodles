#include "stdafx.h"
#include "Golem.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

/// <summary>
/// ����: �߸� ����(��ȣ ��) Ŭ���� ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-08-03
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CGolem::CGolem(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CAnimatedObject(pCreateMgr, nMeshes)
{
	srand((unsigned)time(NULL));

	m_ObjectType = ObjectType::GOLEM;

	m_sightRange = CONVERT_PaperUnit_to_InG(100.0f);
	m_detectRange = CONVERT_PaperUnit_to_InG(80.0f);
	m_speed = CONVERT_cm_to_InG(7.682f);

	// Warning! �� �������ͽ� ���� �ʿ�
	m_StatusInfo.HP = m_StatusInfo.maxHP = 1200.0f;
	m_StatusInfo.Def = 20.0f;
	m_StatusInfo.Atk = 200.0f;
	m_StatusInfo.Exp = 500;

	m_attackRange = CONVERT_PaperUnit_to_InG(30);

	BuildSelf(pCreateMgr);
}

CGolem::~CGolem()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CGolem::Animate(float timeElapsed)
{
	AdjustAnimationIndex();
	AnimateByCurState();

	// ��ų ��Ÿ�� ���� ����
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
	UNREFERENCED_PARAMETER(pWayFinder);
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
			// Warning! ȸ�� ó��
			// ��� 1: ��ü ȸ��
			// ��� 2: ���� �ð����� �� %�� ü�� ȸ��
		}
	}

	if (m_nCurrAnimation == Animations::Idle ||
		m_curState == States::Walk)
	{
		CCollisionObject* enemy{ m_pColManager->RequestNearObject(this, m_detectRange) };

		if (!enemy) return;
		if (!Chaseable(enemy)) return;

		SetEnemy(enemy);

		if (Attackable(enemy))
		{
			SetState(States::Attack);
		}
		else SetState(States::Chase);
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
		// �߸��ε� ���� ��ġ�� ���ư����� ���¸� ���̵� ���·� ��ȯ�Ѵ�.
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
		GenerateSubPathToMainPath(pWayFinder);
		SetState(States::Walk);
	}
	else if (Attackable(m_pEnemy))
	{
		int usingSkill{ rand() % 2 };
		if (usingSkill == 1 && m_spAttack1CoolTime <= 0.0f)
		{
			m_nNextAnimation = Animations::SpecialAttack1;
			m_spAttack1CoolTime = COOLTIME_SPECIAL_ATTACK1;
		}
		else if (usingSkill == 1 && m_spAttack2CoolTime <= 0.0f)
		{
			m_nNextAnimation = Animations::SpecialAttack2;
			m_spAttack2CoolTime = COOLTIME_SPECIAL_ATTACK1;
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

void CGolem::ReceiveDamage(float damage)
{
	// �̹� ����� ������ ��� ������ ó���� ���� �ʴ´�.
	if (m_curState == States::Die || m_curState == States::Remove) { return; }

	m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def);
	if (m_StatusInfo.HP <= 0 && m_curState != States::Die) {
		SetState(States::Die);
	}

	if (!m_activated)
	{
		m_activated = true;
		if (m_nCurrAnimation == Animations::Sit)
		{
			m_nCurrAnimation = Animations::SitToIdle;
		}
	}
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
	SetCollisionSize(CONVERT_PaperUnit_to_InG(14));

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
// ���� �Լ�
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

	int wayKind = rand() % 2; // 0: �Ʒ�, 1: ��
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
		// ���� ������
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
		// ���� ������
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
	GenerateSubPathToPosition(pWayFinder, XMFLOAT3(pathBeg.To().x, curPos.y, pathBeg.To().y));

	SetState(StatesType::Walk);
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
}

void CGolem::GenerateSubPathToSpawnLocation(shared_ptr<CWayFinder> pWayFinder)
{
	ResetSubPath();
	m_subPath = pWayFinder->GetPathToPosition(
		GetPosition(), 
		m_spawnLocation);
	m_subDestination = m_subPath->front().To();
	m_subPath->pop_front();
	LookAt(m_subDestination);
}

bool CGolem::FarFromSpawnLocation()
{
	if (m_TeamType != TeamType::Neutral) return false;
	float dstSqr = Vector3::DistanceSquare(GetPosition(), m_spawnLocation);
	return (dstSqr > MAX_RANGE_FROM_SPAWN_GOLEM * MAX_RANGE_FROM_SPAWN_GOLEM);
}
