#include "stdafx.h"
#include "TitleScene.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/01.Camera/03.StaticCamera/StaticCamera.h"
#include "04.Shaders/96.SpritesShader/03.TitleShader/00.TitleScreenShader/TitleScreenShader.h"
#include "04.Shaders/96.SpritesShader/03.TitleShader/01.TitleButtonsShader/TitleButtonsShader.h"

/// <summary>
/// ����: �ε� �� ó���� Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-09-17
/// </summary>

#define TitleScreenShader m_ppShaders[0]
#define TitleButtonsShader m_ppShaders[1]

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CTitleScene::CTitleScene()
{
	m_SceneType = SceneType::TitleScene;
}

CTitleScene::~CTitleScene()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CTitleScene::AnimateObjects(float timeElapsed)
{
	if (m_pCamera) m_pCamera->Update(timeElapsed);

	UpdateShaderVariables();

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(timeElapsed);
	}
}

bool CTitleScene::IsSceneDone()
{
	if (TitleButtonsShader->IsShaderDone()) return true;
	return false;
}

bool CTitleScene::IsExitScene()
{
	if (TitleButtonsShader->IsShaderExit()) return true;
	return false;
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CTitleScene::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;

	m_hWnd = pCreateMgr->GetHwnd();
	m_pCommandList = pCreateMgr->GetCommandList();

	m_pCamera = new CStaticCamera();

	m_pCamera->Initialize(pCreateMgr);

	m_nShaders = 2;
	m_ppShaders = new CShader*[m_nShaders];

	TitleScreenShader = new CTitleScreenShader(pCreateMgr);
	TitleButtonsShader = new CTitleButtonsShader(pCreateMgr);

	for (int i = 0; i < m_nShaders; ++i)
	{
		m_ppShaders[i]->Initialize(pCreateMgr);
	}
}
