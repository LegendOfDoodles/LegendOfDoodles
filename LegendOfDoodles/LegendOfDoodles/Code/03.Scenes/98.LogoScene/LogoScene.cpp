#include "stdafx.h"
#include "LogoScene.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/01.Camera/03.StaticCamera/StaticCamera.h"
#include "04.Shaders/96.SpritesShader/03.LogoShader/LogoShader.h"

/// <summary>
/// 목적: 로고 씬 처리용 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-16
/// </summary>

#define LogoScreenShader m_ppShaders[0]

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CLogoScene::CLogoScene()
{
	m_SceneType = SceneType::LogoScene;
}

CLogoScene::~CLogoScene()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
bool CLogoScene::IsSceneDone()
{
	return LogoScreenShader->IsShaderDone();
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
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
