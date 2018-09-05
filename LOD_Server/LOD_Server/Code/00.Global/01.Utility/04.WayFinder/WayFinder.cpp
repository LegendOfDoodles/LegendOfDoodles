#include "stdafx.h"
#include "WayFinder.h"
#include "00.Global/01.Utility/04.WayFinder/02.Astar/Astar.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"

/// <summary>
/// 목적: 길찾기 알고리즘을 위한 클래스 작성
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-05
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CWayFinder::CWayFinder()
{
	// 충돌 맵 읽어오기
	m_pCollisionMapImage = shared_ptr<CCollisionMapImage>(new CCollisionMapImage(
		_T("Resource/Terrain/TerrainCollision.raw"),
		static_cast<int>(TETRRAIN_COLLISION_WIDTH),
		static_cast<int>(TETRRAIN_COLLISION_HEIGHT),
		TERRAIN_COLLISION_SCALE));

	// 노드 읽어오기
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

	// 에지 읽어오기
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
// 공개 함수
// 직선 이동 가능한지 파악하기 위한 함수
bool CWayFinder::CanGoDirectly(const XMFLOAT2 & source, const XMFLOAT2 & target)
{
	// 해당 방향으로 조금씩 이동하면서 주변에 충돌하는 경우가 발생하는지 파악하여 충돌이 없으면 진행 가능으로 판단한다.
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

// 충돌 지점에서 충돌이 없는 가장 가까운 지점을 찾는 함수
XMFLOAT2 CWayFinder::GetClosestNotCollidePos(const XMFLOAT2 & source, const XMFLOAT2 & target)
{
	// 해당 방향으로 조금씩 이동하면서 주변에 충돌하는 경우가 발생하는지 파악하여 충돌이 없으면 진행 가능으로 판단한다.
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
	// Source 기준으로 가장 가까운 충돌하지 않는 위치를 찾는다.
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

	// 시작지와 끝 지점에서 가장 가까운 노드 검색
	int srcIndex = FindClosestNodeIndexWithPosition(adjSource);
	if (srcIndex == INVALID_NODE) return nullptr;
	int dstIndex = FindClosestNodeIndexWithPosition(adjTarget);
	if (dstIndex == INVALID_NODE) return nullptr;

	// 바로 근처면 이동하지 않음
	if (Vector2::DistanceSquare(adjSource, adjTarget) < NODE_SIZE_SQR)
	{
		return nullptr;
	}
	else if (CanGoDirectly(adjSource, adjTarget))
	{
		// 직선으로 이동 가능한 경우 목적지만 패스에 넣고 종료
		path = new Path;
		path->push_back(CPathEdge(adjSource, adjTarget));
		return path;
	}
	else
	{
		// 길찾기 수행
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
			// 찾은 패스 저장
			path = m_pCurSearch->GetPath();
			if (!path->empty())
			{
				// 소스가 오른쪽 이었으면 패스를 뒤집는다.
				if (adjSource.x <= adjTarget.x)
				{
					// 패스에 도착지를 추가로 연결하고 종료한다.
					if (CanGoDirectly(path->back().To(), adjTarget))
					{
						path->push_back(CPathEdge(path->back().To(), adjTarget));
					}
				}
			}
		}

		m_pCurSearch.reset();
	}

	// 직선 상으로 갈 수 있는 길 돌아가지 않도록 설정
	// 길이 없는 경우 NULL 리턴
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
// 내부 함수
