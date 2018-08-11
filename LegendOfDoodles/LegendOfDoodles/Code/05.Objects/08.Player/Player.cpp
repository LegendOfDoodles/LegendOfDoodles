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
CPlayer::CPlayer(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CAnimatedObject(pCreateMgr, nMeshes)
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
		if (m_nCurrAnimation != Animations::Die) m_nCurrAnimation = Animations::Die;
		if (GetAnimTimeRemainRatio() < 0.05)
		{
			m_curState = States::Remove;
		}
		break;
	case States::Defeat:
		if (m_nCurrAnimation == Animations::Defeat) {
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] - 1)
			{
				m_nCurrAnimation = Animations::Defeat2;
				m_fFrameTime = 0;
			}
		}
		break;
	default:
		break;
	}

	m_fPreFrameTime = m_fFrameTime;
	m_fFrameTime += 30 * timeElapsed;

	AdjustAnimationIndex();

	if (m_fFrameTime > m_nAniLength[m_nAniIndex]) {
		while (m_fFrameTime > m_nAniLength[m_nAniIndex])
			m_fFrameTime -= m_nAniLength[m_nAniIndex];
	}

	if(MoveToDestination(timeElapsed) == States::Done) SetState(States::Idle);

	CAnimatedObject::Animate(timeElapsed);
}

void CPlayer::Render(CCamera * pCamera, UINT instanceCnt)
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

void CPlayer::SetState(StatesType newState, shared_ptr<CWayFinder> pWayFinder)
{
	UNREFERENCED_PARAMETER(pWayFinder);
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
	for (int j = 0; j < 7; j++)
	{
		m_nAniLength[j + 3] = ppskill[j]->GetAnimationLength();
		m_pSkeleton[j + 3] = *ppskill[j];
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
	case Animations::Idle:
		m_nAniIndex = 3;
		break;
	case Animations::StartWalk:
		m_nAniIndex = 4;
		break;
	case Animations::Walking:
		m_nAniIndex = 5;
		break;
	case Animations::SkillQ:
		m_nAniIndex = 6;
		break;
	case Animations::SkillW:
		m_nAniIndex = 7;
		break;
	case Animations::SkillE:
		m_nAniIndex = 8;
		break;
	case Animations::SkillR:
		m_nAniIndex = 9;
		break;
	}
}
