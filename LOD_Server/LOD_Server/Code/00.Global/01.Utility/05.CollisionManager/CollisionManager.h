#pragma once
class CCollisionObject;
class CWayFinder;
struct NodeMap;
struct CharacterStatus {
	int HP;
	int Atk;
	CharacterStatus(int hp, int atk) {
		hp; atk;
	}
};

typedef std::list<CCollisionObject*> CollisionObjectList;

class CCollisionManager
{
public:
	CCollisionManager();
	void GameOver(TeamType type);
	void SetNodeMap(std::vector<NodeMap> map, float size, XMFLOAT2 wh);
	void AddCollider(CCollisionObject* pcol);
	void Update(shared_ptr<CWayFinder> pWayFinder);
	void RequestCollide(CollisionType type, CCollisionObject* pCol, float data1 = 0, float data2 = 0, float damage = 0);
	CCollisionObject* RequestNearObject(CCollisionObject* pCol, float lengh, TeamType type, bool player = false);
	void RequestIncreaseExp(CCollisionObject* pCol, float sightRange, TeamType type, UINT exp);

	PlayerInfo* NearFightingValue(CCollisionObject* pCol, TeamType type);
	XMFLOAT2 GetFrontLinePosition(int line, TeamType type);
	CollisionObjectList* GetEnemyList(TeamType type);
	CollisionObjectList* GetTeamList(CCollisionObject* pCol, TeamType type);

	~CCollisionManager();
	int(*GetFoW(void))[NODE_HEIGHT];

	void SetMyTeam(TeamType team) { m_User = team; }

	float GetnodeSize() { return nodeSize; }
	
protected:
	void SearchSight(int startX, int startY, int dir, int slength, TeamType team);

	bool NearLevel(XMFLOAT2 a, XMFLOAT2 b, bool attack = false) {

		if ((Vector2::Distance(a, b) <15 && !attack) || (Vector2::Distance(a, b) <20 && attack))
			return true;
		else
			return false;
	}

protected:
	TeamType m_Winner{ TeamType::None };
	TeamType m_User{ TeamType::Blue };
	NodeMap** m_BlueSight;
	NodeMap** m_RedSight;
	float nodeSize{ 0 };
	XMFLOAT2 nodeWH;
	int Fow[NODE_WIDTH][NODE_HEIGHT];


	CollisionObjectList m_lstColliders;

	CollisionObjectList m_lstBlueSight;
	CollisionObjectList m_lstRedSight;

	CollisionObjectList m_lstReturn;
};

