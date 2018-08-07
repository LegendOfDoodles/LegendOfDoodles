#include "stdafx.h"
#include "ThrowingMgr.h"
#include "04.Shaders/08.FlyingShader/FlyingShader.h"

/// <summary>
/// 목적: 날아가는 오브젝트(화살, 아령 등)의 생성 요청 관리를 위한 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-07-27
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CThrowingMgr::CThrowingMgr()
{
}

CThrowingMgr::~CThrowingMgr()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CThrowingMgr::RequestSpawn(const XMFLOAT3 & position, const float positionOffset, const XMFLOAT3 & direction, TeamType teamType, FlyingObjectType objectType)
{
	m_pFlyingShader->SpawnFlyingObject(position, positionOffset, direction, teamType, objectType);
}

void CThrowingMgr::RequestPause()
{
	m_pFlyingShader->Pause();
}

void CThrowingMgr::RequestResume()
{
	m_pFlyingShader->Resume();
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
