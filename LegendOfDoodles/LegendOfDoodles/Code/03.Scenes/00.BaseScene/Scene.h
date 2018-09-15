#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"
#include "07.Network/Network.h"

class CCreateMgr;
class CSoundManager;

class CScene
{
public:	// 생성자, 소멸자
	CScene();
	virtual ~CScene();

public: // 공개 함수
	virtual void Initialize(shared_ptr<CCreateMgr> pCreateMgr, shared_ptr<CNetwork> pNetwork = nullptr);
	virtual void Finalize();

	virtual void ReleaseUploadBuffers();

	virtual void ProcessInput();
	virtual void AnimateObjects(float timeElapsed);
	virtual void Render();
	virtual void RenderWithLights();

	virtual void SetViewportsAndScissorRects();
	virtual void UpdateCamera();
	
	// Message Process
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT messageID,
		WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT messageID,
		WPARAM wParam, LPARAM lParam);

	virtual void RenderShadow() {}

	virtual void SetShadowViewportsAndScissorRects() {}
	virtual void UpdateShadowCamera(int renderStage = 0) { renderStage; }

	CCamera * GetCamera() { return m_pCamera; }
	CShader* GetShader(int index) { return m_ppShaders[index]; }

	virtual void ApplyPercentage(float pct) { pct; }

protected: // 내부 함수
	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr);
	virtual void ReleaseObjects();

	virtual void CreateShaderVariables(shared_ptr<CCreateMgr> pCreateMgr) {}
	virtual void ReleaseShaderVariables() {}
	virtual void UpdateShaderVariables() {}

	virtual void OnProcessKeyUp(WPARAM wParam, LPARAM lParam) { wParam; lParam; }

protected: // 변수
	HWND m_hWnd{ NULL };
	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;

	CCamera *m_pCamera{ NULL };

	CShader **m_ppShaders{ NULL };
	int m_nShaders{ 0 };

	// 매니저 클래스
	shared_ptr<CCreateMgr> m_pCreateMgr;
	shared_ptr<CSoundManager> m_pSoundManager;
};

