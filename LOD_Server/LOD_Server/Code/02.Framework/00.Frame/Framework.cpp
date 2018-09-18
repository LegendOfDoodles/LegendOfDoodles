#include "stdafx.h"
#include "Framework.h"
#include "03.Scenes/01.GameScene/GameScene.h"

/// <summary>
/// 목적: 프레임워크 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-18
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CFramework::CFramework()
{
}

CFramework::~CFramework()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
bool CFramework::Initialize()
{
	BuildObjects();

	return(true);
}



void CFramework::Finalize()
{
	ReleaseObjects();
}

void CFramework::FrameAdvance(float timeElapsed)
{
	g_GameTime += timeElapsed;
	m_pScene->AnimateObjects(timeElapsed);
}



////////////////////////////////////////////////////////////////////////
// 내부 함수
void CFramework::BuildObjects()
{
	m_pScene = shared_ptr<CGameScene>(new CGameScene());
	m_pScene->Initialize();
}

void CFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->Finalize();
	}
}