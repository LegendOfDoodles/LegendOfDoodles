#include "stdafx.h"
#include "RoomCardShader.h"

/// <summary>
/// ����: �ε� �� ��¿� ���̴�
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-09-18
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CRoomCardShader::CRoomCardShader() : CShader()
{
	m_EachCardType[0] = CardType::Blue_AI;
	m_EachCardType[1] = CardType::Blue_AI;
	m_EachCardType[2] = CardType::Red_AI;
	m_EachCardType[3] = CardType::Red_AI;
}

CRoomCardShader::~CRoomCardShader()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CRoomCardShader::SetPlayerByIndex(int index)
{
	if (m_EachCardType[index] == CardType::Blue_AI)
	{
		m_EachCardType[index] = CardType::Blue_Player;
	}
	else if (m_EachCardType[index] == CardType::Red_AI)
	{
		m_EachCardType[index] = CardType::Red_Player;
	}
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
