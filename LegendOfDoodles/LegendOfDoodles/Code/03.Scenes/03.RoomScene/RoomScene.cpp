#include "stdafx.h"
#include "RoomScene.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/01.Camera/03.StaticCamera/StaticCamera.h"
#include "04.Shaders/96.SpritesShader/04.RoomShader/00.RoomScreenShader/RoomScreenShader.h"
#include "04.Shaders/96.SpritesShader/04.RoomShader/01.RoomButtonsShader/RoomButtonsShader.h"
#include "04.Shaders/96.SpritesShader/04.RoomShader/02.RoomCardShader/RoomCardShader.h"

/// <summary>
/// 목적: 로딩 신 처리용 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-20
/// </summary>

#define RoomScreenShader m_ppShaders[0]
#define RoomButtonsShader m_ppShaders[1]
#define RoomCardsShader m_ppShaders[2]

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CRoomScene::CRoomScene()
{
	m_SceneType = SceneType::RoomScene;
}

CRoomScene::~CRoomScene()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CRoomScene::AnimateObjects(float timeElapsed)
{
	if (m_pCamera) m_pCamera->Update(timeElapsed);

	UpdateShaderVariables();

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(timeElapsed);
	}
}

bool CRoomScene::IsSceneDone()
{
	if (RoomButtonsShader->IsShaderDone()) return true;
	return false;
}

bool CRoomScene::IsExitScene()
{
	if (RoomButtonsShader->IsShaderExit()) return true;
	return false;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CRoomScene::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;

	m_hWnd = pCreateMgr->GetHwnd();
	m_pCommandList = pCreateMgr->GetCommandList();

	m_pCamera = new CStaticCamera();

	m_pCamera->Initialize(pCreateMgr);

	m_nShaders = 3;
	m_ppShaders = new CShader*[m_nShaders];

	RoomScreenShader = new CRoomScreenShader(pCreateMgr);
	RoomButtonsShader = new CRoomButtonsShader(pCreateMgr);
	RoomCardsShader = new CRoomCardShader(pCreateMgr);

	for (int i = 0; i < m_nShaders; ++i)
	{
		m_ppShaders[i]->Initialize(pCreateMgr);
	}

	RoomButtonsShader->SetNetwork(m_pNetwork);
	RoomCardsShader->SetNetwork(m_pNetwork);
}
