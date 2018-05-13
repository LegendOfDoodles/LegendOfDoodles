#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CHeightMapTerrain;
class CCollisionManager;

class CPlayerShader : public CShader
{
public:
	CPlayerShader();
	~CPlayerShader();

public: // 공개 함수
	virtual void AnimateObjects(float timeElapsed);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);

	void SetColManagerToObject(CCollisionManager* manager);
	
	virtual CBaseObject **GetCollisionObjects() {	return m_ppObjects; }
	int GetObjectCount() { return m_nObjects;	}

protected: // 내부 함수
	virtual void BuildObjects(void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // 변수
	CBaseObject * *m_ppObjects{ NULL };
	int m_nObjects = 0;

	// Warning! 인트로 바꿔야함? -> 종류
	//CSkinnedMesh* m_pStick{ NULL };
	//CSkinnedMesh** m_pSword{ NULL };
	//CSkinnedMesh** m_pSteff{ NULL };
	//CSkinnedMesh** m_pBow{ NULL };

	CHeightMapTerrain * m_pTerrain{ NULL };
};
