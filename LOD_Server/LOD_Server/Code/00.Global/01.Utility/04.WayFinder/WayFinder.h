#pragma once
#include "00.Global/01.Utility/04.WayFinder/00.Node/Node.h"
#include "00.Global/01.Utility/04.WayFinder/01.Edge/Edge.h"
#include "06.Meshes/01.Mesh/Mesh.h"

typedef std::vector<CNode> NodeVector;
typedef std::vector<CEdge> EdgeVector;
typedef std::vector<EdgeVector> EdgeArray;
typedef std::list<CPathEdge> Path;

class CAstar;
class CCollisionObject;

class CWayFinder
{
public: // ������, �Ҹ���
	CWayFinder(float sizeX, float sizeY);
	~CWayFinder();

public: // ���� �Լ�
	bool CanGoDirectly(XMFLOAT2 &source, XMFLOAT2 &target, float collisionSize);
	bool CanGoDirectly(XMFLOAT3 &source, XMFLOAT3 &target, float collisionSize);
	XMFLOAT2 GetClosestNotCollidePos(XMFLOAT2 &source, XMFLOAT2 &target, float collisionSize);
	XMFLOAT3 GetClosestNotCollidePos(XMFLOAT3 &source, XMFLOAT3 &target, float collisionSize);
	Path *GetPathToPosition(XMFLOAT2 &source, XMFLOAT2 &target, float collisionSize);

	void SmoothPath(Path *path, float collisionSize);
	void SmoothPathDetail(Path *path, float collisionSize);

	int FindClosestNodeIndexWithPosition(const XMFLOAT2 &position);

	void AdjustValueByWallCollision(CCollisionObject* collider, XMFLOAT3& dir, float val);

	int GetNodeCount() const { return m_nodes.size(); }
	const CNode& GetNodeAt(int idx) const { return m_nodes[idx]; }
	const EdgeVector& GetEdgesAt(int idx) const { return m_edges[idx]; }

protected: // ���� �Լ�

protected: // ����
	NodeVector m_nodes;
	EdgeArray m_edges;

	XMFLOAT2 m_size{ 0, 0 };	// ��� �� ĭ�� ũ��
	POINT m_nodeMax{ 0, 0 };	// ��� �ִ� ����(����, ����)

	CAstar* m_pCurSearch{ NULL };
	CCollisionMapImage *m_pCollisionMapImage{ NULL };
};