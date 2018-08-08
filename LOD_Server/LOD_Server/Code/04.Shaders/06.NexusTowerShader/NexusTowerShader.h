#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CMaterial;
class CHeightMapTerrain;
class CCollisionManager;
class CFSMMgr;

class CNexusTowerShader : public CShader
{
public: // 생성자, 소멸자
	CNexusTowerShader();
	virtual ~CNexusTowerShader();

public: // 공개 함수
	virtual void Initialize(void *pContext = NULL);

	virtual void AnimateObjects(float timeElapsed);

	void SetColManagerToObject(shared_ptr<CCollisionManager> manager);
	void SetThrowingManagerToObject(shared_ptr<CThrowingMgr> manager);

	int GetNexusCount() { return m_nNexus; }
	int GetTowerCount() { return m_nTower; }

	void SetFSMManager(shared_ptr<CFSMMgr> pManger) { m_pFSMMgr = pManger; }

protected: // 내부 함수
	virtual void BuildObjects(void *pContext = NULL);

protected: // 변수
	int m_nNexus{ 0 };
	int m_nTower{ 0 };

	int m_meshCounts[4];

	CHeightMapTerrain * m_pTerrain{ NULL };

	shared_ptr<CFSMMgr> m_pFSMMgr;
};