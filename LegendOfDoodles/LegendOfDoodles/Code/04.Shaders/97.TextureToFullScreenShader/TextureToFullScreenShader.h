#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CTextureToFullScreenShader : public CShader
{
public: // ������, �Ҹ���
	CTextureToFullScreenShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CTextureToFullScreenShader();

public: // ���� �Լ�
	virtual void CreateGraphicsRootSignature(shared_ptr<CCreateMgr> pCreateMgr);

	void CreateShaderResourceViews(shared_ptr<CCreateMgr> pCreateMgr, shared_ptr<CTexture> pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement, int index = 0);
	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, ComPtr<ID3D12RootSignature> pGraphicsRootSignature, UINT nRenderTargets = 1);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, shared_ptr<CTexture> pContext = NULL);

	virtual void Render(CCamera *pCamera);

	void SetGraphicsRootSignature() { m_pCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature.Get()); }

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

protected: // ����
	shared_ptr<CTexture> m_pTexture;
};

