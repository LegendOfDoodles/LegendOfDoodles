#pragma once
#include "02.Framework/00.Frame/Framework.h"
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"

class CWayFinder;
class CCollisionManager;
class CFSMMgr;

class CScene	// Warning! 인 게임 씬이랑 공백 씬이랑 분리할 필요 있음
{
public:	// 생성자, 소멸자
	CScene();
	virtual ~CScene();

public: // 공개 함수
	virtual void Initialize();
	virtual void Finalize();

	void ProcessInput();
	void AnimateObjects(float timeElapsed);

	// Message Process
	void OnProcessingMouseMessage(HWND hWnd, UINT messageID,
		WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT messageID,
		WPARAM wParam, LPARAM lParam);

protected: // 내부 함수
	void BuildObjects();
	void ReleaseObjects();

	void GenerateLayEndWorldPosition(XMFLOAT3& pickPosition, XMFLOAT4X4&	 xmf4x4View);

	virtual void OnProcessKeyUp(WPARAM wParam, LPARAM lParam);

protected: // 변수
	CShader **m_ppShaders{ NULL };
	int m_nShaders{ 0 };

	CBaseObject ** m_ppObjects{ NULL };
	int m_nObjects{ 0 };

	CAnimatedObject * m_pSelectedObject{ NULL };

	CWayFinder* m_pWayFinder{ NULL };

	CCollisionManager *m_pCollisionManager{NULL};

	CFSMMgr * m_pFSMMgr{ NULL };
	// Warning! 터레인 생성해라
	CHeightMapTerrain *m_pTerrain{ NULL };
};

