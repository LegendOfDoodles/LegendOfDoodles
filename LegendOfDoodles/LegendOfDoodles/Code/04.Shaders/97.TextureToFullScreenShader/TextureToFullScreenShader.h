#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CTextureToFullScreenShader : public CShader
{
public: // ������, �Ҹ���
	CTextureToFullScreenShader(CCreateMgr *pCreateMgr);
	virtual ~CTextureToFullScreenShader();

public: // ���� �Լ�
	virtual void CreateGraphicsRootSignature(CCreateMgr *pCreateMgr);

	virtual void CreateShader(CCreateMgr *pCreateMgr, ID3D12RootSignature *pd3dGraphicsRootSignature, UINT nRenderTargets = 1);

	virtual void BuildObjects(CCreateMgr * pCreateMgr, void *pContext = NULL);

	virtual void Render(CCamera *pCamera);

	void SetGraphicsRootSignature() { m_pCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature); }

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void ReleaseObjects();

protected: // ����
	CTexture * m_pTexture{ NULL };
};

