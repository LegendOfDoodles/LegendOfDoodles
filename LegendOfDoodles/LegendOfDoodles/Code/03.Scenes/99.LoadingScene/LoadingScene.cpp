#include "stdafx.h"
#include "LoadingScene.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/01.Camera/03.StaticCamera/StaticCamera.h"
#include "04.Shaders/96.SpritesShader/01.LoadingScreenShader/LoadingScreenShader.h"

/// <summary>
/// ����: �ε� �� ó���� Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-09-14
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CLoadingScene::CLoadingScene()
{
}

CLoadingScene::~CLoadingScene()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CLoadingScene::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;

	m_hWnd = pCreateMgr->GetHwnd();
	m_pCommandList = pCreateMgr->GetCommandList();

	m_pCamera = new CStaticCamera();

	m_pCamera->Initialize(pCreateMgr);

	m_nShaders = 1;
	m_ppShaders = new CShader*[m_nShaders];

	m_ppShaders[0] = new CLoadingScreenShader(pCreateMgr);

	for (int i = 0; i < m_nShaders; ++i)
	{
		m_ppShaders[i]->Initialize(pCreateMgr);
	}
}
