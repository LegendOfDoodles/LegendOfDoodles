#include "stdafx.h"
#include "Edge.h"

/// <summary>
/// ����: �� ã�� �˰��򿡼� ����� ���� ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-04-30
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
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
// ������, �Ҹ���
CPathEdge::CPathEdge(XMFLOAT2 from, XMFLOAT2 to)
	: m_from(from), m_to(to)
{
}

CPathEdge::~CPathEdge()
{
}