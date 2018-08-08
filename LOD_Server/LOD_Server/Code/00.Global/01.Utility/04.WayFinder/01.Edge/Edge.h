#pragma once

class CEdge
{
public: // 생성자, 소멸자
	CEdge(int from, int to, int cost = 10);
	~CEdge();

public: // 공개 함수
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

protected: // 내부 함수

protected: // 변수
	int m_from;	// 엣지의 시작 지점(노드의 식별 번호 저장)
	int m_to;	// 엣지의 끝 지점(노드의 식별 번호 저장)
	int m_cost;	// 해당 엣지를 지나는 데 드는 비용(상하좌우:10, 대각선: 14)
};

class CPathEdge
{
public: // 생성자, 소멸자
	CPathEdge() {}
	CPathEdge(XMFLOAT2 from, XMFLOAT2 to);
	~CPathEdge();

public: // 공개 함수
	XMFLOAT2 From() const { return m_from; }
	XMFLOAT2 To() const { return m_to; }

	void SetDestination(XMFLOAT2 newDest) { m_to = newDest; }

protected: // 내부 함수

protected: // 변수
	XMFLOAT2 m_from;	// 엣지의 시작 지점(노드의 Position 저장)
	XMFLOAT2 m_to;		// 엣지의 끝 지점(노드의 Position 저장)
};