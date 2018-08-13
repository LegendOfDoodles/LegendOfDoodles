#include "stdafx.h"
#include "EffectManager.h"
#include "04.Shaders\09.EffectShader\EffectShader.h"

/// <summary>
/// 목적: Effect 생성을 요청을 위한 클래스
/// 최종 수정자:  이용선
/// 수정자 목록:  이용선
/// 최종 수정 날짜: 2018-08-06
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CEffectMgr::CEffectMgr()
{
}

CEffectMgr::~CEffectMgr()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CEffectMgr::RequestSpawn(const XMFLOAT3 & position, const XMFLOAT3 & direction, int aniLength, EffectObjectType objectType)
{
	m_pEffectShader->SpawnEffectObject(position, direction, aniLength, objectType);
}

void CEffectMgr::RequestPause()
{
	m_pEffectShader->Pause();
}

void CEffectMgr::RequestResume()
{
	m_pEffectShader->Resume();
}
