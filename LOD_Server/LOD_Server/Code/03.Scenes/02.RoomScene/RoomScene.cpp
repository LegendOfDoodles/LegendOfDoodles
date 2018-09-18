#include "stdafx.h"
#include "RoomScene.h"
#include "04.Shaders/96.SpritesShader/04.RoomShader/02.RoomCardShader/RoomCardShader.h"

/// <summary>
/// 목적: 로딩 신 처리용 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-18
/// </summary>

#define RoomCardsShader m_ppShaders[0]

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CRoomScene::CRoomScene()
{
}

CRoomScene::~CRoomScene()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CRoomScene::ApplyPlayerIndex(int index)
{
	RoomCardsShader->SetPlayerByIndex(index);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CRoomScene::BuildObjects()
{
	m_nShaders = 1;
	m_ppShaders = new CShader*[m_nShaders];
	
	RoomCardsShader = new CRoomCardShader();

	for (int i = 0; i < m_nShaders; ++i)
	{
		m_ppShaders[i]->Initialize();
	}
}
