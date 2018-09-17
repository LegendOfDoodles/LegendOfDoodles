#include "stdafx.h"
#include "LoadingScene.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/01.Camera/03.StaticCamera/StaticCamera.h"
#include "04.Shaders/96.SpritesShader/01.LoadingShader/01.LoadingScreenShader/LoadingScreenShader.h"
#include "04.Shaders/96.SpritesShader/01.LoadingShader/02.LoadingBarShader/LoadingBarShader.h"

/// <summary>
/// ����: �ε� �� ó���� Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-09-17
/// </summary>

#define LoadingScreenShader m_ppShaders[0]
#define LoadingBarShader m_ppShaders[1]

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CLoadingScene::CLoadingScene()
{
	m_SceneType = SceneType::LoadingScene;
}

CLoadingScene::~CLoadingScene()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�

void CLoadingScene::ApplyPercentage(float pct)
{
	LoadingBarShader->ApplyPercentage(pct);
	if (pct > 1.f) {
		CS_Msg_Demand_Game_Start p;
		p.Character_id = (BYTE)m_pNetwork->m_myid;
		p.size = sizeof(p);
		p.type = CS_GAME_LOAD;
		m_pNetwork->SendPacket(&p);
	}
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CLoadingScene::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;

	m_hWnd = pCreateMgr->GetHwnd();
	m_pCommandList = pCreateMgr->GetCommandList();

	m_pCamera = new CStaticCamera();

	m_pCamera->Initialize(pCreateMgr);

	m_nShaders = 2;
	m_ppShaders = new CShader*[m_nShaders];

	LoadingScreenShader = new CLoadingScreenShader(pCreateMgr);
	LoadingBarShader = new CLoadingBarShader(pCreateMgr);

	for (int i = 0; i < m_nShaders; ++i)
	{
		m_ppShaders[i]->Initialize(pCreateMgr);
	}
}
