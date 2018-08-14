#pragma once
#include "02.Framework/00.Frame/Framework.h"
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"

class CWayFinder;
class CCollisionManager;
class CThrowingMgr;
class CFSMMgr;
class CHeightMapTerrain;

class CScene	// Warning! �� ���� ���̶� ���� ���̶� �и��� �ʿ� ����
{
public:	// ������, �Ҹ���
	CScene();
	virtual ~CScene();

public: // ���� �Լ�
	virtual void Initialize();
	virtual void Finalize();

	void AnimateObjects(float timeElapsed);
	void GenerateLayEndWorldPosition(XMFLOAT3& pickPosition, int id);

	CAnimatedObject** GetPlayerObject() { return  (CAnimatedObject**)m_ppShaders[1]->GetCollisionObjects();}
	CAnimatedObject** GetNeutralityObject() { return (CAnimatedObject**)m_ppShaders[2]->GetCollisionObjects(); }
	CCollisionObject** GetNexusTower() { return m_ppShaders[3]->GetCollisionObjects(); }
	CShader* GetShader(int num) { return m_ppShaders[num]; }

protected: // ���� �Լ�
	void BuildObjects();
	void ReleaseObjects();


protected: // ����
	CHeightMapTerrain * m_pTerrain{ NULL };

	CShader **m_ppShaders{ NULL };
	int m_nShaders{ 0 };

	// �Ŵ��� Ŭ����
	shared_ptr<CWayFinder> m_pWayFinder;
	shared_ptr<CCollisionManager>m_pCollisionManager;
	shared_ptr<CThrowingMgr> m_pThrowingMgr;
	shared_ptr<CFSMMgr> m_pFSMMgr;
};

