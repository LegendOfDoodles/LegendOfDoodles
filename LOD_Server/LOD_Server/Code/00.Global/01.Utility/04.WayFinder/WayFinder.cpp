#include "stdafx.h"
#include "WayFinder.h"
#include "00.Global/01.Utility/04.WayFinder/02.Astar/Astar.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"

/// <summary>
/// ����: ��ã�� �˰����� ���� Ŭ���� �ۼ�
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-09-05
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CWayFinder::CWayFinder()
{
	// �浹 �� �о����
	m_pCollisionMapImage = shared_ptr<CCollisionMapImage>(new CCollisionMapImage(
		_T("Resource/Terrain/TerrainCollision.raw"),
		static_cast<int>(TETRRAIN_COLLISION_WIDTH),
		static_cast<int>(TETRRAIN_COLLISION_HEIGHT),
		TERRAIN_COLLISION_SCALE));

	// ��� �о����
	std::ifstream nodeIn("Resource/Data/AStar/nodesData.txt", std::ios::in, std::ios::binary);

	int maxNode{ 0 };
	nodeIn >> maxNode;
	m_nodes.reserve(maxNode);
	m_nodeMap.reserve(maxNode);

	int index{ INVALID_NODE };

	XMFLOAT2 position, size{ 0, 0 };
	for (int i = 0; i < maxNode; ++i)
	{
		nodeIn >> index >> position.x >> position.y >> size.x >> size.y;
		m_nodes.emplace_back(index, position, size);
		NodeMap tmp;
		tmp.Detected = false;
		if (index == -1)
			tmp.Static = true;
		else
			tmp.Static = false;
		m_nodeMap.push_back(tmp);
		if (position.x == 0)m_nodeWH.y++;
	}
	nodeSize = size.x;
	m_nodeWH.x = maxNode / m_nodeWH.y;

	nodeIn.close();

	// ���� �о����
	std::ifstream edgeIn("Resource/Data/AStar/edgesData.txt", std::ios::in, std::ios::binary);

	int maxEdge{ 0 };
	edgeIn >> maxEdge;
	m_edges.resize(maxEdge);

	int edgeSize{ 0 };
	int from{ 0 }, to{ 0 }, cost{ 0 };
	for (int i = 0; i < maxEdge; ++i)
	{
		edgeIn >> edgeSize;
		if (edgeSize)
		{
			m_edges[i].reserve(edgeSize);
			for (int j = 0; j < edgeSize; ++j)
			{
				edgeIn >> from >> to >> cost;
				m_edges[i].emplace_back(from, to, cost);
			}
		}
	}

	edgeIn.close();
}

CWayFinder::~CWayFinder()
{
	m_nodes.clear();
	for (int i = 0; i < m_edges.size(); ++i)
	{
		m_edges[i].clear();
	}
	m_edges.clear();
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
// ���� �̵� �������� �ľ��ϱ� ���� �Լ�
bool CWayFinder::CanGoDirectly(const XMFLOAT2 & source, const XMFLOAT2 & target)
{
	// �ش� �������� ���ݾ� �̵��ϸ鼭 �ֺ��� �浹�ϴ� ��찡 �߻��ϴ��� �ľ��Ͽ� �浹�� ������ ���� �������� �Ǵ��Ѵ�.
	XMFLOAT2 toTarget{ Vector2::Subtract(target, source, true) };
	XMFLOAT2 addVal{ Vector2::ScalarProduct(toTarget, NODE_SIZE_HALF) };
	XMFLOAT2 curPos = Vector2::Add(source, addVal);

	do
	{
		for (float checker = -NODE_SIZE; checker < NODE_SIZE; checker += CONVERT_PaperUnit_to_InG(1))
		{
			if (m_pCollisionMapImage->GetCollision(curPos.x, curPos.y + checker)) return false;
			if (m_pCollisionMapImage->GetCollision(curPos.x + checker, curPos.y)) return false;
			if (m_pCollisionMapImage->GetCollision(curPos.x - checker, curPos.y + checker)) return false;
			if (m_pCollisionMapImage->GetCollision(curPos.x + checker, curPos.y - checker)) return false;
			if (m_pCollisionMapImage->GetCollision(curPos.x + checker, curPos.y + checker)) return false;
		}
		curPos = Vector2::Add(curPos, addVal);
	} while (Vector2::DistanceSquare(curPos, target) > NODE_SIZE_SQR);
	return true;
}

bool CWayFinder::CanGoDirectly(const XMFLOAT3 & source, const XMFLOAT3 & target)
{
	return CanGoDirectly(XMFLOAT2(source.x, source.z), XMFLOAT2(target.x, target.z));
}

// �浹 �������� �浹�� ���� ���� ����� ������ ã�� �Լ�
XMFLOAT2 CWayFinder::GetClosestNotCollidePos(const XMFLOAT2 & source, const XMFLOAT2 & target)
{
	// �ش� �������� ���ݾ� �̵��ϸ鼭 �ֺ��� �浹�ϴ� ��찡 �߻��ϴ��� �ľ��Ͽ� �浹�� ������ ���� �������� �Ǵ��Ѵ�.
	XMFLOAT2 toTarget{ Vector2::Subtract(target, source, true) };
	XMFLOAT2 addVal{ Vector2::ScalarProduct(toTarget, NODE_SIZE_HALF) };
	XMFLOAT2 curPos = source;

	do
	{
		for (float yChecker = -NODE_SIZE; yChecker < NODE_SIZE; yChecker += CONVERT_PaperUnit_to_InG(1))
		{
			for (float xChecker = -NODE_SIZE; xChecker < NODE_SIZE; xChecker += CONVERT_PaperUnit_to_InG(1))
			{
				if (!m_pCollisionMapImage->GetCollision(curPos.x, curPos.y + yChecker)) return XMFLOAT2(curPos.x, curPos.y + yChecker);
				if (!m_pCollisionMapImage->GetCollision(curPos.x + xChecker, curPos.y)) return XMFLOAT2(curPos.x + xChecker, curPos.y);
				if (!m_pCollisionMapImage->GetCollision(curPos.x - xChecker, curPos.y + yChecker)) return XMFLOAT2(curPos.x - xChecker, curPos.y + yChecker);
				if (!m_pCollisionMapImage->GetCollision(curPos.x + xChecker, curPos.y - yChecker)) return XMFLOAT2(curPos.x + xChecker, curPos.y - yChecker);
				if (!m_pCollisionMapImage->GetCollision(curPos.x + xChecker, curPos.y + yChecker)) return XMFLOAT2(curPos.x + xChecker, curPos.y + yChecker);
			}
		}
		curPos = Vector2::Add(curPos, addVal);
	} while (Vector2::DistanceSquare(curPos, target) > NODE_SIZE_SQR);
	return target;
}

XMFLOAT3 CWayFinder::GetClosestNotCollidePos(const XMFLOAT3 & source, const XMFLOAT3 & target)
{
	XMFLOAT2 result{ GetClosestNotCollidePos(XMFLOAT2(source.x, source.z), XMFLOAT2(target.x, target.z)) };
	return XMFLOAT3(result.x, 0, result.y);
}

XMFLOAT2 CWayFinder::GetClosestNotCollidePos(const XMFLOAT2 & source)
{
	// Source �������� ���� ����� �浹���� �ʴ� ��ġ�� ã�´�.
	int cnt{ 1 };
	while (true)
	{
		for (float yChecker = -NODE_SIZE * cnt; yChecker < NODE_SIZE * cnt; yChecker += CONVERT_PaperUnit_to_InG(1))
		{
			for (float xChecker = -NODE_SIZE * cnt; xChecker < NODE_SIZE * cnt; xChecker += CONVERT_PaperUnit_to_InG(1))
			{
				if (!m_pCollisionMapImage->GetCollision(source.x, source.y + yChecker)) return XMFLOAT2(source.x, source.y + yChecker);
				if (!m_pCollisionMapImage->GetCollision(source.x + xChecker, source.y)) return XMFLOAT2(source.x + xChecker, source.y);
				if (!m_pCollisionMapImage->GetCollision(source.x - xChecker, source.y + yChecker)) return XMFLOAT2(source.x - xChecker, source.y + yChecker);
				if (!m_pCollisionMapImage->GetCollision(source.x + xChecker, source.y - yChecker)) return XMFLOAT2(source.x + xChecker, source.y - yChecker);
				if (!m_pCollisionMapImage->GetCollision(source.x + xChecker, source.y + yChecker)) return XMFLOAT2(source.x + xChecker, source.y + yChecker);
			}
		}
		cnt++;
	}

	return XMFLOAT2();
}

XMFLOAT3 CWayFinder::GetClosestNotCollidePos(const XMFLOAT3 & source)
{
	XMFLOAT2 result{ GetClosestNotCollidePos(XMFLOAT2(source.x, source.z)) };
	return XMFLOAT3(result.x, 0, result.y);
}

Path *CWayFinder::GetPathToPosition(const XMFLOAT2 &source, const XMFLOAT2 &target)
{
	Path *path{ nullptr };

	XMFLOAT2 adjSource{ GetClosestNotCollidePos(source) }, adjTarget{ GetClosestNotCollidePos(target) };

	// �������� �� �������� ���� ����� ��� �˻�
	int srcIndex = FindClosestNodeIndexWithPosition(adjSource);
	if (srcIndex == INVALID_NODE) return nullptr;
	int dstIndex = FindClosestNodeIndexWithPosition(adjTarget);
	if (dstIndex == INVALID_NODE) return nullptr;

	// �ٷ� ��ó�� �̵����� ����
	if (Vector2::DistanceSquare(adjSource, adjTarget) < NODE_SIZE_SQR)
	{
		return nullptr;
	}
	else if (CanGoDirectly(adjSource, adjTarget))
	{
		// �������� �̵� ������ ��� �������� �н��� �ְ� ����
		path = new Path;
		path->push_back(CPathEdge(adjSource, adjTarget));
		return path;
	}
	else
	{
		// ��ã�� ����
		if (adjSource.x < adjTarget.x) m_pCurSearch = shared_ptr<CAstar>(new CAstar(shared_from_this(), srcIndex, dstIndex));
		else m_pCurSearch = shared_ptr<CAstar>(new CAstar(shared_from_this(), dstIndex, srcIndex));

		int result;
		for (int i = 0; i < 10000; ++i)
		{
			result = m_pCurSearch->FindPath();
			if (result == States::Found || result == States::Not_Found)
				break;
		}
		if (result == States::Found)
		{
			// ã�� �н� ����
			path = m_pCurSearch->GetPath();
			if (!path->empty())
			{
				// �ҽ��� ������ �̾����� �н��� �����´�.
				if (adjSource.x <= adjTarget.x)
				{
					// �н��� �������� �߰��� �����ϰ� �����Ѵ�.
					if (CanGoDirectly(path->back().To(), adjTarget))
					{
						path->push_back(CPathEdge(path->back().To(), adjTarget));
					}
				}
			}
		}

		m_pCurSearch.reset();
	}

	// ���� ������ �� �� �ִ� �� ���ư��� �ʵ��� ����
	// ���� ���� ��� NULL ����
	if(!SmoothPathDetail(path)) return NULL;

	if (adjSource.x > adjTarget.x)
	{
		path->reverse();
		path->push_back(CPathEdge(path->back().To(), path->back().From()));
	}

	return path;
}

Path * CWayFinder::GetPathToPosition(const XMFLOAT3 & source, const XMFLOAT3 & target)
{
	return GetPathToPosition(XMFLOAT2(source.x, source.z), XMFLOAT2(target.x, target.z));
}

Path * CWayFinder::GetPathToPosition(const XMFLOAT3 & source, const XMFLOAT2 & target)
{
	return GetPathToPosition(XMFLOAT2(source.x, source.z), target);
}

bool CWayFinder::SmoothPath(Path *path)
{
	if (!path) return false;

	Path::iterator e1(path->begin()), e2(path->begin());

	++e2;

	while (e2 != path->end())
	{
		if (CanGoDirectly(e1->From(), e2->To()))
		{
			e1->SetDestination(e2->To());
			e2 = path->erase(e2);
		}
		else
		{
			e1 = e2;
			++e2;
		}
	}

	return true;
}

bool CWayFinder::SmoothPathDetail(Path * path)
{
	if (!path) return false;

	Path::iterator e1(path->begin()), e2;

	while (e1 != path->end())
	{
		e2 = e1;
		++e2;

		while (e2 != path->end())
		{
			if (CanGoDirectly(e1->From(), e2->To()))
			{
				e1->SetDestination(e2->To());
				e2 = path->erase(++e1, ++e2);
				e1 = e2;
				--e1;
			}
			else
			{
				++e2;
			}
		}
		++e1;
	}

	return true;
}

int CWayFinder::FindClosestNodeIndexWithPosition(const XMFLOAT2 & position)
{
	int closestIndex{ INVALID_NODE };
	int closestRange{ INT_MAX };
	int curRange{ INT_MAX };

	for (int i = 0; i < m_nodes.size(); ++i)
	{
		if (m_nodes[i].Index() == INVALID_NODE) continue;
		curRange = m_nodes[i].GetDistanceSquareWithPosition(position);
		if (curRange < closestRange)
		{
			closestRange = curRange;
			closestIndex = m_nodes[i].Index();
		}
	}

	return closestIndex;
}

void CWayFinder::AdjustValueByWallCollision(CCollisionObject* collider, const XMFLOAT3 & dir, float val)
{
	XMFLOAT3 pos{ collider->GetPosition() };
	XMFLOAT3 velocity{ Vector3::ScalarProduct(dir, val) };
	velocity.y = 0;
	XMFLOAT3 newPos{ Vector3::Add(pos, velocity) };

	if (CanGoDirectly(pos, newPos))
	{
		collider->Translate(&velocity);
	}
	else
	{
		XMFLOAT3 newDir{ Vector3::Subtract(GetClosestNotCollidePos(pos, newPos), newPos, true) };
		XMFLOAT3 axis{ Vector3::ScalarProduct(newDir, val) };
		collider->Translate(&axis);
	}
}

bool CWayFinder::IsInTerrain(const XMFLOAT2 & target)
{
	if (target.x < 0) return false;
	if (target.x > TERRAIN_SIZE_WIDTH) return false;
	if (target.y < 0) return false;
	if (target.y > TERRAIN_SIZE_HEIGHT) return false;
	return true;
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
