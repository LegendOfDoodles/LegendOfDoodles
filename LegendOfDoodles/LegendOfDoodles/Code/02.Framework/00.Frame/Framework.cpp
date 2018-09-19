#include "stdafx.h"
#include "Framework.h"
#include "03.Scenes/01.GameScene/GameScene.h"
#include "03.Scenes/99.LoadingScene/LoadingScene.h"
#include "03.Scenes/98.LogoScene/LogoScene.h"
#include "03.Scenes/02.TitleScene/TitleScene.h"
#include "03.Scenes/03.RoomScene/RoomScene.h"

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
bool CFramework::Initialize(HINSTANCE hInstance, HWND hWnd, shared_ptr<CNetwork> pNetwork)
{
	m_hWnd = hWnd;
	
	m_pCreateMgr = shared_ptr<CCreateMgr>(new CCreateMgr);

	m_pCreateMgr->Initialize(hInstance, hWnd);
	m_pRenderMgr = m_pCreateMgr->GetRenderMgr();

	m_pNetwork = pNetwork;

	return(true);
}

void CFramework::Finalize()
{
	ReleaseObjects();
	m_pCreateMgr->Release();
}

void CFramework::FrameAdvance(float timeElapsed)
{
	m_pScene->ProcessInput();
	m_pScene->AnimateObjects(timeElapsed);
	m_pRenderMgr->Render(m_pScene);

	ChangeDoneScene();
}

void CFramework::BuildObjects()
{
	m_pLoadingScene = shared_ptr<CLoadingScene>(new CLoadingScene());
	m_pLoadingScene->Initialize(m_pCreateMgr, m_pNetwork);
	m_pLoadingScene->ReleaseUploadBuffers();
	m_pRenderMgr->SetLoadingScene(m_pLoadingScene);

	ChangeSceneByType(SceneType::LogoScene);
}

LRESULT CALLBACK CFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		m_pCreateMgr->Resize(LOWORD(lParam), HIWORD(lParam));
		// m_createMgr.ChangeScreenMode();
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		if(m_pScene) m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		if(m_pScene) m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->Finalize();
	if (m_pLoadingScene) m_pLoadingScene->Finalize();
}

void CFramework::ChangeDoneScene()
{
	// 씬 변경 처리
	if (m_pScene->IsSceneDone())
	{
		if (m_pScene->GetCurSceneType() == SceneType::LogoScene)
		{
			ChangeSceneByType(SceneType::TitleScene);
		}
		else if (m_pScene->GetCurSceneType() == SceneType::TitleScene)
		{
			if (m_pScene->IsExitScene())
			{
				m_running = false;
			}
			else
			{
				ChangeSceneByType(SceneType::RoomScene);
			}
		}
		else if (m_pScene->GetCurSceneType() == SceneType::RoomScene)
		{
			if (m_pScene->IsExitScene())
			{
				ChangeSceneByType(SceneType::TitleScene);
				m_pNetwork->Finalize();
			}
			else
			{
				ChangeSceneByType(SceneType::GameScene);
			}
		}
		else if (m_pScene->GetCurSceneType() == SceneType::GameScene)
		{
			
		}
	}
}

void CFramework::ChangeSceneByType(SceneType type)
{
	if (m_pScene) m_pScene->Finalize();

	if (type == SceneType::LogoScene)
	{
		m_pScene = shared_ptr<CLogoScene>(new CLogoScene());
	}
	else if (type == SceneType::TitleScene)
	{
		m_pScene = shared_ptr<CTitleScene>(new CTitleScene());
	}
	else if (type == SceneType::RoomScene)
	{
		m_pNetwork->Initialize(m_hWnd);
		m_pScene = shared_ptr<CRoomScene>(new CRoomScene());
		m_pNetwork->SetRoomScene(m_pScene);
	}
	else if (type == SceneType::GameScene)
	{
		m_pScene = shared_ptr<CGameScene>(new CGameScene());
	}

	m_pScene->Initialize(m_pCreateMgr, m_pNetwork);

	m_pScene->ReleaseUploadBuffers();
}
