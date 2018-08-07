#include "stdafx.h"
#include "Framework.h"

/// <summary>
/// 목적: 프레임워크 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-05
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
	m_pScene->AnimateObjects(timeElapsed);
	// Warning! 임시 종료 확인 -> 향우 변경 필요
	m_running = !(GetAsyncKeyState(VK_ESCAPE) & 0x8000);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CFramework::BuildObjects()
{
	m_pScene = shared_ptr<CScene>(new CScene());
	m_pScene->Initialize();
}

void CFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->Finalize();
	}
}