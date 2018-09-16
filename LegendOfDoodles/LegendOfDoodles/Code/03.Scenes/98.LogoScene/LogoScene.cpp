#include "stdafx.h"
#include "LogoScene.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/01.Camera/03.StaticCamera/StaticCamera.h"
#include "04.Shaders/96.SpritesShader/03.LogoShader/LogoShader.h"

/// <summary>
/// ����: �ΰ� �� ó���� Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-09-16
/// </summary>

#define LogoScreenShader m_ppShaders[0]

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CLogoScene::CLogoScene()
{
	m_SceneType = SceneType::LogoScene;
}

CLogoScene::~CLogoScene()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
bool CLogoScene::IsSceneDone()
{
	return LogoScreenShader->IsShaderDone();
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CLogoScene::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;

	m_hWnd = pCreateMgr->GetHwnd();
	m_pCommandList = pCreateMgr->GetCommandList();

	m_pCamera = new CStaticCamera();

	m_pCamera->Initialize(pCreateMgr);

	m_nShaders = 1;
	m_ppShaders = new CShader*[m_nShaders];

	LogoScreenShader = new CLogoShader(pCreateMgr);

	for (int i = 0; i < m_nShaders; ++i)
	{
		m_ppShaders[i]->Initialize(pCreateMgr);
	}
}
