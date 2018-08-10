#include "stdafx.h"
#include "Framework.h"

/// <summary>
/// ����: �����ӿ�ũ Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-08-05
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

	BuildObjects();



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

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CFramework::BuildObjects()
{
	m_pRenderMgr->ResetCommandList();

	m_pScene = shared_ptr<CScene>(new CScene());
	m_pScene->Initialize(m_pCreateMgr, m_pNetwork);

	m_pRenderMgr->ExecuteCommandList();

	m_pScene->ReleaseUploadBuffers();
}

void CFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->Finalize();
	}
}