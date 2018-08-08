#pragma once

class CEdge
{
public: // ������, �Ҹ���
	CEdge(int from, int to, int cost = 10);
	~CEdge();

public: // ���� �Լ�
	int From() { return m_from; }
	int To() { return m_to; }
	int Cost() { return m_cost; }

	const int From() const { return m_from; }
	const int To() const { return m_to; }
	const int Cost() const { return m_cost; }

	bool operator==(const CEdge& other)
	{
		if (m_from != other.m_from) return false;
		if (m_to != other.m_to) return false;
		if (m_cost != other.m_cost) return false;
		return true;
	}

	bool operator!=(const CEdge& other)
	{
		return !(*this == other);
	}

protected: // ���� �Լ�

protected: // ����
	int m_from;	// ������ ���� ����(����� �ĺ� ��ȣ ����)
	int m_to;	// ������ �� ����(����� �ĺ� ��ȣ ����)
	int m_cost;	// �ش� ������ ������ �� ��� ���(�����¿�:10, �밢��: 14)
};

class CPathEdge
{
public: // ������, �Ҹ���
	CPathEdge() {}
	CPathEdge(XMFLOAT2 from, XMFLOAT2 to);
	~CPathEdge();

public: // ���� �Լ�
	XMFLOAT2 From() const { return m_from; }
	XMFLOAT2 To() const { return m_to; }

	void SetDestination(XMFLOAT2 newDest) { m_to = newDest; }

protected: // ���� �Լ�

protected: // ����
	XMFLOAT2 m_from;	// ������ ���� ����(����� Position ����)
	XMFLOAT2 m_to;		// ������ �� ����(����� Position ����)
};