#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CHeightMapTerrain;
class CCollisionManager;

class CPlayerShader : public CShader
{
public:
	CPlayerShader();
	~CPlayerShader();

public: // ���� �Լ�
	virtual void AnimateObjects(float timeElapsed);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);

	void SetColManagerToObject(CCollisionManager* manager);
	
	virtual CBaseObject **GetCollisionObjects() {	return m_ppObjects; }
	int GetObjectCount() { return m_nObjects;	}

protected: // ���� �Լ�
	virtual void BuildObjects(void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // ����
	CBaseObject * *m_ppObjects{ NULL };
	int m_nObjects = 0;

	// Warning! ��Ʈ�� �ٲ����? -> ����
	//CSkinnedMesh* m_pStick{ NULL };
	//CSkinnedMesh** m_pSword{ NULL };
	//CSkinnedMesh** m_pSteff{ NULL };
	//CSkinnedMesh** m_pBow{ NULL };

	CHeightMapTerrain * m_pTerrain{ NULL };
};
