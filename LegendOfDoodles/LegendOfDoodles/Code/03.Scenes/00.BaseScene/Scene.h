#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"
#include "07.Network/Network.h"

class CCreateMgr;
class CSoundManager;

class CScene
{
public:	// ������, �Ҹ���
	CScene();
	virtual ~CScene();

public: // ���� �Լ�
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

protected: // ���� �Լ�
	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr);
	virtual void ReleaseObjects();

	virtual void CreateShaderVariables(shared_ptr<CCreateMgr> pCreateMgr) {}
	virtual void ReleaseShaderVariables() {}
	virtual void UpdateShaderVariables() {}

	virtual void OnProcessKeyUp(WPARAM wParam, LPARAM lParam) { wParam; lParam; }

protected: // ����
	HWND m_hWnd{ NULL };
	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;

	CCamera *m_pCamera{ NULL };

	CShader **m_ppShaders{ NULL };
	int m_nShaders{ 0 };

	// �Ŵ��� Ŭ����
	shared_ptr<CCreateMgr> m_pCreateMgr;
	shared_ptr<CSoundManager> m_pSoundManager;
};

