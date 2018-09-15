#include "stdafx.h"
#include "LoadingScene.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/01.Camera/03.StaticCamera/StaticCamera.h"
#include "04.Shaders/96.SpritesShader/01.LoadingScreenShader/LoadingScreenShader.h"

/// <summary>
/// 목적: 로딩 신 처리용 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-14
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CLoadingScene::CLoadingScene()
{
}

CLoadingScene::~CLoadingScene()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수

////////////////////////////////////////////////////////////////////////
// 내부 함수
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
