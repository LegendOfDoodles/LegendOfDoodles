#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "00.Global/01.Utility/04.WayFinder/01.Edge/Edge.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"

typedef std::list<CPathEdge> Path;
typedef std::list<CCollisionObject*> CollisionObjectList;

class CMaterial;
class CHeightMapTerrain;
class CCollisionManager;
class CThrowingMgr;
class CUIObjectManager;
class CFSMMgr;

class CMinionShader : public CShader
{
public: // ������, �Ҹ���
	CMinionShader();
	virtual ~CMinionShader();

public: // ���� �Լ�
	virtual void Initialize(void *pContext = NULL);

	virtual void AnimateObjects(float timeElapsed);

	virtual CollisionObjectList* GetMinionList(ObjectType Minion_Type, TeamType Team_Type);

	void SetCollisionManager(shared_ptr<CCollisionManager> pManger) { m_pColManager = pManger; }
	void SetThrowingManager(shared_ptr<CThrowingMgr> pManger) { m_pThrowingMgr = pManger; }
	void SetFSMManager(shared_ptr<CFSMMgr> pManger) { m_pFSMMgr = pManger; }

	

protected: // ���� �Լ�
	virtual void BuildObjects(void *pContext = NULL);

	virtual void ReleaseObjects();

	void CreatePathes();

	int GetPossibleIndex();
	void SpawnMinion();

	void SetPossibleIndex(int idx) { m_indexArr[idx] = true; }
	void ResetPossibleIndex(int idx) { m_indexArr[idx] = false; }

protected: // ����
	ObjectType m_kind{ ObjectType::SwordMinion };

	CollisionObjectList m_blueSwordMinions;
	CollisionObjectList m_blueStaffMinions;
	CollisionObjectList m_blueBowMinions;
	CollisionObjectList m_redSwordMinions;
	CollisionObjectList m_redStaffMinions;
	CollisionObjectList m_redBowMinions;

	bool m_indexArr[MAX_MINION];

	Path m_pathes[4];

	shared_ptr<CCollisionManager> m_pColManager;
	shared_ptr<CThrowingMgr> m_pThrowingMgr;
	CHeightMapTerrain * m_pTerrain{ NULL };

	shared_ptr<CFSMMgr> m_pFSMMgr;

	bool m_bSpawning{ false };
	bool m_neverSpawn{ false };

	float m_readyTime{ 0.0f };
	float m_spawnTime{ 0.0f };
	float m_preSpawnTime{ 0.0f };

	int m_curSpawnCount{ 0 };

	int m_minionId{ 0 };	// 0 ~ 1000
};
