#pragma once
#include "03.Scenes/00.BaseScene/Scene.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"

class CWayFinder;
class CCollisionManager;
class CThrowingMgr;
class CFSMMgr;
class CHeightMapTerrain;

class CGameScene : public CScene
{
public:	// 생성자, 소멸자
	CGameScene();
	virtual ~CGameScene();

public: // 공개 함수
	virtual void Initialize();
	virtual void Finalize();

	virtual void AnimateObjects(float timeElapsed);
	virtual void GenerateLayEndWorldPosition(XMFLOAT3& pickPosition, int id);

	virtual CAnimatedObject** GetPlayerObject() { return  (CAnimatedObject**)m_ppShaders[1]->GetCollisionObjects();}
	virtual CAnimatedObject** GetNeutralityObject() { return (CAnimatedObject**)m_ppShaders[2]->GetCollisionObjects(); }
	virtual CCollisionObject** GetNexusTower() { return m_ppShaders[3]->GetCollisionObjects(); }

protected: // 내부 함수
	virtual void BuildObjects();

protected: // 변수
	CHeightMapTerrain * m_pTerrain{ NULL };

	// 매니저 클래스
	shared_ptr<CWayFinder> m_pWayFinder;
	shared_ptr<CCollisionManager>m_pCollisionManager;
	shared_ptr<CThrowingMgr> m_pThrowingMgr;
	shared_ptr<CFSMMgr> m_pFSMMgr;
};

