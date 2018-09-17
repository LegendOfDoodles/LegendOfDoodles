#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CLogoShader : public CShader
{
public:	// 생성자, 소멸자
	CLogoShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CLogoShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables(int opt = 0);

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual bool IsShaderDone() { return (m_loadPercentage >= TIME_LOGO_SHOW); }

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);
	virtual void ReleaseObjects();

protected: // 변수
	CBaseObject *m_pLogoScreen{ NULL };
	float m_loadPercentage{ 0.f };
};

