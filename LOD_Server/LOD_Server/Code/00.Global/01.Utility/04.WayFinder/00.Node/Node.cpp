#include "stdafx.h"
#include "Node.h"

/// <summary>
/// ����: ��ã�� �˰��򿡼� ����� ��� ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-04-20
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CNode::CNode(int index, XMFLOAT2 &position, XMFLOAT2 &size)
	: m_index(index), m_position(position), m_size(size)
{
}

CNode::~CNode()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
const POINT CNode::GetDividIndex() const
{
	POINT dividIndex;

	dividIndex.x = m_position.x / m_size.x;
	dividIndex.y = m_position.y / m_size.y;

	return dividIndex;
}

int CNode::GetDistanceSquareWithPosition(const XMFLOAT2 &position)
{
	return  (position.x - m_position.x) * (position.x - m_position.x) + (position.y - m_position.y) * (position.y - m_position.y);
}


////////////////////////////////////////////////////////////////////////
// ���� �Լ�
int CNode::PositionToIndex(XMFLOAT2 position)
{
	int x = position.x / m_size.x;
	int y = position.y / m_size.y;
	int xMax = TERRAIN_SIZE_WIDTH / m_size.x + 1;

	return  xMax * y + x;
}
