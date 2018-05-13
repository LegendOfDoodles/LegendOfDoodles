#include "stdafx.h"
#include "Astar.h"

/// <summary>
/// 목적: 길찾기 용 Astar 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-20
/// </summary>

////////////////////////////////////////////////////////////////////////
// IndexedPriorityQLow
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
template<class KeyType>
IndexedPriorityQLow<KeyType>::IndexedPriorityQLow(std::vector<KeyType>& keys, int maxSize)
	:m_vecKeys(keys), m_maxSize(maxSize), m_size(0)
{
	m_Heap.assign(maxSize + 1, 0);
	m_invHeap.assign(maxSize + 1, 0);
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
template<class KeyType>
void IndexedPriorityQLow<KeyType>::insert(const int idx)
{
	assert(m_size + 1 <= m_maxSize);

	++m_size;

	m_Heap[m_size] = idx;

	m_invHeap[idx] = m_size;

	ReorderUpwards(m_size);
}

template<class KeyType>
int IndexedPriorityQLow<KeyType>::Pop()
{
	Swap(1, m_size);

	ReorderDownwards(1, m_size - 1);

	return m_Heap[m_size--];
}

template<class KeyType>
void IndexedPriorityQLow<KeyType>::ChangePriority(const int idx)
{
	ReorderUpwards(m_invHeap[idx]);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
template<class KeyType>
void IndexedPriorityQLow<KeyType>::Swap(int a, int b)
{
	int temp = m_Heap[a]; m_Heap[a] = m_Heap[b]; m_Heap[b] = temp;

	m_invHeap[m_Heap[a]] = a; m_invHeap[m_Heap[b]] = b;
}

template<class KeyType>
void IndexedPriorityQLow<KeyType>::ReorderUpwards(int nd)
{
	while ((nd > 1) && (m_vecKeys[m_Heap[nd / 2]] > m_vecKeys[m_Heap[nd]]))
	{
		Swap(nd / 2, nd);

		nd /= 2;
	}
}

template<class KeyType>
void IndexedPriorityQLow<KeyType>::ReorderDownwards(int nd, int HeapSize)
{
	while (2 * nd <= HeapSize)
	{
		int child = 2 * nd;

		if ((child < HeapSize) && (m_vecKeys[m_Heap[child]] > m_vecKeys[m_Heap[child + 1]]))
		{
			++child;
		}

		if (m_vecKeys[m_Heap[nd]] > m_vecKeys[m_Heap[child]])
		{
			Swap(child, nd);
			nd = child;
		}
		else
		{
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////
// Astar
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CAstar::CAstar(const CWayFinder *pFinder, int source, int target)
	: m_pFinder(pFinder), m_source(source), m_target(target),
	m_GCosts(pFinder->GetNodeCount(), 0.0), m_FCosts(pFinder->GetNodeCount(), 0.0),
	m_shortestPathTree(pFinder->GetNodeCount()), m_searchFrontier(pFinder->GetNodeCount())
{
	m_pPQ = new IndexedPriorityQLow<double>(m_FCosts, pFinder->GetNodeCount());
	m_pPQ->insert(m_source);
}

CAstar::~CAstar()
{
	if (m_pPQ) delete m_pPQ;
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
int CAstar::FindPath()
{
	int NextClosestNode;

	// 못 찾은 경우
	if (m_pPQ->empty()) return States::Not_Found;

	NextClosestNode = m_pPQ->Pop();
	m_shortestPathTree[NextClosestNode] = m_searchFrontier[NextClosestNode];

	// 목표에 도착한 경우
	if (NextClosestNode == m_target) return States::Found;

	const EdgeVector &edges = m_pFinder->GetEdgesAt(NextClosestNode);
	for (int i = 0; i < edges.size(); ++i)
	{
		// 휴리스틱 구한다.           
		double HCost = Heuristic_Diagonal(m_target, edges[i].To());

		// 단계 코스트 구한다.
		double GCost = m_GCosts[NextClosestNode] + edges[i].Cost();

		// 코스트를 구한다.
		if (m_searchFrontier[edges[i].To()] == NULL)
		{
			m_FCosts[edges[i].To()] = GCost + HCost;
			m_GCosts[edges[i].To()] = GCost;

			m_pPQ->insert(edges[i].To());

			m_searchFrontier[edges[i].To()] = &edges[i];
		}

		// 이미 지나간 길도 더 효율적인 길이면 그쪽으로 간다.
		else if ((GCost < m_GCosts[edges[i].To()]) && (m_shortestPathTree[edges[i].To()] == NULL))
		{
			m_FCosts[edges[i].To()] = GCost + HCost;
			m_GCosts[edges[i].To()] = GCost;

			m_pPQ->ChangePriority(edges[i].To());

			m_searchFrontier[edges[i].To()] = &edges[i];
		}
	}

	return States::Processing;
}

Path* CAstar::GetPath()
{
	Path *path = new Path;

	if (m_target < 0)  return path;

	int nd = m_target;

	while ((nd != m_source) && (m_shortestPathTree[nd] != 0))
	{
		path->push_front(CPathEdge(m_pFinder->GetNodeAt(m_shortestPathTree[nd]->From()).Position(),
			m_pFinder->GetNodeAt(m_shortestPathTree[nd]->To()).Position()));

		nd = m_shortestPathTree[nd]->From();
	}

	return path;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
// 참고 : http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html#S7 
double CAstar::Heuristic_Menhattan(int from, int to)
{
	XMFLOAT2 fromDividIndex = m_pFinder->GetNodeAt(from).Position();
	XMFLOAT2 toDividIndex = m_pFinder->GetNodeAt(to).Position();
	return (abs(fromDividIndex.x - toDividIndex.x) + abs(fromDividIndex.y - toDividIndex.y)) * 10.0f;
}

double CAstar::Heuristic_Diagonal(int from, int to)
{
	XMFLOAT2 fromDividIndex = m_pFinder->GetNodeAt(from).Position();
	XMFLOAT2 toDividIndex = m_pFinder->GetNodeAt(to).Position();
	float dx = abs(fromDividIndex.x - toDividIndex.x);
	float dy = abs(fromDividIndex.y - toDividIndex.y);
	return 10.0f * max(dx, dy) + /*(14.0f -  10.0f)*/ 4.0f * min(dx, dy);
}

double CAstar::Heuristic_Euclidean(int from, int to)
{
	XMFLOAT2 fromDividIndex = m_pFinder->GetNodeAt(from).Position();
	XMFLOAT2 toDividIndex = m_pFinder->GetNodeAt(to).Position();
	return 10.0f * Vector2::Distance(fromDividIndex, toDividIndex);
}

double CAstar::Heuristic_EuclideanSquare(int from, int to)
{
	XMFLOAT2 fromDividIndex = m_pFinder->GetNodeAt(from).Position();
	XMFLOAT2 toDividIndex = m_pFinder->GetNodeAt(to).Position();
	return 10.0f * Vector2::DistanceSquare(fromDividIndex, toDividIndex);
}

double CAstar::Heuristic_NoisyEuclidean(int from, int to)
{
	XMFLOAT2 fromDividIndex = m_pFinder->GetNodeAt(from).Position();
	XMFLOAT2 toDividIndex = m_pFinder->GetNodeAt(to).Position();
	return 10.0f * Vector2::Distance(fromDividIndex, toDividIndex) * RandInRange(0.9f, 1.1f);
}

double CAstar::Heuristic_Dijkstra(int from, int to)
{
	return 0.0;
}