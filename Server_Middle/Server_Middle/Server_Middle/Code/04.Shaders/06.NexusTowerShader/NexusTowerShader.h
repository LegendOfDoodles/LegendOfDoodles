
#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CHeightMapTerrain;
class CCollisionManager;

class CNexusTowerShader : public CShader
{
public: // 생성자, 소멸자
	CNexusTowerShader();
	virtual ~CNexusTowerShader();

public: // 공개 함수
	virtual void AnimateObjects(float timeElapsed);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);

	void SetColManagerToObject(CCollisionManager* manager);

	int GetObjectCount() { return m_nObjects; }
	virtual CBaseObject **GetCollisionObjects() { return m_ppObjects; }

protected: // 내부 함수
	virtual void BuildObjects(void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // 변수
	CBaseObject * *m_ppObjects{ NULL };
	int m_nObjects{ 0 };

	CHeightMapTerrain * m_pTerrain{ NULL };
};