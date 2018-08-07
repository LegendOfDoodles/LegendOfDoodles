#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/05.SkyBox/SkyBox.h"

class CSkyBoxShader : public CShader
{
public: // ������, �Ҹ���
	CSkyBoxShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CSkyBoxShader();

public: // ���� �Լ�
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables(int opt = 0);

	virtual void Render(CCamera *pCamera);

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // ����
	CSkyBox * m_pSkyBox{ NULL };
	CFloor *m_pFloor{ NULL };
};

