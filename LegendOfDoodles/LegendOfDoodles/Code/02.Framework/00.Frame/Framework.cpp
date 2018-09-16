#include "stdafx.h"
#include "Framework.h"
#include "03.Scenes/01.GameScene/GameScene.h"
#include "03.Scenes/99.LoadingScene/LoadingScene.h"

/// <summary>
/// ����: �����ӿ�ũ Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-09-15
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
	// Warning! �ӽ� ���� Ȯ�� -> ��� ���� �ʿ�
	m_running = !(GetAsyncKeyState(VK_ESCAPE) & 0x8000);
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

void CFramework::BuildObjects()
{
	m_pLoadingScene = shared_ptr<CLoadingScene>(new CLoadingScene());
	m_pLoadingScene->Initialize(m_pCreateMgr, m_pNetwork);
	m_pLoadingScene->ReleaseUploadBuffers();
	m_pRenderMgr->SetLoadingScene(m_pLoadingScene);
	m_pRenderMgr->RenderLoadingScreen();

	m_pScene = shared_ptr<CGameScene>(new CGameScene());
	m_pScene->Initialize(m_pCreateMgr, m_pNetwork);

	m_pScene->ReleaseUploadBuffers();
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->Finalize();
	if (m_pLoadingScene) m_pLoadingScene->Finalize();
}