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
struct NodeMap {
	bool Detected{ false };
	bool Static{ false };
};
class CWayFinder : public std::enable_shared_from_this<CWayFinder>
{
public: // 생성자, 소멸자
	CWayFinder();
	~CWayFinder();

public: // 공개 함수
	bool CanGoDirectly(const XMFLOAT2 &source, const XMFLOAT2 &target);
	bool CanGoDirectly(const XMFLOAT3 &source, const XMFLOAT3 &target);
	XMFLOAT2 GetClosestNotCollidePos(const XMFLOAT2 &source, const XMFLOAT2 &target);
	XMFLOAT3 GetClosestNotCollidePos(const XMFLOAT3 &source, const XMFLOAT3 &target);
	Path *GetPathToPosition(const XMFLOAT2 &source, const XMFLOAT2 &target);
	Path *GetPathToPosition(const XMFLOAT3 &source, const XMFLOAT3 &target);
	Path *GetPathToPosition(const XMFLOAT3 &source, const XMFLOAT2 &target);

	void SmoothPath(Path *path);
	void SmoothPathDetail(Path *path);

	int FindClosestNodeIndexWithPosition(const XMFLOAT2 &position);

	void AdjustValueByWallCollision(CCollisionObject* collider, const XMFLOAT3& dir, float val);

	bool IsInTerrain(const XMFLOAT2& target);

	int GetNodeCount() const { return static_cast<int>(m_nodes.size()); }
	const CNode& GetNodeAt(int idx) const { return m_nodes[idx]; }
	const EdgeVector& GetEdgesAt(int idx) const { return m_edges[idx]; }
	std::vector<NodeMap> GetNodeMap() { return m_nodeMap; }
	float GetNodeSize() { return nodeSize; }
	XMFLOAT2 GetNodeWH() { return m_nodeWH; }
protected: // 내부 함수

protected: // 변수
	NodeVector m_nodes;
	EdgeArray m_edges;
	
	std::vector<NodeMap> m_nodeMap;
	float nodeSize{ 0 };
	XMFLOAT2 m_nodeWH;

	shared_ptr<CAstar> m_pCurSearch;
	shared_ptr<CCollisionMapImage> m_pCollisionMapImage;
};