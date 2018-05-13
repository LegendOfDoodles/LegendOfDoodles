#pragma once
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

template<class KeyType>
class IndexedPriorityQLow
{
public:	// ������, �Ҹ���
	IndexedPriorityQLow(std::vector<KeyType>& keys, int maxSize);

public: // ���� �Լ�
	void insert(const int idx);
	int Pop();

	void ChangePriority(const int idx);

	bool empty()const { return (m_size == 0); }

protected: // ���� �Լ�
	void Swap(int a, int b);

	void ReorderUpwards(int nd);
	void ReorderDownwards(int nd, int HeapSize);

private: // ����
	std::vector<KeyType>&  m_vecKeys;
	std::vector<int> m_Heap;
	std::vector<int> m_invHeap;
	int m_size;
	int m_maxSize;
};

class CAstar
{
public: // ������, �Ҹ���
	CAstar(const CWayFinder *finder, int source, int target);
	~CAstar();

public: // ���� �Լ�
	int FindPath();
	Path *GetPath();

protected: // ���� �Լ�
	double Heuristic_Menhattan(int from, int to);
	double Heuristic_Diagonal(int from, int to);
	double Heuristic_Euclidean(int from, int to);
	double Heuristic_EuclideanSquare(int from, int to);
	double Heuristic_NoisyEuclidean(int from, int to);
	double Heuristic_Dijkstra(int from, int to);

protected: // ����
	const CWayFinder *m_pFinder;

	int m_source;	// ��ã�� ���� ���
	int m_target;	// ��ã�� ��ǥ ���

	std::vector<double>            m_GCosts;	// �̵� ���
	std::vector<double>            m_FCosts;	//	F = G + H(���� ���)

	std::vector<const CEdge*>       m_shortestPathTree;	// ��ǥ -> ���� ��ġ Ʈ��
	std::vector<const CEdge*>       m_searchFrontier;	// ������ ��� ����

	IndexedPriorityQLow<double>*    m_pPQ;		// F�� ���� �� ���� ������ ���� �뵵
};