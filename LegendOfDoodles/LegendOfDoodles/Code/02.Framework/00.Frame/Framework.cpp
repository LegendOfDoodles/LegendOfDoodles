#include "stdafx.h"
#include "Framework.h"
#include "03.Scenes/01.GameScene/GameScene.h"
#include "03.Scenes/99.LoadingScene/LoadingScene.h"
#include "03.Scenes/98.LogoScene/LogoScene.h"

/// <summary>
/// ����: �����ӿ�ũ Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-09-16
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CFramework::CFramework()
{
}

CFramework::~CFramework()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
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

	// Warning! �ӽ� ���� Ȯ�� -> ���� ���� �ʿ�
	m_running = !(GetAsyncKeyState(VK_ESCAPE) & 0x8000);
}

void CFramework::BuildObjects()
{
	m_pLoadingScene = shared_ptr<CLoadingScene>(new CLoadingScene());
	m_pLoadingScene->Initialize(m_pCreateMgr, m_pNetwork);
	m_pLoadingScene->ReleaseUploadBuffers();
	m_pRenderMgr->SetLoadingScene(m_pLoadingScene);

	m_pScene = shared_ptr<CLogoScene>(new CLogoScene());
	m_pScene->Initialize(m_pCreateMgr, m_pNetwork);

	m_pScene->ReleaseUploadBuffers();
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
		m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->Finalize();
	if (m_pLoadingScene) m_pLoadingScene->Finalize();
}

void CFramework::ChangeDoneScene()
{
	// �� ���� ó��
	if (m_pScene->IsSceneDone())
	{
		if (m_pScene->GetCurSceneType() == SceneType::LogoScene)
		{
			if (m_pScene) m_pScene->Finalize();
			m_pScene = shared_ptr<CGameScene>(new CGameScene());
			m_pScene->Initialize(m_pCreateMgr, m_pNetwork);

			m_pScene->ReleaseUploadBuffers();
		}
		else if (m_pScene->GetCurSceneType() == SceneType::GameScene)
		{
			
		}
	}
}
