#include "stdafx.h"
#include "WayFinder.h"
#include "00.Global/01.Utility/04.WayFinder/02.Astar/Astar.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"

/// <summary>
/// ����: ��ã�� �˰����� ���� Ŭ���� �ۼ�
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-06-01
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CWayFinder::CWayFinder()
{
	// �浹 �� �о����
	m_pCollisionMapImage = new CCollisionMapImage(_T("Resource/Terrain/TerrainCollision.raw"),
		TETRRAIN_COLLISION_WIDTH, TETRRAIN_COLLISION_HEIGHT, TERRAIN_COLLISION_SCALE);

	// ��� �о����
	std::ifstream nodeIn("Resource/Data/AStar/nodesData.txt", std::ios::in, std::ios::binary);

	int maxNode{ 0 };
	nodeIn >> maxNode;
	m_nodes.reserve(maxNode);

	int index{ INVALID_NODE };
	XMFLOAT2 position, size;
	for (int i = 0; i < maxNode; ++i)
	{
		nodeIn >> index >> position.x >> position.y >> size.x >> size.y;
		m_nodes.emplace_back(index, position, size);
	}

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
	if (m_pCollisionMapImage) Safe_Delete(m_pCollisionMapImage);
}

// ���� �̵� �������� �ľ��ϱ� ���� �Լ�
bool CWayFinder::CanGoDirectly(XMFLOAT2 & source, XMFLOAT2 & target, float collisionSize)
{
	// �ش� �������� ���ݾ� �̵��ϸ鼭 �ֺ��� �浹�ϴ� ��찡 �߻��ϴ��� �ľ��Ͽ� �浹�� ������ ���� �������� �Ǵ��Ѵ�.
	XMFLOAT2 toTarget{ Vector2::Subtract(target, source, true) };
	XMFLOAT2 addVal{ Vector2::ScalarProduct(toTarget, collisionSize * 0.5f) };
	XMFLOAT2 curPos = source;

	do
	{
		if (m_pCollisionMapImage->GetCollision(curPos.x - collisionSize, curPos.y)) return false;
		if (m_pCollisionMapImage->GetCollision(curPos.x, curPos.y + collisionSize)) return false;
		if (m_pCollisionMapImage->GetCollision(curPos.x + collisionSize, curPos.y)) return false;
		if (m_pCollisionMapImage->GetCollision(curPos.x, curPos.y - collisionSize)) return false;
		curPos = Vector2::Add(curPos, addVal);
	} while (Vector2::DistanceSquare(curPos, target) > collisionSize * collisionSize);
	return true;
}

bool CWayFinder::CanGoDirectly(XMFLOAT3 & source, XMFLOAT3 & target, float collisionSize)
{
	return CanGoDirectly(XMFLOAT2(source.x, source.z), XMFLOAT2(target.x, target.z), collisionSize);
}

// �浹 �������� �浹�� ���� ���� ����� ������ ã�� �Լ�
XMFLOAT2 CWayFinder::GetClosestNotCollidePos(XMFLOAT2 & source, XMFLOAT2 & target, float collisionSize)
{
	// �ش� �������� ���ݾ� �̵��ϸ鼭 �ֺ��� �浹�ϴ� ��찡 �߻��ϴ��� �ľ��Ͽ� �浹�� ������ ���� �������� �Ǵ��Ѵ�.
	XMFLOAT2 toTarget{ Vector2::Subtract(target, source, true) };
	XMFLOAT2 addVal{ Vector2::ScalarProduct(toTarget, collisionSize * 0.5f) };
	XMFLOAT2 curPos = source;

	do
	{
		if (!m_pCollisionMapImage->GetCollision(curPos.x - collisionSize, curPos.y)) return XMFLOAT2(curPos.x - collisionSize, curPos.y);
		if (!m_pCollisionMapImage->GetCollision(curPos.x +collisionSize, curPos.y)) return XMFLOAT2(curPos.x + collisionSize, curPos.y);
		if (!m_pCollisionMapImage->GetCollision(curPos.x, curPos.y - collisionSize)) return XMFLOAT2(curPos.x, curPos.y - collisionSize);
		if (!m_pCollisionMapImage->GetCollision(curPos.x, curPos.y + collisionSize)) return XMFLOAT2(curPos.x, curPos.y + collisionSize);
		if (!m_pCollisionMapImage->GetCollision(curPos.x - collisionSize, curPos.y - collisionSize)) return XMFLOAT2(curPos.x - collisionSize, curPos.y - collisionSize);
		if (!m_pCollisionMapImage->GetCollision(curPos.x + collisionSize, curPos.y - collisionSize)) return XMFLOAT2(curPos.x + collisionSize, curPos.y - collisionSize);
		if (!m_pCollisionMapImage->GetCollision(curPos.x - collisionSize, curPos.y + collisionSize)) return XMFLOAT2(curPos.x - collisionSize, curPos.y + collisionSize);
		if (!m_pCollisionMapImage->GetCollision(curPos.x + collisionSize, curPos.y + collisionSize)) return XMFLOAT2(curPos.x + collisionSize, curPos.y + collisionSize);
		curPos = Vector2::Add(curPos, addVal);
	} while (Vector2::DistanceSquare(curPos, target) > collisionSize * collisionSize);
	return target;
}

XMFLOAT3 CWayFinder::GetClosestNotCollidePos(XMFLOAT3 & source, XMFLOAT3 & target, float collisionSize)
{
	XMFLOAT2 result{ GetClosestNotCollidePos(XMFLOAT2(source.x, source.z), XMFLOAT2(target.x, target.z), collisionSize) };
	return XMFLOAT3(result.x, 0, result.y);
}

Path *CWayFinder::GetPathToPosition(XMFLOAT2 &source, XMFLOAT2 &target, float collisionSize)
{
	Path *path{ nullptr };

	// �������� �� �������� ���� ����� ��� �˻�
	int srcIndex = FindClosestNodeIndexWithPosition(source);
	if (srcIndex == INVALID_NODE) return nullptr;
	int dstIndex = FindClosestNodeIndexWithPosition(target);
	if (dstIndex == INVALID_NODE) return nullptr;

	// �������� �浹ü ���� ��� �������� �浹�� ���� ���� ����� ��ġ�� �����Ѵ�.
	if (m_pCollisionMapImage->GetCollision(target.x, target.y))
		target = GetClosestNotCollidePos(target, m_nodes[dstIndex].Position(), collisionSize);

	// �������� �̵� ������ ���
	if (CanGoDirectly(source, target, collisionSize))
	{
		// �������� �н��� �ְ� ����
		path = new Path;
		path->push_back(CPathEdge(source, target));
		return path;
	}
	else
	{
		// ��ã�� ����
		if(source.x < target.x) m_pCurSearch = new CAstar(this, srcIndex, dstIndex);
		else m_pCurSearch = new CAstar(this, dstIndex, srcIndex);

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
			// �ҽ��� ������ �̾����� �н��� �����´�.
			if (source.x < target.x)
			{
				// �н��� �������� �߰��� �����ϰ� �����Ѵ�.
				if (!path->empty()) path->push_back(CPathEdge(path->back().To(), target));
			}
			else
			{
				// �н��� �������� �߰��� �����ϰ� �����Ѵ�.
				if (!path->empty()) path->push_back(CPathEdge(path->back().To(), source));
				path->reverse();
			}
		}
	
		delete m_pCurSearch;
	}

	// ���� ������ �� �� �ִ� �� ���ư��� �ʵ��� ����
	SmoothPathDetail(path, collisionSize);

	return path;
}

void CWayFinder::SmoothPath(Path *path, float collisionSize)
{
	Path::iterator e1(path->begin()), e2(path->begin());

	++e2;

	while (e2 != path->end())
	{
		if (CanGoDirectly(e1->From(), e2->To(), collisionSize))
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
}

void CWayFinder::SmoothPathDetail(Path * path, float collisionSize)
{
	if (!path) return;

	Path::iterator e1(path->begin()), e2;

	while (e1 != path->end())
	{
		e2 = e1;
		++e2;

		while (e2 != path->end())
		{
			if (CanGoDirectly(e1->From(), e2->To(), collisionSize))
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

void CWayFinder::AdjustValueByWallCollision(CCollisionObject* collider, XMFLOAT3 & dir, float val)
{
	XMFLOAT3 pos{ collider->GetPosition() };
	XMFLOAT3 velocity{ Vector3::ScalarProduct(dir, val) };
	velocity.y = 0;
	XMFLOAT3 newPos{ Vector3::Add(pos, velocity) };

	if (CanGoDirectly(pos, newPos, collider->GetCollisionSize()))
	{
		collider->Translate(&velocity);
	}
	else
	{
		XMFLOAT3 newDir{ Vector3::Subtract(GetClosestNotCollidePos(pos, newPos, collider->GetCollisionSize()), newPos, true) };
		collider->Translate(&Vector3::ScalarProduct(newDir, val));
	}
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
