#include "stdafx.h"
#include "Framework.h"
#include "03.Scenes/00.BaseScene/Scene.h"

/// <summary>
/// ����: ��
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-04-14
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
	m_pScene->ProcessInput();
	m_pScene->AnimateObjects(timeElapsed);
}

CScene * CFramework::GetScene()
{
	return m_pScene;
}

LRESULT CALLBACK CFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
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
	{
		m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);

		break;
	}
	}
	return(0);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CFramework::BuildObjects()
{
	m_pScene = new CScene();
	m_pScene->Initialize();
}

void CFramework::ReleaseObjects()
{
	if (!m_pScene) return;

	m_pScene->Finalize();
	Safe_Delete(m_pScene);
}