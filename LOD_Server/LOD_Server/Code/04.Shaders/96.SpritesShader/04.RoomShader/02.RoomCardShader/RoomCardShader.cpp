#include "stdafx.h"
#include "RoomCardShader.h"

/// <summary>
/// 목적: 로딩 바 출력용 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-18
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
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
// 공개 함수
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
// 내부 함수
