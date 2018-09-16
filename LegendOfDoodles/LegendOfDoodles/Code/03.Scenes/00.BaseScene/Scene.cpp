#include "stdafx.h"
#include "Scene.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/01.Camera/01.AOSCamera/AOSCamera.h"
#include "00.Global/01.Utility/09.SoundManager/SoundManager.h"

/// <summary>
/// 목적: 기본 씬, 인터페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-15
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CScene::CScene()
{
}

CScene::~CScene()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CScene::Initialize(shared_ptr<CCreateMgr> pCreateMgr, shared_ptr<CNetwork> pNetwork)
{
	m_pNetwork = pNetwork;
	BuildObjects(pCreateMgr);
	CreateShaderVariables(pCreateMgr);
}

void CScene::Finalize()
{
	ReleaseObjects();
	ReleaseShaderVariables();
}

void CScene::ReleaseUploadBuffers()
{
	if (m_ppShaders)
	{
		for (int j = 0; j < m_nShaders; j++)
		{
			if (m_ppShaders[j])
			{
				m_ppShaders[j]->ReleaseUploadBuffers();
			}
		}
	}
}

void CScene::ProcessInput()
{
	UCHAR pKeyBuffer[256];

	GetKeyboardState(pKeyBuffer);

	bool continual = m_pCamera->OnProcessMouseInput(pKeyBuffer);
	if (continual) continual = m_pCamera->OnProcessKeyInput(pKeyBuffer);
	for (int i = 0; i < m_nShaders; ++i) {
		if (continual)
			continual = m_ppShaders[i]->OnProcessKeyInput(pKeyBuffer);
	}
}

void CScene::AnimateObjects(float timeElapsed)
{
	if (m_pCamera) m_pCamera->Update(timeElapsed);
	if (m_pSoundManager) m_pSoundManager->Update(timeElapsed);

	UpdateShaderVariables();

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(timeElapsed);
	}
}

void CScene::Render()
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(m_pCamera);
	}
}

void CScene::RenderWithLights()
{
	UpdateShaderVariables();
	if (m_pCamera) m_pCamera->UpdateShaderVariables(1);
}

void CScene::SetViewportsAndScissorRects()
{
	if (m_pCamera) m_pCamera->SetViewportsAndScissorRects();
}

void CScene::UpdateCamera()
{
	if (m_pCamera) m_pCamera->UpdateShaderVariables();
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		if (m_pCamera) m_pCamera->SavePickedPos();
		for (int i = 0; i < m_nShaders; ++i)
			m_ppShaders[i]->OnProcessMouseInput(wParam);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEWHEEL:
		if (m_pCamera) m_pCamera->OnProcessMouseWheel(wParam, lParam);
		break;
	default:
		break;
	}
}

void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(hWnd);
	if (nMessageID == WM_KEYUP)
	{
		OnProcessKeyUp(wParam, lParam);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CScene::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;

	m_hWnd = pCreateMgr->GetHwnd();
	m_pCommandList = pCreateMgr->GetCommandList();

	m_pCamera = new CAOSCamera();

	m_pCamera->Initialize(pCreateMgr);

	m_pSoundManager = shared_ptr<CSoundManager>(new CSoundManager(m_pCamera));	// Fmod System Init
	m_pSoundManager->loading();		// Sound File Load
	
	// BackGround Music
	m_pSoundManager->play(SOUND::Back_Ground, XMFLOAT3(0, 0, 0));
}

void CScene::ReleaseObjects()
{
	if (m_pCamera)
	{
		m_pCamera->Finalize();
		Safe_Delete(m_pCamera);
	}
	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			if (m_ppShaders[i]) m_ppShaders[i]->Finalize();
		}
		Safe_Delete_Array(m_ppShaders);
	}
}
