#pragma once
#include "02.Framework/00.Frame/Framework.h"
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"

class CWayFinder;
class CCollisionManager;
class CThrowingMgr;
class CFSMMgr;
class CHeightMapTerrain;

class CScene	// Warning! 인 게임 씬이랑 공백 씬이랑 분리할 필요 있음
{
public:	// 생성자, 소멸자
	CScene();
	virtual ~CScene();

public: // 공개 함수
	virtual void Initialize();
	virtual void Finalize();

	void AnimateObjects(float timeElapsed);

protected: // 내부 함수
	void BuildObjects();
	void ReleaseObjects();

	void PickObjectPointedByCursor(WPARAM wParam, LPARAM lParam);
	void GenerateLayEndWorldPosition(XMFLOAT3& pickPosition, XMFLOAT4X4&	 xmf4x4View);

protected: // 변수
	CHeightMapTerrain * m_pTerrain{ NULL };

	CShader **m_ppShaders{ NULL };
	int m_nShaders{ 0 };

	XMFLOAT3 m_pickWorldPosition{ 0.f, 0.f, 0.f };

	// 매니저 클래스
	shared_ptr<CWayFinder> m_pWayFinder;
	shared_ptr<CCollisionManager>m_pCollisionManager;
	shared_ptr<CThrowingMgr> m_pThrowingMgr;
	shared_ptr<CFSMMgr> m_pFSMMgr;
};

