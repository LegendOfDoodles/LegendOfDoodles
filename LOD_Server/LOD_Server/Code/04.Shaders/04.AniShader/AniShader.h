#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "00.Global/01.Utility/04.WayFinder/01.Edge/Edge.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"

typedef std::list<CPathEdge> Path;
typedef std::list<CCollisionObject*> CollisionObjectList;

class CHeightMapTerrain;
class CCollisionManager;
class CFSMMgr;

class CAniShader : public CShader
{
public: // 생성자, 소멸자
	CAniShader();
	virtual ~CAniShader();

public: // 공개 함수
	virtual void AnimateObjects(float timeElapsed);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);

	void SetCollisionManager(CCollisionManager* pManger) { m_pColManager = pManger; }
	void SetFSMManager(CFSMMgr *pManger) { m_pFSMMgr = pManger; }

protected: // 내부 함수
	virtual void BuildObjects(void *pContext = NULL);

	virtual void ReleaseObjects();

	void CreatePathes();

	int GetPossibleIndex();
	void SpawnMinion();

	void ResetPossibleIndex(int idx) { m_indexArr[idx] = false; }

protected: // 변수
	CBaseObject * *m_ppObjects{ NULL };
	int m_nObjects{ 0 };

	ObjectType m_kind{ ObjectType::SwordMinion };

	CollisionObjectList m_blueObjects;
	CollisionObjectList m_redObjects;

	bool m_indexArr[MAX_MINION]{ false };

	Path m_pathes[4];

	CCollisionManager* m_pColManager{NULL};
	CHeightMapTerrain * m_pTerrain{ NULL };

	CFSMMgr * m_pFSMMgr{ NULL };

	float m_spawnTime{ 10.1f };
	float m_preSpawnTime{ 0.0f };
};
