#include "stdafx.h"
#include "PlayerAI.h"

/// <summary>
/// 목적: 플레이어 AI 관리 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-10-04
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CPlayerAI::CPlayerAI(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CPlayer(pCreateMgr, nMeshes)
{
}

CPlayerAI::~CPlayerAI()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CPlayerAI::Animate(float timeElapsed)
{
	CPlayer::Animate(timeElapsed);
	m_refreshTimeChecker += timeElapsed;

	// 사망한 상태인 경우 다음 행동을 계산할 필요가 없으므로
	if (m_curState == States::Die || m_curState == States::Remove) return;

	PlayAction(m_currentActionIndex, timeElapsed);
}

void CPlayerAI::PrepareData()
{
	XMFLOAT3 curPos{ GetPosition() };
	if (curPos.z > TERRAIN_SIZE_HEIGHT * 0.5f)
	{
		m_ownLine = PlayerLine::TopLine;
		m_otherLine = PlayerLine::BottomLine;
	}
	else
	{
		m_ownLine = PlayerLine::BottomLine;
		m_otherLine = PlayerLine::TopLine;
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CPlayerAI::PlayAction(int index, float timeElapsed)
{
	switch (index) {
	case Push_Line:
		PushLine(timeElapsed);
		break;
	case Attack_Player:
		AttackPlayer(timeElapsed);
		break;
	case Fall_Back:
		FallBack(timeElapsed);
		break;
	case Support_Line:
		SupportLine(timeElapsed);
		break;
	}
}

void CPlayerAI::PushLine(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);
}

void CPlayerAI::AttackPlayer(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);
}

void CPlayerAI::FallBack(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);
}

void CPlayerAI::SupportLine(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);
}
