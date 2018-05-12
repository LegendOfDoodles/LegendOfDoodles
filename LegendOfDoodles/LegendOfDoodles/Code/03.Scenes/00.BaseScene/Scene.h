#pragma once
#include "02.Framework/00.Frame/Framework.h"
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"

class CCreateMgr;
class CWayFinder;
class CCollisionManager;
class CHPGaugeManager;
class CFSMMgr;

struct LIGHT
{
	XMFLOAT4				m_color;
	XMFLOAT3				m_position;
	float 					m_fFalloff;
	XMFLOAT3				m_direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
};

class CScene	// Warning! 인 게임 씬이랑 공백 씬이랑 분리할 필요 있음
{
public:	// 생성자, 소멸자
	CScene();
	virtual ~CScene();

public: // 공개 함수
	virtual void Initialize(CCreateMgr *pCreateMgr, Network* pNetwork);
	virtual void Finalize();

	void ReleaseUploadBuffers();

	void ProcessInput();
	void AnimateObjects(float timeElapsed);
	void Render();

	void SetViewportsAndScissorRects();
	void UpdateCamera();

	// Message Process
	void OnProcessingMouseMessage(HWND hWnd, UINT messageID,
		WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT messageID,
		WPARAM wParam, LPARAM lParam);

	CCamera * GetCamera() { return m_pCamera; }

protected: // 내부 함수
	void BuildLights();

	void BuildObjects(CCreateMgr *pCreateMgr);
	void ReleaseObjects();

	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables();

	void PickObjectPointedByCursor(WPARAM wParam, LPARAM lParam);
	void GenerateLayEndWorldPosition(XMFLOAT3& pickPosition, XMFLOAT4X4&	 xmf4x4View);

	virtual void OnProcessKeyUp(WPARAM wParam, LPARAM lParam);

	void CollisionTest();

protected: // 변수
	HWND m_hWnd{ NULL };
	ID3D12GraphicsCommandList *m_pCommandList{ NULL };

	CCamera *m_pCamera{ NULL };

	CShader **m_ppShaders{ NULL };
	int m_nShaders{ 0 };

	LIGHTS	 *m_pLights = NULL;

	ID3D12Resource	*m_pd3dcbLights = NULL;
	LIGHTS *m_pcbMappedLights = NULL;

	CBaseObject ** m_ppObjects{ NULL };
	int m_nObjects{ 0 };

	CAnimatedObject * m_pSelectedObject{ NULL };

	bool m_bCurCamIsAOS{ true };
	bool m_bCamChanged{ false };
	bool m_bRenderBoundingBox{ false };

	XMFLOAT3 m_pickWorldPosition{ 0.f, 0.f, 0.f };

	CWayFinder* m_pWayFinder{ NULL };

	CCreateMgr* m_pCreateMgr{ NULL };

	CCollisionManager *m_pCollisionManager{NULL};
	CHPGaugeManager *m_pHPGaugeManager{NULL};

	CFSMMgr * m_pFSMMgr{ NULL };

	Network* m_pNetwork;
	
	int m_FrameCheck = 0;
};

