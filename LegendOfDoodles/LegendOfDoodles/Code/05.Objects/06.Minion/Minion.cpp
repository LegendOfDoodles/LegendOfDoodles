#include "stdafx.h"
#include "Minion.h"

/// <summary>
/// 목적: 미니언 클래스 분할
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-08-07
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CMinion::CMinion(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CAnimatedObject(pCreateMgr, nMeshes)
{
	m_sightRange = CONVERT_PaperUnit_to_InG(80.0f);
	m_detectRange = CONVERT_PaperUnit_to_InG(40.0f);
	m_speed = CONVERT_cm_to_InG(1.805f);
}

CMinion::~CMinion()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CMinion::Animate(float timeElapsed)
{
	AdjustAnimationIndex();

	m_fPreFrameTime = m_fFrameTime;
	m_fFrameTime += 30 * timeElapsed;

	if (m_fFrameTime > m_nAniLength[m_nAniIndex]) {
		while (m_fFrameTime > m_nAniLength[m_nAniIndex])
			m_fFrameTime -= m_nAniLength[m_nAniIndex];
	}

	CAnimatedObject::Animate(timeElapsed);
}

void CMinion::Render(CCamera * pCamera, UINT instanceCnt)
{
	OnPrepareRender();

	if (!IsVisible(pCamera) || !m_Detected) return;

	if (m_pMaterial)
	{
		m_pMaterial->Render(pCamera);
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

void CMinion::SetState(StatesType newState)
{
	m_nextState = m_curState = newState;

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

	//CCollisionObject* enemy{ m_pColManager->RequestNearObject(this, m_detectRange) };

	//if (!enemy) return;
	//if (!Chaseable(enemy)) return;

	//SetEnemy(enemy);

	//if (Attackable(enemy)) SetState(States::Attack);
	//else SetState(States::Chase);
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
	//if (!Chaseable(m_pEnemy))
	//{
	//	SetEnemy(NULL);
	//	GenerateSubPathToMainPath(pWayFinder);
	//	SetState(States::Walk);
	//}
	//else
	//{
	//	MoveToSubDestination(timeElapsed, pWayFinder);
	//}

	//if (Attackable(m_pEnemy)) SetState(States::Attack);
}

void CMinion::PlayAttack(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	UNREFERENCED_PARAMETER(timeElapsed);

	//if (!CheckEnemyState(m_pEnemy))
	//{
	//	SetEnemy(NULL);
	//	GenerateSubPathToMainPath(pWayFinder);
	//	SetState(States::Walk);
	//}
	//else if (!Attackable(m_pEnemy))
	//{
	//	SetNextState(States::Chase);
	//}
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
CSwordMinion::CSwordMinion(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CMinion(pCreateMgr, nMeshes)
{
	SetType(ObjectType::SwordMinion);
	m_StatusInfo.maxHP = 445;
	m_StatusInfo.HP = 445;
	m_StatusInfo.Atk = 100;
	m_StatusInfo.Def = 0;
	m_StatusInfo.Exp = 64;

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
			//LookAt(m_pEnemy->GetPosition());
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
CMagicMinion::CMagicMinion(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CMinion(pCreateMgr, nMeshes)
{
	SetType(ObjectType::StaffMinion);
	m_StatusInfo.maxHP = 280;
	m_StatusInfo.HP = 280;
	m_StatusInfo.Atk = 23;
	m_StatusInfo.Def = 0;
	m_StatusInfo.Exp = 64;

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
			//LookAt(m_pEnemy->GetPosition());
		}
		if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f
			&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) {
			m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Minion_Magic);
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
CBowMinion::CBowMinion(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CMinion(pCreateMgr, nMeshes)
{
	SetType(ObjectType::BowMinion);
	m_StatusInfo.maxHP = 300;
	m_StatusInfo.HP = 300;
	m_StatusInfo.Atk = 20;
	m_StatusInfo.Def = 0;
	m_StatusInfo.Exp = 64;

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
			//LookAt(m_pEnemy->GetPosition());
		}
		if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f
			&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) {
			m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Minion_Arrow);
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
