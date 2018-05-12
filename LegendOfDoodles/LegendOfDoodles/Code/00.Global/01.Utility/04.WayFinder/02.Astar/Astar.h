#pragma once
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

template<class KeyType>
class IndexedPriorityQLow
{
public:	// 생성자, 소멸자
	IndexedPriorityQLow(std::vector<KeyType>& keys, int maxSize);

public: // 공개 함수
	void insert(const int idx);
	int Pop();

	void ChangePriority(const int idx);

	bool empty()const { return (m_size == 0); }

protected: // 내부 함수
	void Swap(int a, int b);

	void ReorderUpwards(int nd);
	void ReorderDownwards(int nd, int HeapSize);

private: // 변수
	std::vector<KeyType>&  m_vecKeys;
	std::vector<int> m_Heap;
	std::vector<int> m_invHeap;
	int m_size;
	int m_maxSize;
};

class CAstar
{
public: // 생성자, 소멸자
	CAstar(const CWayFinder *finder, int source, int target);
	~CAstar();

public: // 공개 함수
	int FindPath();
	Path *GetPath();

protected: // 내부 함수
	double Heuristic_Menhattan(int from, int to);
	double Heuristic_Diagonal(int from, int to);
	double Heuristic_Euclidean(int from, int to);
	double Heuristic_EuclideanSquare(int from, int to);
	double Heuristic_NoisyEuclidean(int from, int to);
	double Heuristic_Dijkstra(int from, int to);

protected: // 변수
	const CWayFinder *m_pFinder;

	int m_source;	// 길찾기 시작 노드
	int m_target;	// 길찾기 목표 노드

	std::vector<double>            m_GCosts;	// 이동 비용
	std::vector<double>            m_FCosts;	//	F = G + H(예상 비용)

	std::vector<const CEdge*>       m_shortestPathTree;	// 목표 -> 시작 위치 트리
	std::vector<const CEdge*>       m_searchFrontier;	// 가능한 노드 모음

	IndexedPriorityQLow<double>*    m_pPQ;		// F가 작은 것 부터 꺼내기 위한 용도
};