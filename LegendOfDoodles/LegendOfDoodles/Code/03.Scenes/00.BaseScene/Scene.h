#pragma once
#include "02.Framework/00.Frame/Framework.h"
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "07.Network/Network.h"

class CCreateMgr;
class CWayFinder;
class CCollisionManager;
class CUIObjectManager;
class CThrowingMgr;
class CEffectMgr;
class CFSMMgr;
class CNetwork;

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


struct FOW
{
	int m_bFoW[256];
};

class CScene	// Warning! 인 게임 씬이랑 공백 씬이랑 분리할 필요 있음
{
public:	// 생성자, 소멸자
	CScene();
	virtual ~CScene();

public: // 공개 함수
	virtual void Initialize(shared_ptr<CCreateMgr> pCreateMgr, shared_ptr<CNetwork> pNetwork);
	virtual void Finalize();

	void ReleaseUploadBuffers();

	void ProcessInput();
	void AnimateObjects(float timeElapsed);
	void Render();
	void RenderShadow();
	void RenderWithLights();

	void SetViewportsAndScissorRects();
	void SetShadowViewportsAndScissorRects();
	void UpdateCamera();
	void UpdateShadowCamera(int renderStage = 0);

	// Message Process
	void OnProcessingMouseMessage(HWND hWnd, UINT messageID,
		WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT messageID,
		WPARAM wParam, LPARAM lParam);

	CCamera * GetCamera() { return m_pCamera; }

protected: // 내부 함수
	void CreateCbvAndSrvDescriptorHeap(shared_ptr<CCreateMgr> pCreateMgr, int nConstantBufferViews, int nShaderResourceViews, int index);

	void BuildLights();

	void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr);
	void ReleaseObjects();

	virtual void CreateShaderVariables(shared_ptr<CCreateMgr> pCreateMgr);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables();

	void PickObjectPointedByCursor(WPARAM wParam, LPARAM lParam);
	void GenerateLayEndWorldPosition(XMFLOAT3& pickPosition, XMFLOAT4X4&	 xmf4x4View);

	virtual void OnProcessKeyUp(WPARAM wParam, LPARAM lParam);

protected: // 변수
	HWND m_hWnd{ NULL };
	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;

	CCamera *m_pCamera{ NULL };
	CCamera *m_pLightCamera{ NULL };

	shared_ptr<CNetwork> m_pNetwork;

	CShader **m_ppShaders{ NULL };
	int m_nShaders{ 0 };

	CMaterial			*m_pSketchEffect{ NULL };
	CMaterial			*m_pCubeMap{ NULL };
	LIGHTS	 *m_pLights{ NULL };

	ComPtr<ID3D12Resource>	m_pcbLights;
	LIGHTS *m_pcbMappedLights{ NULL };

	FOW		 *m_pFoW{ NULL };
	ComPtr<ID3D12Resource>	m_pcbbFoW;
	FOW *m_pcbMappedFoW = NULL;


	CCollisionObject ** m_ppObjects{ NULL };
	int m_nObjects{ 0 };

	CAnimatedObject * m_pMyPlayer{ NULL };

	bool m_bCurCamIsAOS{ true };
	bool m_bCamChanged{ false };
	bool m_bRenderBoundingBox{ false };

	XMFLOAT3 m_pickWorldPosition{ 0.f, 0.f, 0.f };

	// 매니저 클래스
	shared_ptr<CWayFinder> m_pWayFinder;
	shared_ptr<CCreateMgr> m_pCreateMgr;
	shared_ptr<CCollisionManager>m_pCollisionManager;
	shared_ptr<CUIObjectManager> m_pUIObjectsManager;
	shared_ptr<CThrowingMgr> m_pThrowingMgr;
	shared_ptr<CEffectMgr> m_pEffectMgr;
	shared_ptr<CFSMMgr> m_pFSMMgr;

	const int	m_nHeaps{ 2 };
	std::vector<ComPtr<ID3D12DescriptorHeap>>	m_pCbvSrvDescriptorHeaps;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_cbvCPUDescriptorStartHandles[2];
	D3D12_GPU_DESCRIPTOR_HANDLE		m_cbvGPUDescriptorStartHandles[2];
	D3D12_CPU_DESCRIPTOR_HANDLE		m_srvCPUDescriptorStartHandles[2];
	D3D12_GPU_DESCRIPTOR_HANDLE		m_srvGPUDescriptorStartHandles[2];
};

