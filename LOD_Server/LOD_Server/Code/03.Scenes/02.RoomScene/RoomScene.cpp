#include "stdafx.h"
#include "RoomScene.h"
#include "04.Shaders/96.SpritesShader/04.RoomShader/02.RoomCardShader/RoomCardShader.h"

/// <summary>
/// ����: �ε� �� ó���� Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-09-18
/// </summary>

#define RoomCardsShader m_ppShaders[0]

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CRoomScene::CRoomScene()
{
}

CRoomScene::~CRoomScene()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CRoomScene::ApplyPlayerIndex(int index)
{
	RoomCardsShader->SetPlayerByIndex(index);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
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
