#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

struct CB_SPRITE_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
	float m_percentage;
};

class CLoadingScreenShader : public CShader
{
public:	// ������, �Ҹ���
	CLoadingScreenShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CLoadingScreenShader();

public: // ���� �Լ�
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables(int opt = 0);
	virtual void Render(CCamera *pCamera);

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);
	virtual void ReleaseObjects();

protected: // ����
	CBaseObject *m_pBackground{ NULL };
};

