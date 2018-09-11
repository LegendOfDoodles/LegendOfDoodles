#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "00.Global/01.Utility/04.WayFinder/01.Edge/Edge.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"

typedef std::list<CPathEdge> Path;

class CMaterial;
class CHeightMapTerrain;
class CCollisionManager;
class CThrowingMgr;
class CFSMMgr;

struct NeutralObjectIndices
{
	int m_begIndex;
	int m_endIndex;

	NeutralObjectIndices() {}
	NeutralObjectIndices(int a, int b):m_begIndex(a), m_endIndex(b){}
};

class CNeutralityShader : public CShader
{
public: // 생성자, 소멸자
	CNeutralityShader();
	virtual ~CNeutralityShader();

public: // 공개 함수
	virtual void Initialize(void *pContext = NULL);

	virtual void AnimateObjects(float timeElapsed);

	void SetColManagerToObject(shared_ptr<CCollisionManager> manager);
	void SetThrowingManagerToObject(shared_ptr<CThrowingMgr> manager);

	virtual void UpdateActiveNeutralStatus();

	void SetCollisionManager(shared_ptr<CCollisionManager> pManger) { m_pColManager = pManger; }
	void SetFSMManager(shared_ptr<CFSMMgr> pManger) { m_pFSMMgr = pManger; }

protected: // 내부 함수

	virtual void BuildObjects(void *pContext = NULL);

	void CreatePathes();

protected: // 변수
	shared_ptr<CCollisionManager> m_pColManager;
	CHeightMapTerrain * m_pTerrain{ NULL };

	shared_ptr<CCreateMgr> m_pCreateMgr{ NULL };
	shared_ptr<CFSMMgr> m_pFSMMgr;

	std::unordered_map<ObjectType, NeutralObjectIndices> m_objectsIndices;

	Path m_pathes[6];
};
