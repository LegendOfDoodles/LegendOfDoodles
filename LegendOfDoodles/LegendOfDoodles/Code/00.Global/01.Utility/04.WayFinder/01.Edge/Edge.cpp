#include "stdafx.h"
#include "Edge.h"

/// <summary>
/// 목적: 길 찾기 알고리즘에서 사용할 엣지 정의
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-30
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CEdge::CEdge(int from, int to, int cost)
	: m_from(from), m_to(to), m_cost(cost)
{
}

CEdge::~CEdge()
{
}

////////////////////////////////////////////////////////////////////////
// PathEdge
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CPathEdge::CPathEdge(XMFLOAT2 from, XMFLOAT2 to)
	: m_from(from), m_to(to)
{
}

CPathEdge::~CPathEdge()
{
}